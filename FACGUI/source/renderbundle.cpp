#include "renderbundle.h"

FACGUI::RenderBundle::RenderBundle(QWidget *parent) : QOpenGLWidget(parent) {
    this->setLocale(QLocale::C);
    this->pool = &FAC::FiberPool::getInstance();
    pool->addObserver(this);
    this->data = RenderData::getInstance();
    this->timer = nullptr;

    setFocusPolicy(Qt::FocusPolicy::StrongFocus);
} // RenderBundle::RenderBundle

FACGUI::RenderBundle::~RenderBundle() {
    pool->removeObserver(this);
    makeCurrent();
    teardownGL();
} // RenderBundle::~RenderBundle

void FACGUI::RenderBundle::initializeGL() {
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glClearColor(0.75f, 0.75f, 0.75f, 1.0f);

    // Fiber bundles
    m_shaderProgram = new QOpenGLShaderProgram(this);
    m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/3dshader.vert");
    m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/3dshader.frag");
    switch(FACGUI::Settings::rendering()) {
    case FACGUI::RenderingOption::Filled:
        m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/3dshaderfilled.geom");
        break;
    case FACGUI::RenderingOption::Hollow:
        m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/3dshaderhollow.geom");
        break;
    case FACGUI::RenderingOption::Lines:
        m_shaderProgram->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/3dshaderlines.geom");
        break;
    }
    m_shaderProgram->link();
    m_shaderProgram->bind();

    u_modelToWorld = m_shaderProgram->uniformLocation("modelToWorld");
    u_worldToCamera = m_shaderProgram->uniformLocation("worldToCamera");
    u_cameraPos = m_shaderProgram->uniformLocation("cameraPos");
    u_lightPos = m_shaderProgram->uniformLocation("lightPos");
    u_corners = m_shaderProgram->uniformLocation("corners");
    m_shaderProgram->setUniformValue(u_corners, m_corners);

    m_buffer.create();
    m_buffer.bind();
    m_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_buffer.allocate(data->data().data(), int(data->data().size()) * Vertex::stride());

    m_vao.create();
    m_vao.bind();
    m_shaderProgram->enableAttributeArray(0);
    m_shaderProgram->enableAttributeArray(1);
    m_shaderProgram->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionSize, Vertex::stride());
    m_shaderProgram->setAttributeBuffer(1, GL_FLOAT, Vertex::infoOffset(), Vertex::InfoSize, Vertex::stride());

    m_vao.release();
    m_buffer.release();
    m_shaderProgram->release();

    // Color bubble
    m_bubbleShaderProgram = new QOpenGLShaderProgram(this);
    m_bubbleShaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/bubble.vert");
    m_bubbleShaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/bubble.frag");
    m_bubbleShaderProgram->link();
    m_bubbleShaderProgram->bind();

    auto radius = 0.1;
    auto NUMBER_OF_VERTICES=100;
    for(double i = 0; i < 2 * M_PI + (2 * M_PI / NUMBER_OF_VERTICES); i += 2 * M_PI / NUMBER_OF_VERTICES){
        m_bubbleData.push_back(QVector3D((1-radius) + cos(i) * radius, -(1-radius) + sin(i) * radius, 0.0));
        m_bubbleData.push_back(QVector3D((1-radius), -(1-radius), radius));
    }

    m_bubbleShaderProgram->setUniformValue(m_bubbleShaderProgram->uniformLocation("XMID"), float((1.0f-radius)));
    m_bubbleShaderProgram->setUniformValue(m_bubbleShaderProgram->uniformLocation("YMID"), float(-(1.0f-radius)));
    m_bubbleShaderProgram->setUniformValue(m_bubbleShaderProgram->uniformLocation("radius"), float(radius));
    u_bubbleRotation = m_bubbleShaderProgram->uniformLocation("rotation");
    u_bubbleRGB = m_bubbleShaderProgram->uniformLocation("rgb");
    m_bubbleShaderProgram->setUniformValue(u_bubbleRGB, true);

    m_bubbleBuffer.create();
    m_bubbleBuffer.bind();
    m_bubbleBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_bubbleBuffer.allocate(m_bubbleData.data(), int(m_bubbleData.size() * sizeof(QVector3D)));

    m_bubbleVao.create();
    m_bubbleVao.bind();
    m_bubbleShaderProgram->enableAttributeArray(0);
    m_bubbleShaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(QVector3D));

    fps_timer.start();
    frameCounter = 0;
    m_corners = 11;
    sleep = false;
} // RenderBundle::initializeGL

void FACGUI::RenderBundle::resizeGL(int width, int height) {
    m_projection.setToIdentity();
    QOpenGLWidget::resizeGL(width, height);
} // RenderBundle::resizeGL

void FACGUI::RenderBundle::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_shaderProgram->bind();
    m_vao.bind();
    m_shaderProgram->setUniformValue(u_worldToCamera, m_projection);
    m_shaderProgram->setUniformValue(u_modelToWorld, m_transform.toMatrix());
    m_shaderProgram->setUniformValue(u_cameraPos, m_transform.translation());
    m_shaderProgram->setUniformValue(u_lightPos, m_transform.rotation() * m_transform.translation());
    m_shaderProgram->setUniformValue(u_corners, m_corners);
    glDrawArrays(GL_LINES, 0, int(data->data().size()));
    m_vao.release();
    m_shaderProgram->release();

    m_bubbleShaderProgram->bind();
    m_bubbleVao.bind();
    m_bubbleShaderProgram->setUniformValue(u_bubbleRGB, data->color() == 0);
    m_bubbleShaderProgram->setUniformValue(u_bubbleRotation,  m_transform.rotation().toRotationMatrix());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, int(this->m_bubbleData.size()));
    m_bubbleVao.release();
    m_bubbleShaderProgram->release();

    ++frameCounter;
    if(frameCounter >= 10) {
        auto time = fps_timer.elapsed()/frameCounter;
        if(time > 25) {
            if(m_corners > 5) --m_corners;
        } else if(time < 20){
            if(m_corners < 11) ++m_corners;
        }
        frameCounter = 0;
        fps_timer.restart();
    }
    sleep = true;
} // RenderBundle::paintGL

void FACGUI::RenderBundle::teardownGL() {
    m_vao.destroy();
    m_buffer.destroy();
    if(m_shaderProgram) delete m_shaderProgram;

    m_bubbleVao.destroy();
    m_bubbleBuffer.destroy();
    if(m_bubbleShaderProgram) delete m_bubbleShaderProgram;
} // RenderBundle::teardownGL

bool FACGUI::RenderBundle::event(QEvent *event) {
    if(event->type() == 1005) {
        updateBuffer();
        return true;
    }
    return QOpenGLWidget::event(event);
}

void FACGUI::RenderBundle::updateBuffer() {
    this->setResetValues();
    m_buffer.bind();
    m_buffer.allocate(data->data().data(), int(data->data().size() * Vertex::stride()));
    m_buffer.release();
    sleep = false;
    update();
}

void FACGUI::RenderBundle::setVisible(bool visible) {
    if(visible) {
        if(timer == nullptr) {
            timer = new QTimer(this);
            timer->setSingleShot(false);
            timer->setTimerType(Qt::PreciseTimer);
            timer->start(DESIRED_FRAME_TIME);
            connect(timer, &QTimer::timeout, this, &RenderBundle::update);
        }
    } else {
        if(timer != nullptr) {
            disconnect(timer, &QTimer::timeout, this, &RenderBundle::update);
            delete timer;
            timer = nullptr;
        }
    }
    QOpenGLWidget::setVisible(visible);
    sleep = false;
}

void FACGUI::RenderBundle::update() {
    if(!sleep) {
        QOpenGLWidget::update();
    } else {
        fps_timer.revertBy(DESIRED_FRAME_TIME);
    }
}

void FACGUI::RenderBundle::setAngle(float x, float y, float z) {
    m_lightPos = QVector3D(x, y, z);
    m_transform.rotateBy(90, x, z, y);
    this->standardRotation = m_transform.rotation();
}

void FACGUI::RenderBundle::resetView() {
    m_transform.reset();
    m_transform.rotateBy(standardRotation);

    m_transform.setMiddlePoint(-(max_x+min_x)/2, -(max_y+min_y)/2, -(max_z+min_z)/2);

    m_transform.scaleBy(qMin(1000.0f, 1/scale));
    sleep = false;
}

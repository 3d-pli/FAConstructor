#include "render3d.h"

FACGUI::Render3D::Render3D(QWidget *parent) : RenderBundle(parent) {
    this->setResetValues();
    this->resetView();
} // Render3D::Render3D

void FACGUI::Render3D::initializeGL() {
    RenderBundle::initializeGL();
} // Render3D::initializeGL

void FACGUI::Render3D::resizeGL(int width, int height) {
    RenderBundle::resizeGL(width, height);
    m_projection.perspective(30.0f, width/float(height), 0.1f, 1000.0f);
    //m_projection.ortho(-1, 1, -1, 1, 0.1f, 1000.0f);
} // Render3D::resizeGL

void FACGUI::Render3D::paintGL() {
    RenderBundle::paintGL();
} // Render3D::paintGL

void FACGUI::Render3D::keyPressEvent(QKeyEvent *event) {
    const float moveScale = 0.1f;

    QVector3D movement(0, 0, 0);

    switch(event->key()) {
    case Qt::Key_W:
        movement += -QVector3D(0, 1, 0);
        break;
    case Qt::Key_S:
        movement += QVector3D(0, 1, 0);
        break;
    case Qt::Key_D:
        movement += -QVector3D(1, 0, 0);
        break;
    case Qt::Key_A:
        movement += QVector3D(1, 0, 0);
        break;
    case Qt::Key_R:
        this->resetView();
        break;
    }
    m_transform.translateBy(moveScale * movement);
    sleep = false;
}

void FACGUI::Render3D::mousePressEvent(QMouseEvent *event) {
    this->startPos = event->pos();
    if(event->button() == Qt::MouseButton::LeftButton) {
        this->leftPressed = true;
    }
    if(this->rotateAble) {
        if(event->button() == Qt::MouseButton::RightButton) {
            this->rightPressed = true;
        }
        if(event->button() == Qt::MouseButton::MiddleButton) {
            this->middlePressed = true;
        }
    }
    sleep = false;
}

void FACGUI::Render3D::mouseMoveEvent(QMouseEvent *event) {
    const float moveScale = 0.1f * m_transform.scale().x();
    if(leftPressed) {
        QVector3D movement(startPos);
        movement -= QVector3D(event->pos());
        movement.setX(-movement.x());
        movement *= moveScale;
        m_transform.translateBy(movement);
        startPos = event->pos();
    }
    if(rightPressed) {
        QVector3D movement(startPos);
        movement -= QVector3D(event->pos());
        m_transform.rotateBy(-movement.x(), QVector3D(0, 1, 0));
        m_transform.rotateBy(-movement.y(), QVector3D(1, 0, 0));
        startPos = event->pos();
    }
    if(middlePressed) {
        QVector3D movement(startPos);
        movement -= QVector3D(event->pos());
        m_transform.rotateBy(movement.x(), QVector3D(0, 0, 1));
        startPos = event->pos();
    }
    sleep = false;
}

void FACGUI::Render3D::mouseReleaseEvent(QMouseEvent *event) {
    if(event->button() == Qt::MouseButton::LeftButton) {
        this->leftPressed = false;
    }
    if(event->button() == Qt::MouseButton::RightButton) {
        this->rightPressed = false;
    }
    if(event->button() == Qt::MouseButton::MiddleButton) {
        this->middlePressed = false;
    }
    sleep = false;
}

void FACGUI::Render3D::wheelEvent(QWheelEvent *event) {
    const float growScale = 0.1f;
    m_transform.scaleBy((event->delta() > 0 ? growScale : -growScale) + 1);
    sleep = false;
}

bool FACGUI::Render3D::event(QEvent *event) {
    if(event->type() == 1004) {
        FAC::Plugindata::setRotation(m_transform.rotation());
        return true;
    } else return FACGUI::RenderBundle::event(event);
    return false;
}

void FACGUI::Render3D::setResetValues() {
    if(data->data().empty()) {
        min_x = min_y = -1;
        max_x = max_y = 1;
        min_z = 0;
        max_z = 5;
    }
    else {
        max_x = max_y = max_z = INT_MIN;
        min_x = min_y = min_z = INT_MAX;

        for(auto element : data->data()) {
            QVector3D pos = element.position();
            if(pos.x() < min_x) {
                min_x = pos.x();
            }
            if(pos.x() > max_x) {
                max_x = pos.x();
            }

            if(pos.y() < min_y) {
                min_y = pos.y();
            }

            if(pos.y() > max_y) {
                max_y = pos.y();
            }

            if(pos.z() < min_z) {
                min_z = pos.z();
            }

            if(pos.z() > max_z) {
                max_z = pos.z();
            }
        }
    }
    scale = qMax(qAbs(max_x)+qAbs(min_x), qAbs(max_y)+qAbs(min_y));
}

void FACGUI::Render3D::resetView() {
    RenderBundle::resetView();
    m_transform.translateBy(0.0f, 0.0f, -1.0f);
    m_transform.scaleBy(0.8f);
    sleep = false;
}

#include "render2d.h"

FACGUI::Render2D::Render2D(QWidget *parent) : RenderBundle(parent) {
    this->setResetValues();
    this->resetView();
} // Render2D::Render2D

void FACGUI::Render2D::initializeGL() {
    RenderBundle::initializeGL();
} // Render2D::initializeGL

void FACGUI::Render2D::resizeGL(int width, int height) {
    RenderBundle::resizeGL(width, height);
    m_projection.ortho(-1, 1, -1, 1, -0.1, 0.1);
} // Render2D::resizeGL

void FACGUI::Render2D::paintGL() {
    RenderBundle::paintGL();
} // Render2D::paintGL

void FACGUI::Render2D::keyPressEvent(QKeyEvent *event) {
    const float moveScale = 0.05f;

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
    case Qt::Key_Q:
        movement += QVector3D(0, 0, 0.25f);
        break;
    case Qt::Key_E:
        movement += -QVector3D(0, 0, 0.25f);
        break;
    case Qt::Key_R:
        this->resetView();
        break;
    }
    m_transform.translateBy(moveScale * movement);
    sleep = false;
}

void FACGUI::Render2D::mousePressEvent(QMouseEvent *event) {
    this->startPos = event->pos();
    if(event->button() == Qt::MouseButton::LeftButton) {
        this->leftPressed = true;
    }
    sleep = false;
}

void FACGUI::Render2D::mouseMoveEvent(QMouseEvent *event) {
    const float moveScale = 0.1f * m_transform.scale().x();
    if(leftPressed) {
        QVector3D movement(startPos);
        movement -= QVector3D(event->pos());
        movement.setX(-movement.x());
        movement *= moveScale;
        m_transform.translateBy(movement);
        startPos = event->pos();
    }
    sleep = false;
}

void FACGUI::Render2D::mouseReleaseEvent(QMouseEvent *event) {
    if(event->button() == Qt::MouseButton::LeftButton) {
        this->leftPressed = false;
    }
    sleep = false;
}

void FACGUI::Render2D::wheelEvent(QWheelEvent *event) {
    const float growScale = 0.1f;
    m_transform.scaleBy((event->delta() > 0 ? growScale : -growScale) + 1);
    sleep = false;
}

void FACGUI::Render2D::setResetValues() {
    if(data->data().empty()) {
        min_x = min_y = 0;
        max_x = max_y = 0;
        min_z = 0;
        max_z = 0;
    }
    else {
        max_x = max_y = max_z = INT_MIN;
        min_x = min_y = min_z = INT_MAX;

            for(auto element : data->data()) {
            QVector3D pos = element.position();
            if(pos.x() < min_x) {
                min_x = pos.x();
            } else if(pos.x() > max_x) {
                max_x = pos.x();
            }

            if(pos.y() < min_y) {
                min_y = pos.y();
            } else if(pos.y() > max_y) {
                max_y = pos.y();
            }

            if(pos.z() < min_z) {
                min_z = pos.z();
            } else if(pos.z() > max_z) {
                max_z = pos.z();
            }
        }
    }

    auto orientation = this->standardRotation.toVector4D();
    if(orientation.x() > 0) {
        scale = qMax(qAbs(max_x)+qAbs(min_x), qAbs(max_y)+qAbs(min_y));
    } else if(orientation.y() > 0) {
        scale = qMax(qAbs(max_y)+qAbs(min_y), qAbs(max_z)+qAbs(min_z));
    } else if(orientation.z() > 0) {
        scale = qMax(qAbs(max_x)+qAbs(min_x), qAbs(max_z)+qAbs(min_z));
    }
}

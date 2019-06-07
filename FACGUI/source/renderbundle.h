#ifndef RENDERBUNDLE_H
#define RENDERBUNDLE_H
#define GL_SILENCE_DEPRECATION
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>
#include <QString>
#include <QEvent>
#include <QKeyEvent>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <climits>
#include "elapsedtimer.h"
#include "fiberpool.h"
#include "helpers.h"
#include "renderdata.h"
#include "settings.h"
#include "transformation3d.h"
#include "vertex.h"
#include "widgetdef.h"

#define DESIRED_FRAME_TIME 16

namespace FACGUI {
    class RenderBundle : public QOpenGLWidget,
                         protected QOpenGLFunctions
    {
        Q_OBJECT
        MAKE_TESTABLE(RenderBundle)
    public:
        RenderBundle(QWidget* parent = nullptr);
        ~RenderBundle();

        void initializeGL();
        void resizeGL(int width, int height);
        void paintGL();
        void teardownGL();
        void update();

        bool event(QEvent *event);
        void setAngle(float x, float y, float z);

        virtual void keyPressEvent(QKeyEvent * event) = 0;
        virtual void mousePressEvent(QMouseEvent * event) = 0;
        virtual void mouseReleaseEvent(QMouseEvent * event) = 0;
        virtual void mouseMoveEvent(QMouseEvent * event) = 0;
        virtual void wheelEvent(QWheelEvent * event) = 0;
        void setVisible(bool visible);

    public slots:
        virtual void resetView();

    protected:

        virtual void setResetValues() = 0;
        void updateBuffer();

        float min_x, max_x, min_y, max_y, min_z, max_z, scale;

        FAC::FiberPool * pool;
        RenderData * data;
        QQuaternion standardRotation;

        bool leftPressed = false;
        bool rightPressed = false;
        bool middlePressed = false;
        bool rotateAble = true;
        QPoint startPos;

        /* Main buffer */
        int u_modelToWorld;
        int u_worldToCamera;
        int u_cameraPos;
        int u_lightPos;
        int u_corners;

        QOpenGLBuffer m_buffer;
        QOpenGLShaderProgram* m_shaderProgram = nullptr;
        QOpenGLVertexArrayObject m_vao;
        QMatrix4x4 m_projection;
        Transformation3D m_transform;
        QVector3D m_lightPos;
        int m_corners;

        /* Color bubble */
        int u_bubbleRotation;
        int u_bubbleRGB;
        QOpenGLBuffer m_bubbleBuffer;
        QOpenGLShaderProgram* m_bubbleShaderProgram = nullptr;
        QOpenGLVertexArrayObject m_bubbleVao;
        std::vector<QVector3D> m_bubbleData;

        bool sleep = false;
        bool orthoView;
        QTimer * timer;

        ElapsedTimer fps_timer;
        unsigned frameCounter;
    };
}

#endif // RENDERBUNDLE_H

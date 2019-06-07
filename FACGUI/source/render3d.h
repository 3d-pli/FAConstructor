#ifndef Render3D_H
#define Render3D_H
#include "renderbundle.h"
#include "plugindata.h"

namespace FACGUI {
    class Render3D : public RenderBundle
    {
        Q_OBJECT
        MAKE_TESTABLE(Render3D)
    public:
        Render3D(QWidget* parent = nullptr);
        void initializeGL();
        void resizeGL(int width, int height);
        void paintGL();

        void keyPressEvent(QKeyEvent * event);
        void mousePressEvent(QMouseEvent * event);
        void mouseReleaseEvent(QMouseEvent * event);
        void mouseMoveEvent(QMouseEvent * event);
        void wheelEvent(QWheelEvent * event);
        bool event(QEvent * event);

        void resetView();

    protected:
        void setResetValues();
    };
}

#endif // RENDERWIDGET_H

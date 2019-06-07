#ifndef Render2D_H
#define Render2D_H
#include "renderbundle.h"

namespace FACGUI {
    class Render2D : public FACGUI::RenderBundle
    {
        Q_OBJECT
        MAKE_TESTABLE(Render2D)
    public:
        Render2D(QWidget* parent = nullptr);
        void initializeGL();
        void resizeGL(int width, int height);
        void paintGL();

        void keyPressEvent(QKeyEvent * event);
        void mousePressEvent(QMouseEvent * event);
        void mouseReleaseEvent(QMouseEvent * event);
        void mouseMoveEvent(QMouseEvent * event);
        void wheelEvent(QWheelEvent * event);
    protected:
        void setResetValues();
    };
}

#endif // RENDERWIDGET_H

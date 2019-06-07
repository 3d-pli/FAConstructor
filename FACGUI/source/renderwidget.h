#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QSurfaceFormat>
#include <QGridLayout>
#include <vector>
#include "render2d.h"
#include "render3d.h"
#include "renderdata.h"
#include "widgetdef.h"

namespace FACGUI {
    class RenderWidget : public QWidget
    {
        Q_OBJECT
        MAKE_TESTABLE(RenderWidget)
    public:
        explicit RenderWidget(QWidget *parent = nullptr);
        static void setFormat(QSurfaceFormat& format);
        QImage getImages();

    public slots:
        void resetView();
        void showX();
        void showY();
        void showZ();
        void show3D();
        void showAll();

    private:
        static QSurfaceFormat format;

        RenderBundle* x_axis;
        RenderBundle* y_axis;
        RenderBundle* z_axis;
        RenderBundle* all_axis;
    };
}

#endif // RENDERWIDGET_H

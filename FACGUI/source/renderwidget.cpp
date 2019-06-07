#include "renderwidget.h"
#include <QDebug>

QSurfaceFormat FACGUI::RenderWidget::format = QSurfaceFormat::defaultFormat();

FACGUI::RenderWidget::RenderWidget(QWidget *parent) : QWidget(parent)
{
    QGridLayout * gridLayout = new QGridLayout(this);

    x_axis = new Render2D(this);
    x_axis->setFormat(format);
    x_axis->makeCurrent();
    x_axis->setAngle(1, 0, 0);
    x_axis->show();
    y_axis = new Render2D(this);
    y_axis->setFormat(format);
    y_axis->makeCurrent();
    y_axis->setAngle(0, 1, 0);
    y_axis->show();
    z_axis = new Render2D(this);
    z_axis->setFormat(format);
    z_axis->makeCurrent();
    z_axis->setAngle(0, 0, 1);
    z_axis->show();
    all_axis = new Render3D(this);
    all_axis->setFormat(format);
    all_axis->makeCurrent();
    all_axis->show();

    show3D();

    gridLayout->addWidget(x_axis, 0, 0);
    gridLayout->addWidget(y_axis, 0, 1);
    gridLayout->addWidget(z_axis, 1, 0);
    gridLayout->addWidget(all_axis, 1, 1);
}

void FACGUI::RenderWidget::setFormat(QSurfaceFormat& format) {
    RenderWidget::format = format;
}

QImage FACGUI::RenderWidget::getImages() {
    QPixmap pixmap(this->size());
    this->render(&pixmap, QPoint(), QRegion(QRect(QPoint(0,0), this->size())));
    return pixmap.toImage();
}

void FACGUI::RenderWidget::resetView() {
    x_axis->resetView();
    y_axis->resetView();
    z_axis->resetView();
    all_axis->resetView();
}

void FACGUI::RenderWidget::showX() {
    x_axis->setVisible(true);
    y_axis->setVisible(false);
    z_axis->setVisible(false);
    all_axis->setVisible(false);
}

void FACGUI::RenderWidget::showY() {
    x_axis->setVisible(false);
    y_axis->setVisible(true);
    z_axis->setVisible(false);
    all_axis->setVisible(false);
}

void FACGUI::RenderWidget::showZ() {
    x_axis->setVisible(false);
    y_axis->setVisible(false);
    z_axis->setVisible(true);
    all_axis->setVisible(false);
}

void FACGUI::RenderWidget::show3D() {
    x_axis->setVisible(false);
    y_axis->setVisible(false);
    z_axis->setVisible(false);
    all_axis->setVisible(true);
}

void FACGUI::RenderWidget::showAll() {
    x_axis->setVisible(true);
    y_axis->setVisible(true);
    z_axis->setVisible(true);
    all_axis->setVisible(true);
}

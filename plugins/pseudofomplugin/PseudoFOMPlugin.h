#ifndef PSEUDOFOMPLUGIN_H
#define PSEUDOFOMPLUGIN_H
#include <vector>
#include <string>
#include <algorithm>
#include <math.h>
#include <iostream>
#include <QFile>
#include <QtWidgets>
#include <QMessageBox>
#include <QObject>
#include <QVector4D>
#include <QVector3D>
#include <fiber.h>
#include <fiberpool.h>
#include <fiberpoint.h>
#include <plugindata.h>
#include <opencv2/opencv.hpp>
#include "plugininterface.h"
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QDialog>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QLabel>
#include <QFileDialog>
#include <QProgressDialog>

class PseudoFOMPlugin : public QObject, FACPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.inm-1.FAConstructor.ExamplePlugin")
    Q_INTERFACES( FACPluginInterface )

private:
    int xrange[2], yrange[2], zrange[2];
    int xsize, ysize;
    float xrotation, yrotation, zrotation;
    double sfactor = 50; //Skalierungsfaktor, gibt an wie viel Pixel 1 entsprechen
    int spacing = 100;
    double proportion;
    int color;
    float thickness;
    bool blockY = false, blockX = false;

    std::vector<std::vector<FAC::Fiber>> fizl; //Fibers in Z-Level
    std::string output_dir;

    int getPoolSize();
    int getZLevel(QVector3D fibpoint, float stepsize);
    void initFIZL(float stepsize);
    void drawPseudoFOM();
    QVector3D rotate(QVector3D position) const;
    bool showDialog();
    void initRanges();

public slots:
    void scaleYValue(int i);
    void scaleXValue(int i);

signals:
    void yValueScaled(int i);
    void xValueScaled(int i);


public:
    QVector3D hsv2bgr(QVector3D c);
    cv::Scalar toColor(QVector3D vec);
    void run();
};
#endif

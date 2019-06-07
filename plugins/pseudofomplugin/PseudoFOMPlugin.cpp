#include "PseudoFOMPlugin.h"

using namespace std;

void PseudoFOMPlugin::run() {
    if(getPoolSize() > 0) {
        xrotation = FAC::Plugindata::getRotX()*M_PI;
        yrotation = FAC::Plugindata::getRotY()*M_PI;
        zrotation = FAC::Plugindata::getRotZ()*M_PI;
        initRanges();
        proportion = abs(xrange[1]-xrange[0])/(double) abs(yrange[1]-yrange[0]);
        while(true) {
            if(showDialog()) {
                try {
                    sfactor = xsize-spacing;
                    sfactor /= abs(xrange[1]-xrange[0]);
                    drawPseudoFOM();
                    break;
                } catch (cv::Exception e) {
                    std::cerr << e.err << std::endl;
                    std::cerr << e.msg << std::endl;
                    QMessageBox qmb;
                    qmb.setText("Not enough memory available. Please reduce your desired image size.");
                    qmb.setIcon(QMessageBox::Critical);
                    qmb.exec();
                }
            } else {
                break;
            }
        }
    } else {
        QMessageBox qmb;
        qmb.setText("No input data available.");
        qmb.setIcon(QMessageBox::Information);
        qmb.exec();
    }
}

bool PseudoFOMPlugin::showDialog() {

    QDialog dialog(nullptr);
    dialog.setLocale(QLocale::C);
    QFormLayout layout(&dialog);
    layout.addRow(new QLabel("Please choose your thickness, resolution and color scheme", &dialog));

    QDoubleSpinBox box;
    box.setDecimals(2);
    box.setMinimum(0.01);
    box.setMaximum(100);
    box.setValue(1);
    layout.addRow("Thickness:", &box);

    layout.addRow(new QLabel("FOM dimensions:", &dialog));
    QSpinBox xSizeBox;
    xSizeBox.setMinimum(1);
    xSizeBox.setMaximum(10e6);
    xSizeBox.setValue(xrange[1] - xrange[0]);
    layout.addRow("x:", &xSizeBox);

    QSpinBox ySizeBox;
    ySizeBox.setMinimum(1);
    ySizeBox.setMaximum(10e6);
    ySizeBox.setValue(yrange[1] - yrange[0]);
    layout.addRow("y:", &ySizeBox);

    xsize = xSizeBox.value();
    ysize = ySizeBox.value();

    QObject::connect(&xSizeBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
                     &PseudoFOMPlugin::scaleYValue);
    QObject::connect(this, &PseudoFOMPlugin::yValueScaled, &ySizeBox, QOverload<int>::of(&QSpinBox::setValue));
    QObject::connect(&ySizeBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
                     &PseudoFOMPlugin::scaleXValue);
    QObject::connect(this, &PseudoFOMPlugin::xValueScaled, &xSizeBox, QOverload<int>::of(&QSpinBox::setValue));

    QComboBox colorBox;
    colorBox.setInsertPolicy(QComboBox::InsertAtBottom);      //Guarantees that
    colorBox.addItem("RGB-Orientation");                      //RGB has the index 0
    colorBox.addItem("HSV-Orientation");                      //And HSV has the index 1
    layout.addRow("Color scheme:", &colorBox);

    QDialogButtonBox buttonBox(QDialogButtonBox::No | QDialogButtonBox::Yes,
                               Qt::Horizontal, &dialog);
    QPushButton*  okButton = buttonBox.button(QDialogButtonBox::Yes);
    okButton->setDefault(true);
    layout.addRow(&buttonBox);
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    output_dir = QFileDialog::getExistingDirectory(nullptr, "Select desired plugin location").toUtf8().constData();
    if(output_dir.empty()) return false;
    if(dialog.exec() == QDialog::Accepted) {
        thickness = box.value();
        xsize = xSizeBox.value();
        ysize = ySizeBox.value();
        color = colorBox.currentIndex();
        return true;
    } else {
        return false;
    }
}

void PseudoFOMPlugin::initRanges(){
    /****
    *   Initialize the Xrange-Array with:
    *   Smallest X-Value at the first position
    *   Biggest X-Value  at the second position
    ****/
    FAC::FiberPool pool = FAC::FiberPool::getInstance();
    vector<FAC::Fiber>fibers = pool.getFibers();
    xrange[0] = rotate(fibers[0].getParts()[0].toVector3D()).x();
    xrange[1] = rotate(fibers[0].getParts()[0].toVector3D()).x();

    yrange[0] = rotate(fibers[0].getParts()[0].toVector3D()).y();
    yrange[1] = rotate(fibers[0].getParts()[0].toVector3D()).y();

    zrange[0] = rotate(fibers[0].getParts()[0].toVector3D()).z();
    zrange[1] = rotate(fibers[0].getParts()[0].toVector3D()).z();

    for(auto fibit = fibers.begin(); fibit < fibers.end(); ++fibit) {
        auto fib = fibit->getParts();
        for(auto fpoint = fib.begin(); fpoint < fib.end(); ++fpoint) {
            if( rotate(*fpoint).x() < xrange[0]) {
                xrange[0] = rotate(*fpoint).x();
            }
            if ( rotate(*fpoint).x() > xrange[1]) {
                xrange[1] = rotate(*fpoint).x();
            }

            if( rotate(*fpoint).y() < yrange[0]) {
                yrange[0] = rotate(*fpoint).y();
            }
            if ( rotate(fpoint->toVector3D()).y() > yrange[1]) {
                yrange[1] = rotate(*fpoint).y();
            }
            if( rotate(*fpoint).z() < zrange[0]) {
                zrange[0] = rotate(*fpoint).z();
            }
            if ( rotate(fpoint->toVector3D()).z() > zrange[1]) {
                zrange[1] = rotate(*fpoint).z();
            }
        }
    }
    xrange[0] -= 1;
    xrange[1] += 1;

    yrange[0] -= 1;
    yrange[1] += 1;

    zrange[0] -= 1;
    zrange[1] += 1;

}

QVector3D PseudoFOMPlugin::hsv2bgr(QVector3D c) {
    /****
    * Converts hsv-values to bgr-values, which were used from OpenCv
    ****/

    float H = c.x(); // [0,360]
    float S = c.y(); // [0,1]
    float V = c.z(); // [0,1]

    if( H >= 360) H -= 360;
    if( H <  0  ) H += 360;

    float C = V*S;
    float h60 = H/60;
    float X = C * (1 - abs((h60-int(h60/2)*2)-1));
    float M = V - C;

    QVector3D rgb;

    int hi= int(floor(H/60.));
    switch(hi)
    {
    case 0: case 6:
        rgb = QVector3D(C,X,0);
        break;
    case 1:
        rgb = QVector3D(X,C,0);
        break;
    case 2:
        rgb = QVector3D(0,C,X);
        break;
    case 3:
        rgb = QVector3D(0,X,C);
        break;
    case 4:
        rgb = QVector3D(X,0,C);
        break;
    case 5:
        rgb = QVector3D(C,0,X);
        break;
    }

    rgb[0] = (rgb[0]+M)*255;
    rgb[1] = (rgb[1]+M)*255;
    rgb[2] = (rgb[2]+M)*255;

    return QVector3D(rgb[2], rgb[1], rgb[0]); //From RGB to RGB
}

cv::Scalar PseudoFOMPlugin::toColor(QVector3D vec) {
    /****
    * Converts a given QVector ta an OpenCV Scalar
    ****/
    switch (this->color) {
    case 0: {     //RGB
        vec.normalize();
        vec *= 255;
        return cv::Scalar(int(abs(vec[2])), int(abs(vec[1])), int(abs(vec[0])));
    }

    case 1: {     //HSV
        vec.normalize();
        float inclinationValue = (asin(vec.z())) / M_PI;
        float directionValue = M_PI + atan2(vec.y(), vec.x());
        float h = 360*abs(directionValue)/M_PI;
        float s = 1-(2*abs(inclinationValue));
        float v = 1.0;
        QVector3D colorVector = hsv2bgr(QVector3D(h, s, v));
        return cv::Scalar(colorVector[0], colorVector[1], colorVector[2]);
    }
    }

}

void PseudoFOMPlugin::initFIZL(float stepsize) {
    /****
    * Seperates the Fibers into Z-Levels
    ****/

    int anzahl = ((zrange[1]-zrange[0])/stepsize)+1;
    vector<vector<FAC::Fiber>> fizl(anzahl, vector<FAC::Fiber>());
    FAC::FiberPool * pool = &FAC::FiberPool::getInstance();
    auto fibers = pool->getFibers();
    for(auto fit = fibers.begin(); fit < fibers.end(); fit++) {
        auto fiber = fit->getParts();
        QVector3D ffp = fiber.begin()->toVector3D();//First Fiberpoint
        int zlevel = getZLevel(ffp, stepsize);
        FAC::Fiber fibinz;
        for(auto it = fiber.begin(); it < fiber.end(); it++) {
            if( zlevel == getZLevel(it->toVector3D(), stepsize) ) {
                fibinz.addPoint(FAC::FiberPoint(*it));
            } else {
                fibinz.addPoint(FAC::FiberPoint(*it));
                fizl[zlevel].push_back(fibinz);
                fibinz = *(new FAC::Fiber());
                zlevel = getZLevel(*it, stepsize);
                fibinz.addPoint(FAC::FiberPoint(*it));
            }
        }
        fizl[zlevel].push_back(fibinz);
    }
    this->fizl = fizl;
}

void PseudoFOMPlugin::drawPseudoFOM() {
    /****
    *Draws the Pseudofoms
    ****/
    initFIZL(thickness);
    int xDisplacement = sfactor/(xrange[1]-xrange[0]);
    int yDisplacement = sfactor/(yrange[1]-yrange[0]);
    QProgressDialog *progress = new QProgressDialog("Drawing PseodoFOMs", "Cancel", 0, fizl.size(), nullptr);
    progress->setWindowModality(Qt::WindowModal);
    progress->setValue(0);
    for(int i = 0; i<fizl.size(); i++) {
        progress->setValue(i);
        if (progress->wasCanceled())
            return;

        cv::Mat mat(xsize, ysize, CV_8UC3, cv::Scalar(0, 0, 0));
        for(auto fiber : fizl[i]) {
            auto pointsInFiber = fiber.getParts();
            for(auto fibpoint = pointsInFiber.begin(); (fibpoint+1) < pointsInFiber.end(); fibpoint++) {
                QVector3D direction = (fibpoint+1)->toVector3D()-fibpoint->toVector3D();
                QVector3D from = rotate(fibpoint->toVector3D());
                int xpos = int((from.x()-xrange[0])*sfactor+spacing/2);
                int ypos = int((from.y()-yrange[0])*sfactor+spacing/2);
                cv::Point p1(xpos, ysize-ypos);
                QVector3D to = rotate((fibpoint+1)->toVector3D());
                xpos = int((to.x()-xrange[0])*sfactor+spacing/2);
                ypos = int((to.y()-yrange[0])*sfactor+spacing/2);
                cv::Point p2(xpos, ysize-ypos);
                cv::line(mat, p1, p2, toColor(direction), std::ceil(fibpoint->r()*sfactor));
            }
        }

        string name = output_dir+"/PseudoFOM"+to_string(i)+".jpg";
        cv::imwrite(name, mat);
        mat.release();
    }
    delete progress;
    QMessageBox qmb;
    qmb.setText("Wrote PseudoFOMs!");
    qmb.setIcon(QMessageBox::Information);
    qmb.exec();
}

inline int PseudoFOMPlugin::getPoolSize() {
    return FAC::FiberPool::getInstance().getFibers().size();
}

inline int PseudoFOMPlugin::getZLevel(QVector3D fibpoint, float stepsize) {
    return (rotate(fibpoint).z()-zrange[0])/stepsize;
}

QVector3D PseudoFOMPlugin::rotate(QVector3D position) const {
    /****
    * Rotates the given Vector to camera angle
    ****/

    //rotate x-axis
    QVector3D tmp = position;
    position.setY(cos(xrotation)*tmp.y()-sin(xrotation)*tmp.z());
    position.setZ(cos(xrotation)*tmp.z()+sin(xrotation)*tmp.y());

    //rotate y-axis
    tmp = position;
    position.setX(cos(yrotation)*tmp.x()+sin(yrotation)*tmp.z());
    position.setZ(cos(yrotation)*tmp.z()-sin(yrotation)*tmp.x());

    //rotate z-axis
    tmp = position;
    position.setX(cos(zrotation)*tmp.x()-sin(zrotation)*tmp.y());
    position.setY(cos(zrotation)*tmp.y()+sin(zrotation)*tmp.x());

    return position;

}

void PseudoFOMPlugin::scaleYValue(int i) {
    blockX = true;
    if(!blockY) {
        emit yValueScaled(i/proportion);
    }
    blockX = false;
}

void PseudoFOMPlugin::scaleXValue(int i) {
    blockY = true;
    if(!blockX) {
        emit xValueScaled(i*proportion);
    }
    blockY = false;
}

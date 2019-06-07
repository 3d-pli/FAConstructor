#include <QTest>
#include "fiber.h"
#include <iostream>

using namespace FAC;

class FiberTest : public QObject {
    Q_OBJECT
private slots:
    void testCreateFiberEmpty() {
        Fiber f;
        QVERIFY(f.isActive());
        QVERIFY(f.getParts().empty());
        QVERIFY(f.function().empty());
    }

    void testCreateFiberByPoints() {
        // Populate vector with values
        std::vector<FiberPoint> points(100);
        for(unsigned i = 0; i < 100; ++i) {
            points.at(i) = FiberPoint(i, i, i, i);
        }
        // Create fiber from vector
        Fiber f(points);
        // Check properties
        QVERIFY(f.isActive());
        QVERIFY(f.getParts().size() == 100);
        QVERIFY(f.function().empty());
        // Check if values were saved properly
        auto savedPoints = f.getParts();
        for(unsigned i = 0; i < savedPoints.size(); ++i) {
            QVERIFY(savedPoints.at(i).toQVector4D() == QVector4D(i, i, i, i));
        }

        QVector<FiberPoint> qpoints = QVector<FiberPoint>::fromStdVector(points);
        // Create fiber from vector
        Fiber f2(points);
        // Check properties
        QVERIFY(f2.isActive());
        QVERIFY(f2.getParts().size() == 100);
        QVERIFY(f2.function().empty());
        // Check if values were saved properly
        savedPoints = f2.getParts();
        for(unsigned i = 0; i < savedPoints.size(); ++i) {
            QVERIFY(savedPoints.at(i).toQVector4D() == QVector4D(i, i, i, i));
        }

    }

    void testCreateFiberByCopyWOFunction() {
        // Populate vector with values
        std::vector<FiberPoint> points(100);
        for(unsigned i = 0; i < 100; ++i) {
            points.at(i) = FiberPoint(i, i, i, i);
        }
        // Create fiber from vector
        Fiber f(points);

        Fiber f2(f);
        QVERIFY(f2.isActive());
        QVERIFY(f2.getParts().size() == 100);
        QVERIFY(f2.function().empty());

        f.rotate(90, 0, 0);

        auto savedPoints = f2.getParts();
        for(unsigned i = 0; i < savedPoints.size(); ++i) {
            QVERIFY(savedPoints.at(i).toQVector4D() == QVector4D(i, i, i, i));
        }
    }

    void testCreateFiberByFunction() {
        QString x = "t";
        QString y = "t";
        QString z = "t";
        float min = -1;
        float max = 1;
        QString radius = "1";
        Fiber f = Fiber::createByFunction(FiberFunction(x, y, z, min, max, radius), 0.01f);

        QVERIFY(f.isActive());
        QVERIFY(!f.function().empty());
        QVERIFY(f.getParts().size() == 201);
    }

    void testCreateFiberByCopyWFunction() {
        QString x = "t";
        QString y = "t";
        QString z = "t";
        float min = -1;
        float max = 1;
        QString radius = "1";
        Fiber f = Fiber::createByFunction(FiberFunction(x, y, z, min, max, radius), 0.01f);

        Fiber f2(f);
        QVERIFY(f2.isActive());
        QVERIFY(f2.getParts().size() == 201);
        QVERIFY(!f2.function().empty());
    }

    void testActive() {
        Fiber f;
        QVERIFY(f.isActive());
        f.setActive(false);
        QVERIFY(!f.isActive());
        f.setActive(true);
        QVERIFY(f.isActive());
        f.setActive(true);
        QVERIFY(f.isActive());
    }

    void testTranslate() {
        // Populate vector with values
        std::vector<FiberPoint> points(100);
        for(unsigned i = 0; i < 100; ++i) {
            points.at(i) = FiberPoint(i, i, i, i);
        }
        // Create fiber from vector
        Fiber f(points);

        QVector3D translation(1, 1, 1);
        f.translate(translation);
        auto savedPoints = f.getParts();
        for(unsigned i = 0; i < savedPoints.size(); ++i) {
            QVERIFY(savedPoints.at(i).toQVector4D() == QVector4D(i+1, i+1, i+1, i));
        }
        f.translate(-1, -1, -1);
        savedPoints = f.getParts();
        for(unsigned i = 0; i < savedPoints.size(); ++i) {
            QVERIFY(savedPoints.at(i).toQVector4D() == QVector4D(i, i, i, i));
        }
    }

    void testRotate() {
        // Populate vector with values
        std::vector<FiberPoint> points(100);
        for(unsigned i = 0; i < 100; ++i) {
            points.at(i) = FiberPoint(i, 0, 0, i);
        }
        // Create fiber from vector
        Fiber f(points);

        f.rotate(0, 90, 0);
        auto savedPoints = f.getParts();
        for(int i = 0; i < 100; ++i) {
            QVERIFY(savedPoints.at(i).toQVector4D() == QVector4D(0, 0, -i, i));
        }

        f.rotate(0, 90, 0);
        savedPoints = f.getParts();
        for(int i = 0; i < 100; ++i) {
            QVERIFY(savedPoints.at(i).toQVector4D() == QVector4D(-i, 0, 0, i));
        }

        f.rotate(0, 0, 90);
        savedPoints = f.getParts();
        for(int i = 0; i < 100; ++i) {
            QVERIFY(savedPoints.at(i).toQVector4D() == QVector4D(0, -i, 0, i));
        }

        f.rotate(90, 0, 0);
        savedPoints = f.getParts();
        for(int i = 0; i < 100; ++i) {
            QVERIFY(savedPoints.at(i).toQVector4D() == QVector4D(0, 0, -i, i));
        }

        f.rotate(90, 0, 0);
        savedPoints = f.getParts();
        for(int i = 0; i < 100; ++i) {
            QVERIFY(savedPoints.at(i).toQVector4D() == QVector4D(0, i, 0, i));
        }
    }
};

QTEST_MAIN(FiberTest)
#include "FiberTest.moc"

#include <QTest>
#include "fiberpoint.h"
#include <iostream>

using namespace FAC;

class FiberPointTest : public QObject {
    Q_OBJECT
private slots:
    void testCreateFiberPoint() {
        FiberPoint point;
        QVERIFY(point.x() == 0);
        QVERIFY(point.y() == 0);
        QVERIFY(point.z() == 0);
        QVERIFY(point.r() == 0);
        QVERIFY(point.w() == 0);
    }

    void testSetFiberPoint() {
        FiberPoint point(1, 2, 3, 1);
        QVERIFY(point.x() == 1);
        QVERIFY(point.y() == 2);
        QVERIFY(point.z() == 3);
        QVERIFY(point.r() == 1);
        QVERIFY(point.w() == 1);
    }

    void testSetFiberPointsByQVector3D() {
        QVector3D pos(1, 2, 3);
        FiberPoint point(pos, 1);
        QVERIFY(point.x() == 1);
        QVERIFY(point.y() == 2);
        QVERIFY(point.z() == 3);
        QVERIFY(point.r() == 1);
        QVERIFY(point.w() == 1);
    }

    void testSetFiberPointsByQVector4D() {
        QVector4D pos(1, 2, 3, 1);
        FiberPoint point(pos);
        QVERIFY(point.x() == 1);
        QVERIFY(point.y() == 2);
        QVERIFY(point.z() == 3);
        QVERIFY(point.r() == 1);
        QVERIFY(point.w() == 1);
    }

    void testFiberPointToQVector3D() {
        FiberPoint point(1, 2, 3, 1);
        QVERIFY(point.toQVector3D() == QVector3D(1, 2, 3));
    }

    void testFiberPointToQVector4D() {
        FiberPoint point(1, 2, 3, 1);
        QVERIFY(point.toQVector4D() == QVector4D(1, 2, 3, 1));
    }

    void testFiberPointString() {
        float x = 1.0f, y = 2.0f, z = 3.0f, r = 1.0f;
        FiberPoint point(x, y, z, r);
        std::string compareTo = std::to_string(x) + "\t" + std::to_string(y) + "\t" + std::to_string(z) + "\t" + std::to_string(r) + "\n";
        std::replace(compareTo.begin(), compareTo.end(), ',', '.');
        QVERIFY(std::string(point) == compareTo);
    }
};

QTEST_MAIN(FiberPointTest)
#include "FiberPointTest.moc"

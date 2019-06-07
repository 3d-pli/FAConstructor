#include <QTest>
#include "fiberfunction.h"

using namespace FAC;

class FiberFunctionTest : public QObject {
    Q_OBJECT
private slots:
    void testEmptyFunction() {
        FiberFunction f;
        QVERIFY(f.empty());
    }

    void testFilledFunction() {
        QString radius = "2", x = "t", y = "t+1", z = "t+2";
        T min = 0, max = 1,  rotX = 3,
                rotY = 4, rotZ = 5, transformX = 6,
                transformY = 7, transformZ = 8;

        FiberFunction f(x, y, z, min, max, radius, rotX, rotY, rotZ,
                        transformX, transformY, transformZ);
        QVERIFY(!f.empty());
        QVERIFY(f.functionX() == x);
        QVERIFY(f.functionY() == y);
        QVERIFY(f.functionZ() == z);
        QVERIFY(f.min() == min);
        QVERIFY(f.max() == max);
        QVERIFY(f.radius() == radius);
        QVERIFY(f.rotX() == rotX);
        QVERIFY(f.rotY() == rotY);
        QVERIFY(f.rotZ() == rotZ);
        QVERIFY(f.transformX() == transformX);
        QVERIFY(f.transformY() == transformY);
        QVERIFY(f.transformZ() == transformZ);
    }
};

QTEST_MAIN(FiberFunctionTest)
#include "FiberFunctionTest.moc"

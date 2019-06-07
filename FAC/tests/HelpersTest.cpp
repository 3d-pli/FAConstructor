#include <QTest>
#include "helpers.h"
#include <cfloat>

using namespace FAC;

class HelpersTest : public QObject {
    Q_OBJECT
private slots:
    void testPow2C() {
        for(unsigned i = 0; i < 100; ++i) {
            QVERIFY(pow2C(i) == i*i);
        }

        for(float i = -100; i < 100; i+=0.05) {
            QVERIFY(pow2C(i) == i*i);
        }

        for(double i = -100; i < 100; i+=0.05) {
            QVERIFY(pow2C(i) == i*i);
        }

        for(int i = -100; i < 100; ++i) {
            QVERIFY(pow2C(i) == i*i);
        }
    }

    void testCreateEmptyFloat3() {
        float3 f;
        QVERIFY(f.x == 0.0f);
        QVERIFY(f.y == 0.0f);
        QVERIFY(f.z == 0.0f);
    }

    void testCreateFloat3() {
        float3 f(1, 2, 3);
        QVERIFY(f.x == 1.0f);
        QVERIFY(f.y == 2.0f);
        QVERIFY(f.z == 3.0f);
    }

    void testCubicInterpolate() {
#ifndef WITHOUT_BOOST
        //TODO
        QVERIFY(true);
#else
        //TODO
        QVERIFY(true);
#endif
    }

    void testLinearRegression() {
        std::vector<QVector4D> testQVector = {QVector4D(1.0f, 0, 0, 1.0f), QVector4D(2.0f, 0, 0, 2.0f)};
        std::vector<FiberPoint> testFiberPoint = {FiberPoint(1.0f, 0, 0, 1.0f), FiberPoint(2.0f, 0, 0, 2.0f)};

        std::vector<QVector4D> expectedValues;
        for(auto i = 1.0f; i < 2.0f; i+= 0.05f) {
            expectedValues.push_back(QVector4D(i, 0.0f, 0.0f, i));
        }

        Interpolation::LinearRegression(testQVector, 0.05f);
        Interpolation::LinearRegression(testFiberPoint, 0.05f);

        QVERIFY(testQVector.size() == expectedValues.size());
        QVERIFY(testFiberPoint.size() == expectedValues.size());

        for(auto i = 0l; i < testQVector.size(); ++i) {
            QVERIFY((testQVector.at(i) - expectedValues.at(i)).lengthSquared() < FLT_EPSILON);
            QVERIFY((testFiberPoint.at(i) - expectedValues.at(i)).lengthSquared() < FLT_EPSILON);
        }
    }

    void testBundleCreation() {
        //TODO
        QVERIFY(true);
    }
};

QTEST_MAIN(HelpersTest)
#include "HelpersTest.moc"

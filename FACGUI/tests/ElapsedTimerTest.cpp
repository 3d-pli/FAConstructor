#include <QTest>
#include "elapsedtimer.h"

using namespace FACGUI;

class ElapsedTimerTest : public QObject {
    Q_OBJECT
private slots:
    void testCreation() {
        ElapsedTimer timer;
        QTest::qSleep(5);
        QVERIFY(timer.elapsed() == 0);
        timer.start();
        QTest::qSleep(5);
        QVERIFY(timer.elapsed() >= 5);
    }

    void testStop() {
        ElapsedTimer timer;
        timer.start();
        QTest::qSleep(10);
        timer.stop();
        auto save = timer.elapsed();
        QTest::qSleep(10);
        QVERIFY(timer.elapsed() == save);
    }

    void testRestart() {
        ElapsedTimer timer;
        timer.start();
        QTest::qSleep(10);
        timer.stop();
        timer.restart();
        QVERIFY(timer.elapsed() == 0);
        QVERIFY(timer.nsecsElapsed() == 0);
        timer.advanceBy(100);
        timer.restart();
        QVERIFY(timer.elapsed() == 0);
        QVERIFY(timer.nsecsElapsed() == 0);
    }

    void testAdvanceTimer() {
        ElapsedTimer timer;
        timer.advanceBy(100);
        QVERIFY(timer.elapsed() == 100);
        QVERIFY(timer.nsecsElapsed() == 100*1e6);
        timer.advanceBy(-100);
        QVERIFY(timer.elapsed() == 0);
    }

    void testRevertTimer() {
        ElapsedTimer timer;
        timer.revertBy(100);
        QVERIFY(timer.elapsed() == -100);
        QVERIFY(timer.nsecsElapsed() == -100*1e6);
        timer.revertBy(-100);
        QVERIFY(timer.elapsed() == 0);
    }
};

QTEST_MAIN(ElapsedTimerTest)
#include "ElapsedTimerTest.moc"

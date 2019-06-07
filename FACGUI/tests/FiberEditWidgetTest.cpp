#include <QTest>
#include "fibereditwidget.h"
#include <iostream>

using namespace FACGUI;
using namespace FAC;

namespace FACGUI {
    class FiberEditWidgetTest : public QObject {
        Q_OBJECT
    private slots:
        void testCreateEmptyFiberEditWidget() {
            FiberEditWidget fibereditwidget;
            fibereditwidget.openEmpty();
            fibereditwidget.show();

            QVERIFY(fibereditwidget.table->columnCount() == 4);
            QVERIFY(fibereditwidget.table->rowCount() == 1);
        }

        void testCreateFiberEditWidget() {
            std::vector<FiberPoint> points;
            for(unsigned i = 0; i < 100; ++i) {
                points.push_back(FiberPoint(i, i, i, i));
            }
            Fiber f(points);
            auto pool = &FiberPool::getInstance();
            pool->addFiber(f);

            FiberEditWidget fibereditwidget;
            fibereditwidget.openGivenFiber(f, 0);
            QVERIFY(!fibereditwidget.newFiber);
            fibereditwidget.show();

            QVERIFY(fibereditwidget.table->columnCount() == 4);
            QVERIFY(fibereditwidget.table->rowCount() == 101);

            for(unsigned i = 0; i < 100; ++i) {
                for(unsigned j = 0; j < 4; ++j) {
                    QVERIFY(fibereditwidget.table->item(i, j)->text() == QString::number(i));
                }
            }
            pool->clearPool();
        }

        void testCreateFiberWithEditWidget() {
            FiberEditWidget fibereditwidget;
            fibereditwidget.openEmpty();
            fibereditwidget.show();

            QApplication::setActiveWindow(&fibereditwidget);
            fibereditwidget.table->setFocus();
            QVERIFY(fibereditwidget.table->hasFocus());
            QTest::keyClicks(QApplication::focusWidget(), "1");
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);
            QVERIFY(fibereditwidget.changed);
            QTest::keyClicks(QApplication::focusWidget(), "1");
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);
            QTest::keyClicks(QApplication::focusWidget(), "1");
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);
            QTest::keyClicks(QApplication::focusWidget(), "1");
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);
            QTest::keyClicks(QApplication::focusWidget(), "2");
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);
            QTest::keyClicks(QApplication::focusWidget(), "2");
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);
            QTest::keyClicks(QApplication::focusWidget(), "2");
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);
            QTest::keyClicks(QApplication::focusWidget(), "1");
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);

            fibereditwidget.close();
            auto pool = &FiberPool::getInstance();
            QVERIFY(pool->getFibers().size() == 1);
            pool->clearPool();
        }

        void testLinearInterpolation() {
            std::vector<FiberPoint> points;
            for(unsigned i = 0; i < 10; ++i) {
                points.push_back(FiberPoint(i, 0, 0, i));
            }
            Fiber f(points);
            auto pool = &FiberPool::getInstance();
            pool->addFiber(f);

            FiberEditWidget fibereditwidget;
            fibereditwidget.openGivenFiber(f, 0);
            fibereditwidget.show();

            QVERIFY(fibereditwidget.table->columnCount() == 4);
            QVERIFY(fibereditwidget.table->rowCount() == 11);

            QApplication::setActiveWindow(&fibereditwidget);
            fibereditwidget.regression->setFocus();
            QTest::mouseClick(QApplication::focusWidget(), Qt::LeftButton);
            fibereditwidget.close();

            QVERIFY(pool->getFibers().size() == 1);
            QVERIFY(pool->getFibers().at(0).getParts().size() > 10);
            pool->clearPool();
        }

        void testCubicInterpolation() {
            std::vector<FiberPoint> points;
            for(unsigned i = 0; i < 10; ++i) {
                points.push_back(FiberPoint(i, 0, 0, i));
            }
            Fiber f(points);
            auto pool = &FiberPool::getInstance();
            pool->addFiber(f);

            FiberEditWidget fibereditwidget;
            fibereditwidget.openGivenFiber(f, 0);
            fibereditwidget.show();

            QVERIFY(fibereditwidget.table->columnCount() == 4);
            QVERIFY(fibereditwidget.table->rowCount() == 11);

            QApplication::setActiveWindow(&fibereditwidget);
            fibereditwidget.interpolate->setFocus();
            QTest::mouseClick(QApplication::focusWidget(), Qt::LeftButton);
            fibereditwidget.close();

            QVERIFY(pool->getFibers().size() == 1);
            QVERIFY(pool->getFibers().at(0).getParts().size() > 10);
            pool->clearPool();
        }

        void testChangeValues() {
            std::vector<FiberPoint> points;
            for(unsigned i = 0; i < 100; ++i) {
                points.push_back(FiberPoint(i, i, i, i));
            }
            Fiber f(points);
            auto pool = &FiberPool::getInstance();
            pool->addFiber(f);

            FiberEditWidget fibereditwidget;
            fibereditwidget.openGivenFiber(f, 0);
            QVERIFY(!fibereditwidget.newFiber);
            fibereditwidget.show();

            QApplication::setActiveWindow(&fibereditwidget);
            fibereditwidget.table->setFocus();
            QVERIFY(fibereditwidget.table->hasFocus());
            QTest::keyClicks(QApplication::focusWidget(), "2");
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);
            QVERIFY(fibereditwidget.changed);
            QTest::keyClicks(QApplication::focusWidget(), "2");
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);
            QTest::keyClicks(QApplication::focusWidget(), "2");
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);
            QTest::keyClicks(QApplication::focusWidget(), "2");
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);
            QTest::keyClicks(QApplication::focusWidget(), "0");
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);
            QTest::keyClicks(QApplication::focusWidget(), "0");
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);
            QTest::keyClicks(QApplication::focusWidget(), "0");
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);
            QTest::keyClicks(QApplication::focusWidget(), "0");
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);

            for(unsigned j = 0; j < 4; ++j) {
                QVERIFY(fibereditwidget.table->item(0, j)->text() == QString::number(2));
            }
            for(unsigned j = 0; j < 4; ++j) {
                QVERIFY(fibereditwidget.table->item(1, j)->text() == QString::number(0));
            }
            for(unsigned i = 2; i < 100; ++i) {
                for(unsigned j = 0; j < 4; ++j) {
                    QVERIFY(fibereditwidget.table->item(i, j)->text() == QString::number(i));
                }
            }

            fibereditwidget.close();
            QVERIFY(pool->getFibers().size() == 1);
            pool->clearPool();
        }

        void testOneItem() {
            FiberEditWidget fibereditwidget;
            fibereditwidget.openEmpty();
            fibereditwidget.show();

            QApplication::setActiveWindow(&fibereditwidget);
            fibereditwidget.table->setFocus();
            QVERIFY(fibereditwidget.table->hasFocus());
            QTest::keyClicks(QApplication::focusWidget(), "1");
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);
            QVERIFY(fibereditwidget.changed);
            QTest::keyClicks(QApplication::focusWidget(), "1");
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);
            QTest::keyClicks(QApplication::focusWidget(), "1");
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);
            QTest::keyClicks(QApplication::focusWidget(), "1");
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_Tab);

            fibereditwidget.close();
            auto pool = &FiberPool::getInstance();
            QVERIFY(pool->getFibers().size() == 0);
            pool->clearPool();
        }

        void testKeyModifiers() {
            FiberEditWidget fibereditwidget;
            fibereditwidget.openEmpty();
            fibereditwidget.show();

            QApplication::setActiveWindow(&fibereditwidget);
            fibereditwidget.table->setFocus();
            fibereditwidget.table->selectRow(0);
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_K, Qt::ControlModifier);
            QVERIFY(fibereditwidget.table->rowCount() == 2);
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_J, Qt::ControlModifier);
            QVERIFY(fibereditwidget.table->rowCount() == 3);
            QTest::keyClick(QApplication::focusWidget(), Qt::Key_Delete, Qt::ControlModifier);
            QVERIFY(fibereditwidget.table->rowCount() == 2);

            fibereditwidget.close();
        }
    };
}

QTEST_MAIN(FiberEditWidgetTest)
#include "FiberEditWidgetTest.moc"

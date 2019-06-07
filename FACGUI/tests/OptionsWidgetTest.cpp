#include <QTest>
#include "optionswidget.h"

using namespace FACGUI;
using namespace FAC;

namespace FACGUI {
    class OptionsWidgetTest : public QObject {
        Q_OBJECT
    private slots:
        void init() {
            pool = &FiberPool::getInstance();
            // Create demo fibers
            for(unsigned i = 0; i < 100; ++i) {
                std::vector<FiberPoint> points;
                for(unsigned j = 0; j < 100; ++j) {
                    points.push_back(FiberPoint(i * 100 + j, i * 100 + j, i * 100 + j, i * 100 + j));
                }
                Fiber f(points);
                pool->addFiber(f);
            }
            // Create a demo options widget
            widget = new OptionsWidget;
            QApplication::setActiveWindow(widget);
            widget->fibers->setFocus();
            pool->notifyObservers(UserEvent::DocumentEdited);
        }

        void testDeleteFiber() {
            // Mark items
            QListWidgetItem * item = widget->fibers->item(10);
            QVERIFY(item);
            QRect rect = widget->fibers->visualItemRect(item);
            QTest::mouseClick(widget->fibers->viewport(), Qt::LeftButton, 0, rect.center());
            item = widget->fibers->item(30);
            QVERIFY(item);
            rect = widget->fibers->visualItemRect(item);
            QTest::mouseClick(widget->fibers->viewport(), Qt::LeftButton, Qt::ShiftModifier, rect.center());
            // Create group of selected items
            widget->deleteFiber();
            QVERIFY(pool->getFibers().size() == 79);
        }

        void testCreateGroup() {
            // Mark items
            QListWidgetItem * item = widget->fibers->item(10);
            QVERIFY(item);
            QRect rect = widget->fibers->visualItemRect(item);
            QTest::mouseClick(widget->fibers->viewport(), Qt::LeftButton, 0, rect.center());
            item = widget->fibers->item(30);
            QVERIFY(item);
            rect = widget->fibers->visualItemRect(item);
            QTest::mouseClick(widget->fibers->viewport(), Qt::LeftButton, Qt::ShiftModifier, rect.center());
            // Create group of selected items
            widget->createGroup();
            QVERIFY(pool->getGroups().size() == 1);
            QVERIFY(pool->getGroups().at(0).size() == 21);
        }

        void testDeleteGroup() {
            // Create demo group
            QListWidgetItem * item = widget->fibers->item(10);
            QRect rect = widget->fibers->visualItemRect(item);
            QTest::mouseClick(widget->fibers->viewport(), Qt::LeftButton, Qt::NoModifier, rect.center());
            item = widget->fibers->item(30);
            rect = widget->fibers->visualItemRect(item);
            QTest::mouseClick(widget->fibers->viewport(), Qt::LeftButton, Qt::ShiftModifier, rect.center());
            widget->createGroup();
            widget->updateGroups();
            // Mark group and remove it
            item = widget->groups->item(0);
            QVERIFY(item);
            rect = widget->fibers->visualItemRect(item);
            QTest::mouseClick(widget->groups->viewport(), Qt::LeftButton, Qt::NoModifier, rect.center());
            widget->deleteGroup();
            QVERIFY(pool->getGroups().size() == 0);
        }

        void testAddToGroup() {
            // Mark items
            QListWidgetItem * item = widget->fibers->item(10);
            QRect rect = widget->fibers->visualItemRect(item);
            QTest::mouseClick(widget->fibers->viewport(), Qt::LeftButton, 0, rect.center());
            item = widget->fibers->item(30);
            rect = widget->fibers->visualItemRect(item);
            QTest::mouseClick(widget->fibers->viewport(), Qt::LeftButton, Qt::ShiftModifier, rect.center());
            // Create group of selected items
            widget->createGroup();
            widget->updateGroups();

            item = widget->groups->item(0);
            rect = widget->fibers->visualItemRect(item);
            QTest::mouseClick(widget->groups->viewport(), Qt::LeftButton, Qt::NoModifier, rect.center());

            item = widget->fibers->item(50);
            QVERIFY(item);
            rect = widget->fibers->visualItemRect(item);
            QTest::mouseClick(widget->fibers->viewport(), Qt::LeftButton, 0, rect.center());
            item = widget->fibers->item(70);
            QVERIFY(item);
            rect = widget->fibers->visualItemRect(item);
            QTest::mouseClick(widget->fibers->viewport(), Qt::LeftButton, Qt::ShiftModifier, rect.center());

            widget->addToGroup();
            QVERIFY(pool->getGroups().size() == 1);
            QVERIFY(pool->getGroups().at(0).size() == 42);
        }

        void cleanup() {
            pool->clearPool();
            delete widget;
        }

    private:
        FiberPool * pool;
        OptionsWidget * widget;
    };
}

QTEST_MAIN(OptionsWidgetTest)
#include "OptionsWidgetTest.moc"

#include <QTest>
#include "fiberpool.h"

using namespace FAC;

class FiberPoolTest : public QObject {
    Q_OBJECT
private slots:
    void testPoolInstance() {
        auto pool = FiberPool::getInstance();
        QVERIFY(pool.getFibers().size() == 0);
    }

    void testAddFiber() {
        auto pool = FiberPool::getInstance();
        Fiber f;
        pool.addFiber(f);
        QVERIFY(pool.getFibers().size() == 1);
    }

    void testDeleteFiber() {
        auto pool = FiberPool::getInstance();
        Fiber f;
        // Add two fibers
        pool.addFiber(f);
        pool.addFiber(f);
        // Create two groups. One with the element which gets deleted and one with both fibers
        std::list<unsigned> group = {0};
        pool.createGroup(group);
        group.insert(group.end(), 1);
        pool.createGroup(group);
        // Delete the first fiber. One group should be gone now and the second should have one element left.
        pool.deleteFiber(0);
        QVERIFY(pool.getFibers().size() == 1);
        QVERIFY(pool.getGroups().size() == 1);
        QVERIFY(pool.getGroups().at(0).find(0) != pool.getGroups().at(0).end());
    }

    void testDeleteFiberErrIndex() {
        auto pool = FiberPool::getInstance();
        try {
            pool.deleteFiber(1);
            QFAIL("No exception thrown");
        } catch(QString err) {
            QCOMPARE(err, QString("Index is out of bounds!"));
            QVERIFY(pool.getFibers().size() == 0);
        }
    }

    void testDeleteMultipleFibers() {
        auto pool = FiberPool::getInstance();
        for(unsigned i = 0; i < 100; ++i) {
            std::vector<FiberPoint> points;
            for(unsigned j = 0; j < 100; ++j) {
                points.push_back(FiberPoint(i * 100 + j, i * 100 + j, i * 100 + j, i * 100 + j));
            }
            Fiber f(points);
            pool.addFiber(f);
        }
        QVERIFY(pool.getFibers().size() == 100);
        QList<unsigned> toDelete = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 99};
        pool.deleteFiber(toDelete);
        QVERIFY(pool.getFibers().size() == 89);
    }

    void testCreateGroup() {
        auto pool = FiberPool::getInstance();
        for(unsigned i = 0; i < 100; ++i) {
            std::vector<FiberPoint> points;
            for(unsigned j = 0; j < 100; ++j) {
                points.push_back(FiberPoint(i * 100 + j, i * 100 + j, i * 100 + j, i * 100 + j));
            }
            Fiber f(points);
            pool.addFiber(f);
        }
        QVERIFY(pool.getFibers().size() == 100);
        QVERIFY(pool.getGroups().size() == 0);
        QList<unsigned> indices = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
        pool.createGroup(indices);
        QVERIFY(pool.getGroups().size() == 1);
        auto group = pool.getGroups().at(0);
        QVERIFY(group.size() == indices.size());
        for(auto elem = group.begin(); elem != group.end(); ++elem) {
            QVERIFY(std::find(indices.begin(), indices.end(), *elem) != indices.end());
        }

    }

    void testAddFibersToGroup() {
        auto pool = FiberPool::getInstance();
        for(unsigned i = 0; i < 100; ++i) {
            std::vector<FiberPoint> points;
            for(unsigned j = 0; j < 100; ++j) {
                points.push_back(FiberPoint(i * 100 + j, i * 100 + j, i * 100 + j, i * 100 + j));
            }
            Fiber f(points);
            pool.addFiber(f);
        }
        QVERIFY(pool.getFibers().size() == 100);

        QList<unsigned> indices = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90};
        pool.createGroup(indices);

        indices.append(QList<unsigned> {11, 21, 31, 41, 51, 61, 71, 81, 91});
        pool.addFibersToGroup(0, indices);
        QVERIFY(pool.getGroups().size() == 1);

        auto group = pool.getGroups().at(0);
        QVERIFY(group.size() == indices.size());
        for(auto elem = group.begin(); elem != group.end(); ++elem) {
            QVERIFY(std::find(indices.begin(), indices.end(), *elem) != indices.end());
        }
    }

    void testDeleteGroup() {
        auto pool = FiberPool::getInstance();
        // Create sample fibers
        for(unsigned i = 0; i < 100; ++i) {
            std::vector<FiberPoint> points;
            for(unsigned j = 0; j < 100; ++j) {
                points.push_back(FiberPoint(i * 100 + j, i * 100 + j, i * 100 + j, i * 100 + j));
            }
            Fiber f(points);
            pool.addFiber(f);
        }
        QVERIFY(pool.getFibers().size() == 100);
        // Create sample groups
        QList<unsigned> indices0 = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90};
        pool.createGroup(indices0);
        std::list<unsigned> indices1 = {1, 11, 21, 31, 41, 51, 61, 71, 81, 91};
        pool.createGroup(indices1);
        std::set<unsigned> indices2 = {2, 12, 22, 32, 42, 52, 62, 72, 82, 92};
        pool.createGroup(indices2);
        QVERIFY(pool.getGroups().size() == 3);
        // Delete second group
        pool.deleteGroup(1);
        QVERIFY(pool.getGroups().size() == 2);
        // Check if content of groups is as expected
        auto group = pool.getGroups().at(1);
        QVERIFY(group.size() == indices2.size());
        for(auto elem = group.begin(); elem != group.end(); ++elem) {
            QVERIFY(std::find(indices2.begin(), indices2.end(), *elem) != indices2.end());
        }

        group = pool.getGroups().at(0);
        QVERIFY(group.size() == indices0.size());
        for(auto elem = group.begin(); elem != group.end(); ++elem) {
            QVERIFY(std::find(indices0.begin(), indices0.end(), *elem) != indices0.end());
        }
    }

    void testDeleteFibersFromGroup() {
        auto pool = FiberPool::getInstance();
        // Create sample fibers
        for(unsigned i = 0; i < 100; ++i) {
            std::vector<FiberPoint> points;
            for(unsigned j = 0; j < 100; ++j) {
                points.push_back(FiberPoint(i * 100 + j, i * 100 + j, i * 100 + j, i * 100 + j));
            }
            Fiber f(points);
            pool.addFiber(f);
        }
        QVERIFY(pool.getFibers().size() == 100);
        // Create sample groups
        QList<unsigned> indices0 = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90};
        pool.createGroup(indices0);
        std::list<unsigned> indices1 = {1, 11, 21, 31, 41, 51, 61, 71, 81, 91};
        pool.createGroup(indices1);
        std::set<unsigned> indices2 = {2, 12, 22, 32, 42, 52, 62, 72, 82, 92};
        pool.createGroup(indices2);
        QVERIFY(pool.getGroups().size() == 3);

        QList<unsigned> toDelete = {10, 30, 11, 31};
        pool.deleteFibersFromGroup(0, toDelete);
        QVERIFY(pool.getGroups().at(0).size() == indices0.size() - 2);
    }

    void testClearPool() {
        auto pool = FiberPool::getInstance();
        // Create sample fibers
        for(unsigned i = 0; i < 100; ++i) {
            std::vector<FiberPoint> points;
            for(unsigned j = 0; j < 100; ++j) {
                points.push_back(FiberPoint(i * 100 + j, i * 100 + j, i * 100 + j, i * 100 + j));
            }
            Fiber f(points);
            pool.addFiber(f);
        }
        QVERIFY(pool.getFibers().size() == 100);
        // Create sample groups
        QList<unsigned> indices0 = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90};
        pool.createGroup(indices0);
        std::list<unsigned> indices1 = {1, 11, 21, 31, 41, 51, 61, 71, 81, 91};
        pool.createGroup(indices1);
        std::set<unsigned> indices2 = {2, 12, 22, 32, 42, 52, 62, 72, 82, 92};
        pool.createGroup(indices2);
        QVERIFY(pool.getGroups().size() == 3);

        pool.clearPool();
        QVERIFY(pool.getFibers().size() == 0);
        QVERIFY(pool.getGroups().size() == 0);
    }

    void testGetGroupsContainingFiber() {
        auto pool = FiberPool::getInstance();
        // Create sample fibers
        for(unsigned i = 0; i < 100; ++i) {
            std::vector<FiberPoint> points;
            for(unsigned j = 0; j < 100; ++j) {
                points.push_back(FiberPoint(i * 100 + j, i * 100 + j, i * 100 + j, i * 100 + j));
            }
            Fiber f(points);
            pool.addFiber(f);
        }
        QVERIFY(pool.getFibers().size() == 100);
        // Create sample groups
        QList<unsigned> indices0 = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90};
        pool.createGroup(indices0);
        std::list<unsigned> indices1 = {0, 11, 21, 31, 41, 51, 61, 71, 81, 91};
        pool.createGroup(indices1);
        std::set<unsigned> indices2 = {0, 12, 22, 32, 42, 52, 62, 72, 82, 92};
        pool.createGroup(indices2);
        QVERIFY(pool.getGroups().size() == 3);

        auto containingGroups = pool.getGroupsContainingFiber(0);
        QVERIFY(containingGroups.size() == 3);
    }
};

QTEST_MAIN(FiberPoolTest)
#include "FiberPoolTest.moc"

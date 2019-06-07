#include <QTest>
#include <string>
#include <fstream>
#include <cstdio>
#include <iostream>
#include "H5Cpp.h"
#include "fileio.h"

using namespace FACGUI;
using namespace FAC;

class FileIOTest : public QObject {
    Q_OBJECT
private slots:
    void testWriteDatFile() {
        QString fileName = "TestWriteDatFile.dat";

        // Create demo fiber pool
        FiberPool * pool = &FiberPool::getInstance();
        for(unsigned i = 0; i < 100; ++i) {
            std::vector<FiberPoint> points;
            for(unsigned j = 0; j < 100; ++j) {
                points.push_back(FiberPoint(i * 100 + j, i * 100 + j, i * 100 + j, i * 100 + j));
            }
            Fiber f(points);
            pool->addFiber(f);
        }

        // Write file
        FileIO::writeFile(fileName);
        // Check if content matches expected values
        std::ifstream file(fileName.toStdString());
        QVERIFY(file.good());

        // Check every line of the file
        float counter = 0.0f;
        std::string line;
        while(std::getline(file, line)) {
            std::string toCompare = std::string(std::to_string(counter)+"\t"+std::to_string(counter)+"\t"+std::to_string(counter)+"\t"+std::to_string(counter));
            std::replace(toCompare.begin(), toCompare.end(), ',', '.');
            if(!line.empty()) {
                QVERIFY(line == toCompare);
                ++counter;
            }

        }

        // Clearing up test
        remove(fileName.toStdString().c_str());
        pool->clearPool();
    }

    void testReadDatFile() {
        QString fileName = "TestReadDatFile.dat";

        // Create demo fiber pool
        FiberPool * pool = &FiberPool::getInstance();
        for(unsigned i = 0; i < 100; ++i) {
            std::vector<FiberPoint> points;
            for(unsigned j = 0; j < 100; ++j) {
                points.push_back(FiberPoint(i * 100 + j, i * 100 + j, i * 100 + j, i * 100 + j));
            }
            Fiber f(points);
            pool->addFiber(f);
        }

        // Write file
        FileIO::writeFile(fileName);
        pool->clearPool();

        // Read test
        FileIO::readFile(fileName);
        auto fibers = pool->getFibers();
        QVERIFY(pool->getFibers().size() == 100);
        auto counter = 0l;
        for(auto fiber : fibers) {
            QVERIFY(fiber.getParts().size() == 100);
            QVERIFY(fiber.isActive());
            QVERIFY(fiber.function().empty());
            for(auto point : fiber.getParts()) {
                QVERIFY(point.toQVector4D() == QVector4D(counter, counter, counter, counter));
                ++counter;
            }
        }

        // Clearing up test
        remove(fileName.toStdString().c_str());
        pool->clearPool();
    }

    void testWriteHDF5File() {
        QString fileName = "TestWriteH5File.h5";

        // Create demo fiber pool
        FiberPool * pool = &FiberPool::getInstance();
        for(unsigned i = 0; i < 100; ++i) {
            std::vector<FiberPoint> points;
            for(unsigned j = 0; j < 100; ++j) {
                points.push_back(FiberPoint(i * 100 + j, i * 100 + j, i * 100 + j, i * 100 + j));
            }
            Fiber f(points);
            pool->addFiber(f);
        }

        std::list<unsigned> group = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        pool->createGroup(group);

        // Write file
        FileIO::writeFile(fileName);
        // Check if content matches expected values
        std::ifstream file(fileName.toStdString());
        QVERIFY(file.good());
        // Clearing up test
        remove(fileName.toStdString().c_str());
        pool->clearPool();
    }

    void testReadHDF5File() {
        QString fileName = "TestReadH5File.h5";

        // Create demo fiber pool
        FiberPool * pool = &FiberPool::getInstance();
        for(unsigned i = 0; i < 100; ++i) {
            std::vector<FiberPoint> points;
            for(unsigned j = 0; j < 100; ++j) {
                points.push_back(FiberPoint(i * 100 + j, i * 100 + j, i * 100 + j, i * 100 + j));
            }
            Fiber f(points);
            pool->addFiber(f);
        }

        std::list<unsigned> group = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        pool->createGroup(group);

        // Write file
        FileIO::writeFile(fileName);
        pool->clearPool();

        // Read test
        FileIO::readFile(fileName);
        auto fibers = pool->getFibers();
        QVERIFY(pool->getFibers().size() == 100);
        auto counter = 0l;
        for(auto fiber : fibers) {
            QVERIFY(fiber.getParts().size() == 100);
            QVERIFY(fiber.isActive());
            QVERIFY(fiber.function().empty());
            for(auto point : fiber.getParts()) {
                QVERIFY(point.toQVector4D() == QVector4D(counter, counter, counter, counter));
                ++counter;
            }
        }
        QVERIFY(pool->getGroups().size() == 1);
        auto readGroup = pool->getGroups().at(0);
        for(auto elem = readGroup.begin(); elem != readGroup.end(); ++elem) {
            QVERIFY(std::find(group.begin(), group.end(), *elem) != group.end());
        }

        // Clearing up test
        remove(fileName.toStdString().c_str());
        pool->clearPool();
    }
};

QTEST_MAIN(FileIOTest)
#include "FileIOTest.moc"

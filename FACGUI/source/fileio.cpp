#include "fileio.h"
#include <QDebug>
#include <QElapsedTimer>
#include <cstdio>

bool FACGUI::FileIO::writeFile(QString& path) {
    if(path.endsWith(".h5")) {
        return writeHDF5File(path);
    } else return writeDatFile(path);
}

bool FACGUI::FileIO::readFile(QString& path, bool override) {
    if(path.endsWith(".h5")) {
        return readHDF5File(path, override);
    } else return readDatFile(path, override);
}

bool FACGUI::FileIO::writeDatFile(QString& path) {
    FAC::FiberPool * pool = &FAC::FiberPool::getInstance();
    auto fibers = pool->getFibers();

    QFile * destination = new QFile(path);
    if(!destination->open(QIODevice::WriteOnly)) {
        QMessageBox::critical(nullptr, "File can't be written", destination->errorString());
        return false;
    }

    QProgressDialog progress("Writing to "+path, "Cancel", 0, 100);
    progress.setWindowModality(Qt::WindowModal);
    for(unsigned i = 0; i < fibers.size(); ++i) {
        progress.setValue(qint64(float(i) / fibers.size() * 100));
        for(auto point : fibers[i].getParts()) {
            destination->write(std::string(point).c_str());
        }
        destination->write("\n");
        destination->flush();
        if (progress.wasCanceled()) {
            return false;
        }
    }
    progress.setValue(100);
    progress.close();
    destination->close();
    delete destination;
    return true;
}

bool FACGUI::FileIO::readDatFile(QString& path, bool override) {
    FAC::FiberPool * pool = &FAC::FiberPool::getInstance();

    QFile * file = new QFile(path);
    if(!file->open(QIODevice::ReadOnly)) {
        QMessageBox::critical(nullptr, "File can't be read", file->errorString());
        delete file;
        return false;
    } else {
        double maxBytes = file->size();
        double bytesRead = 0;
        std::ifstream stream(path.toStdString());
        std::string line;
        std::string elem;
        std::array<float, 4> values;
        FAC::Fiber fiber;

        if(override) {
            pool->clearPool();
        }

        QProgressDialog progress("Reading file "+path, "Cancel", 0, 100);
        progress.setWindowModality(Qt::WindowModal);
        try {
            while(std::getline(stream, line)) {
                bytesRead += line.size() + !stream.eof();
                // If the line is empty, there is a new fiber after the next line
                if(line.empty()) {
                    if(!fiber.getParts().empty()) {
                        pool->addFiber(fiber);
                        fiber = FAC::Fiber();
                    }
                    continue;
                }

                std::stringstream ss(line);

                // If all data is present
                for(int i = 0; i < 4; ++i) {
                    if(ss.peek() == EOF) throw std::exception();
                    ss >> elem;
                    values.at(i) = std::stof(elem);
                }
                fiber.addPoint(FAC::FiberPoint(values[0], values[1], values[2], values[3]));
                progress.setValue(qint64((bytesRead/maxBytes)*100));
                if (progress.wasCanceled()) {
                    pool->clearPool();
                    delete file;
                    return false;
                }
            }
            if(!fiber.getParts().empty())  pool->addFiber(fiber);
            delete file;
        } catch(...) {
            QMessageBox::critical(nullptr, "File parsing failed!", "One line does not contain all information needed for parsing to a fiber. Content is:\n" + QString::fromStdString(line));
            delete file;
            return false;
        }
        progress.setValue(100);
        progress.close();
    }
    return true;
}

bool FACGUI::FileIO::writeHDF5File(QString& path) {
    // Get fibers and groups which will be written
    auto fibers = FAC::FiberPool::getInstance().getFibers();
    auto groups = FAC::FiberPool::getInstance().getGroups();

    using namespace H5;
    //Exception::dontPrint();
    H5File file(path.toStdString(), H5F_ACC_TRUNC);

    Group bundle = file.createGroup("/fiber_bundle");

    FloatType datatype(PredType::NATIVE_FLOAT);
    IntType groupType(PredType::NATIVE_UINT);

    QProgressDialog progress("Writing fibers to "+path, "Cancel", 0, 100);
    progress.setWindowModality(Qt::WindowModal);
    for(unsigned i = 0; i < fibers.size(); ++i) {
        progress.setValue(qint64(float(i) / fibers.size() * 100));
        try {
            auto parts = fibers.at(i).getParts();
            Group fiber = bundle.createGroup(std::to_string(i));

            hsize_t posSize[2] = {parts.size(), 3};
            hsize_t radSize[2] = {parts.size(), 1};
            DataSpace positionSpace(2, posSize);
            DataSpace radiusSpace(2, radSize);

            DataSet positionDataSet = fiber.createDataSet("position", datatype, positionSpace);
            DataSet radiusDataSet = fiber.createDataSet("radii", datatype, radiusSpace);

            std::vector<float> points(3*parts.size());
            std::vector<float> radii(parts.size());

            #pragma omp parallel for
            for(unsigned j = 0; j < parts.size(); ++j) {
                points.at(j*3 + 0) = parts.at(j).x();
                points.at(j*3 + 1) = parts.at(j).y();
                points.at(j*3 + 2) = parts.at(j).z();
                radii.at(j) = parts.at(j).r();
            }

            positionDataSet.write(points.data(), datatype);
            radiusDataSet.write(radii.data(), datatype);

            radiusDataSet.close();
            positionDataSet.close();
            positionSpace.close();
            radiusSpace.close();
        } catch (...) {
            file.flush(H5F_SCOPE_GLOBAL);
            return false;
        }
        if (progress.wasCanceled()) {
            file.flush(H5F_SCOPE_GLOBAL);
            return false;
        }

    }
    file.flush(H5F_SCOPE_GLOBAL);

    file.createGroup("/groups/");
    for(unsigned i = 0; i < groups.size(); ++i) {
        std::set<unsigned> currentGroup = groups.at(i);
        std::vector<unsigned> toWrite;
        std::copy(currentGroup.begin(), currentGroup.end(), std::back_inserter(toWrite));

        Group group = file.createGroup("/groups/"+std::to_string(i));
        for(unsigned fiber : toWrite) {
            file.link(H5G_LINK_SOFT, "/fiber_bundle/"+std::to_string(fiber), "/groups/"+std::to_string(i)+"/"+std::to_string(fiber));
        }
    }

    file.close();
    return true;
}

bool FACGUI::FileIO::readHDF5File(QString& path, bool override) {
    FAC::FiberPool * pool = &FAC::FiberPool::getInstance();
    if(override) {
        pool->clearPool();
    }

    using namespace H5;
    Exception::dontPrint();

    FloatType datatype(PredType::NATIVE_FLOAT);
    IntType groupType(PredType::NATIVE_UINT);

    H5File file(path.toStdString(), H5F_ACC_RDONLY);
    Group fiberBundleGroup = file.openGroup("fiber_bundle");

    // Import all fibers
    unsigned counter = 0;
    QProgressDialog progress;
    progress.setWindowModality(Qt::WindowModal);
    progress.setLabelText("Reading file "+path);
    progress.setCancelButton(0);
    progress.setRange(0, 0);
    progress.setMinimumDuration(0);
    progress.show();
    while (true) {
        try {
            Group bundleGroup = fiberBundleGroup.openGroup(std::to_string(counter));
            DataSet bundlePosDataSet = bundleGroup.openDataSet("position");
            DataSet bundleRadDataSet = bundleGroup.openDataSet("radii");
            DataSpace bundlePosDataSpace = bundlePosDataSet.getSpace();
            DataSpace bundleRadDataSpace = bundleRadDataSet.getSpace();

            hsize_t dimsPos[2];
            bundlePosDataSpace.getSimpleExtentDims(dimsPos);
            hsize_t dimsRad[2];
            bundleRadDataSpace.getSimpleExtentDims(dimsRad);

            std::vector<float> xyzData(dimsPos[0] * dimsPos[1]);
            std::vector<float> rData(dimsRad[0] * dimsRad[1]);
            bundlePosDataSet.read(xyzData.data(), datatype);
            bundleRadDataSet.read(rData.data(), datatype);

            FAC::Fiber f(xyzData.size()/3);
            #pragma omp parallel for
            for(unsigned i = 0; i < xyzData.size(); i += 3) {
                f.setFiberPoint(i/3, FAC::FiberPoint(xyzData.at(i), xyzData.at(i+1), xyzData.at(i+2), rData.at(i/3)));
            }
            pool->addFiber(f);
            ++counter;

            bundleRadDataSpace.close();
            bundleRadDataSet.close();
            bundlePosDataSpace.close();
            bundlePosDataSet.close();
            bundleGroup.close();

            if (progress.wasCanceled()) {
                pool->clearPool();
                return false;
            }

            QApplication::processEvents();
        } catch (GroupIException not_found_err) {
            break;
        }
    }

    // Import all groups
    counter = 0;
    fiberBundleGroup.close();
    fiberBundleGroup = file.openGroup("/groups/");
    while (true) {
        try {
            Group currentGroup = fiberBundleGroup.openGroup(std::to_string(counter));
            QVector<unsigned> groupData;
            H5Lvisit(currentGroup.getId(), H5_INDEX_NAME, H5_ITER_NATIVE, FileIO::h5_recursive_name, &groupData);
            ++counter;
            QList<unsigned> groupList = QList<unsigned>::fromVector(groupData);
            pool->createGroup(groupList);
        } catch (GroupIException not_found_err) {
            break;
        }
    }
    progress.cancel();
    return true;
}

herr_t FACGUI::FileIO::h5_recursive_name( hid_t g_id, const char *name, const H5L_info_t *info, void *op_data) {
    auto group_names = reinterpret_cast<QVector<unsigned>*>(op_data);
    std::string sname(name);
    if(sname.find("/") == std::string::npos)
        group_names->push_back(atoi(name));
    return 0;
}



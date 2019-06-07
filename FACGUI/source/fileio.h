#ifndef FILEIO_H
#define FILEIO_H

#include <QString>
#include <QFile>
#include <iostream>
#include <QMessageBox>
#include <QTextStream>
#include <QProgressDialog>
#include <set>
#include <vector>
#include "H5Cpp.h"
#include "fiberpool.h"

namespace FACGUI {
    class FileIO
    {
    public:
        static bool writeFile(QString& path);
        static bool readFile(QString& path, bool override = true);

    private:
        static herr_t h5_recursive_name( hid_t g_id, const char *name, const H5L_info_t *info, void *op_data);
        static bool writeDatFile(QString& path);
        static bool readDatFile(QString& path, bool override);
        static bool writeHDF5File(QString& path);
        static bool readHDF5File(QString& path, bool override);
    };
}

#endif // FILEIO_H

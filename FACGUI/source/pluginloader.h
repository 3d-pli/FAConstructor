#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QAction>
#include <QDebug>
#include <QHash>
#include <QObject>
#include <QtPlugin>
#include <QMessageBox>
#include <QPluginLoader>
#include <QDirIterator>
#include <QCoreApplication>
#include <vector>
#include "fiberpool.h"
#include "plugindata.h"
#include "plugininterface.h"
#include "helpers.h"
#include "settings.h"

namespace FACGUI {
    class PluginLoader : public QObject
    {
        Q_OBJECT
    public:
        PluginLoader();
        std::vector<QString> plugins();
        static void setProgramPath(QString programpath);

    public slots:
        void runPlugin();

    private:
        QPluginLoader loader;
        QHash<QString, QString> plugin_names;
        static QString ProgramPath;
    };
}

#endif // PLUGINLOADER_H

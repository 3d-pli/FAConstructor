#include "pluginloader.h"
#include <QDebug>

QString FACGUI::PluginLoader::ProgramPath = QString("");

FACGUI::PluginLoader::PluginLoader()
{
    auto folders = FACGUI::Settings::paths();
    #ifdef __APPLE__
        auto extension = "/../PlugIns";
        QJsonValue defaultPath(ProgramPath+extension);
        folders.append(defaultPath);
        QCoreApplication::addLibraryPath(defaultPath.toString());
    #endif
    for(auto folder : folders) {
        QDirIterator it(folder.toString());
        QString str;
        while(it.hasNext()) {
            str = it.next();
            if(str.endsWith(".a") || str.endsWith(".so") || str.endsWith(".dylib") || str.endsWith(".dll")) {
                if(it.fileInfo().exists() && it.fileInfo().isFile()) {
                    loader.setFileName(str);
                    loader.load();
                    if(loader.isLoaded()) {
                        QString shortened(str);
                        shortened.replace(folder.toString()+"/", "");
                        shortened.replace(".so", "");
                        shortened.replace(".dylib", "");
                        shortened.replace(".a", "");
                        shortened.replace(".dll", "");
                        if(shortened.startsWith("lib")) {
                            shortened.replace(shortened.indexOf("lib"), 3, "");
                        }
                        plugin_names.insert(shortened, str);
                    } else {
                        qDebug() << "Error while loading" << loader.fileName() << ":" << loader.errorString();
                    }
                }
            }
        }
    }
}

std::vector<QString> FACGUI::PluginLoader::plugins(){
    std::vector<QString> returnValue;
    for(auto elem = plugin_names.begin(); elem != plugin_names.end(); ++elem) {
        returnValue.push_back(elem.key());
    }
    return returnValue;
}

void FACGUI::PluginLoader::runPlugin() {
   QAction* action = qobject_cast<QAction*>(sender());
   if(action) {
        loader.setFileName(plugin_names.value(action->text()));
        loader.load();
        if(loader.isLoaded()) {
            QObject *plugin = loader.instance();
            FACPluginInterface* interface = qobject_cast<FACPluginInterface*>(plugin);
            if(interface) {
                FAC::FiberPool::getInstance().notifyObservers(FAC::UserEvent::PluginCalled);
                interface->run();
            } else {
                QMessageBox::information(nullptr, "Error", "Library does not implement the interface");
            }
        } else {
            qDebug() << loader.errorString();
            QMessageBox::information(nullptr, "Error", "Library could not be loaded at runtime.");
        }
   }
}

void FACGUI::PluginLoader::setProgramPath(QString programpath) {
    PluginLoader::ProgramPath = programpath;
}

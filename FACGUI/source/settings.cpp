#include "settings.h"

float FACGUI::Settings::function_step_size = 0.01f;
float FACGUI::Settings::linear_interpolation_step_size = 0.05f;
unsigned long long FACGUI::Settings::cubic_spline_interpolation_steps = 50;
QString FACGUI::Settings::qt_theme = "Fusion";
QJsonArray FACGUI::Settings::plugin_paths;
FACGUI::RenderingOption FACGUI::Settings::renderingOption = FACGUI::RenderingOption::Filled;

void FACGUI::Settings::loadSettings() {
    auto configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QFile file(configPath+"/FAConstructor/config.dat");
    if(file.open(QFile::ReadOnly)) {
        QString text = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());
        QJsonObject creation = doc.object().value("Creation").toObject();
        function_step_size = creation.value("function_step_size").toDouble();
        linear_interpolation_step_size = creation.value("linear_interpolation_step_size").toDouble();
        cubic_spline_interpolation_steps = creation.value("cubic_spline_interpolation_steps").toInt();
        QJsonObject style = doc.object().value("Theme").toObject();
        qt_theme = style.value("selectedStyle").toString();
        #ifndef __APPLE__
                plugin_paths = doc.object().value("Plugins").toArray();
                for(auto path : plugin_paths) {
                    QCoreApplication::addLibraryPath(path.toString());
                }
        #endif
        renderingOption = static_cast<FACGUI::RenderingOption>(doc.object().value("filled_rendering").toInt());
    } else {
        std::cerr << "Settings file not found or not readable. Using standard values!" << std::endl;
        std::cerr << file.errorString().toStdString() << std::endl;
        writeSettings();
    }
}

void FACGUI::Settings::writeSettings() {
    auto configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QFile file(configPath+"/FAConstructor/config.dat");
    QDir dir;
    dir.mkpath(configPath+"/FAConstructor/");
    if(file.open(QFile::WriteOnly)) {
        QJsonObject all;
        QJsonObject creation {
            {"function_step_size", function_step_size},
            {"linear_interpolation_step_size", linear_interpolation_step_size},
            {"cubic_spline_interpolation_steps", QJsonValue(qint64(cubic_spline_interpolation_steps))}
        };
        all.insert("Creation", creation);
        QJsonObject style {
            {"selectedStyle", qt_theme}
        };
        all.insert("Theme", style);
        #ifndef __APPLE__
            all.insert("Plugins", plugin_paths);
        #endif
        all.insert("filled_rendering", renderingOption);
        QJsonDocument doc(all);
        file.write(doc.toJson());
    } else {
        std::cerr << "Settings file not written." << std::endl;
        std::cerr << file.errorString().toStdString() << std::endl;
    }
}

void FACGUI::Settings::addPath(QString path) {
    QJsonValue val(path);
    plugin_paths.append(val);
}

void FACGUI::Settings::deletePath(QString path) {
    for(int i = plugin_paths.size()-1; i >= 0; --i) {
        if(plugin_paths.at(i).toString() == path) {
            plugin_paths.erase(plugin_paths.begin() + i);
        }
    }
}

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <iostream>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QCoreApplication>

namespace FACGUI {
    enum RenderingOption {Filled = 0, Hollow = 1, Lines = 2};

    class Settings {
    public:
        inline static float functionStepSize() {
            return function_step_size;
        }

        inline static float linInterpolationStepSize() {
            return linear_interpolation_step_size;
        }

        inline static unsigned long long cubSplineSteps() {
            return cubic_spline_interpolation_steps;
        }

        inline static QString theme() {
            return qt_theme;
        }

        inline static QJsonArray paths() {
            return plugin_paths;
        }

        inline static RenderingOption rendering() {
            return renderingOption;
        }

        static void addPath(QString path);
        static void deletePath(QString path);

        static void loadSettings();
        static void writeSettings();

    private:
        Settings();
        static float function_step_size;
        static float linear_interpolation_step_size;
        static unsigned long long cubic_spline_interpolation_steps;
        static QString qt_theme;
        static QJsonArray plugin_paths;
        static RenderingOption renderingOption;

        friend class SettingsWidget;
    };
}

#endif

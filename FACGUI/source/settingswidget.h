#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QFormLayout>
#include <QObject>
#include <QWidget>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QSpinBox>
#include <QStyleFactory>
#include <QPushButton>
#include <QListWidget>
#include <QFileDialog>
#include "settings.h"

namespace FACGUI {
    class SettingsWidget : public QWidget
    {
        Q_OBJECT
    public:
        explicit SettingsWidget(QWidget *parent = nullptr);

    signals:

    public slots:
        void writeSettings();
        void addPath();
        void deletePath();
        void updatePath();

    private:
        QSpinBox *cubic_spline_interpolation_steps;
        QDoubleSpinBox *function_step_size, *linear_interpolation_step_size;
        QPushButton *accept, *cancel, *addPathButton, *deletePathButton;
        QComboBox *qt_theme, *render_style;
        QListWidget *plugin_paths;

        QFormLayout *layout;
    };
}

#endif // SETTINGSWIDGET_H

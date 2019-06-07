#include "settingswidget.h"

FACGUI::SettingsWidget::SettingsWidget(QWidget *parent) : QWidget(parent)
{
    this->setLocale(QLocale::C);
    layout = new QFormLayout(this);

    cubic_spline_interpolation_steps = new QSpinBox(this);
    cubic_spline_interpolation_steps->setRange(5, 100000);
    cubic_spline_interpolation_steps->setValue(Settings::cubSplineSteps());

    function_step_size = new QDoubleSpinBox(this);
    function_step_size->setRange(0.0001, 10e9);
    function_step_size->setDecimals(5);
    function_step_size->setValue(Settings::functionStepSize());

    linear_interpolation_step_size = new QDoubleSpinBox(this);
    linear_interpolation_step_size->setRange(0.0001, 10e9);
    linear_interpolation_step_size->setDecimals(5);
    linear_interpolation_step_size->setValue(Settings::linInterpolationStepSize());

    qt_theme = new QComboBox(this);
    for(auto theme : QStyleFactory::keys()) {
        qt_theme->addItem(theme);
    }
    qt_theme->setCurrentIndex(qt_theme->findText(Settings::theme()));

    render_style = new QComboBox(this);
    render_style->addItem(tr("Filled cylinders"));
    render_style->addItem(tr("Hollow cylinders"));
    render_style->addItem(tr("Line cylinders"));
    render_style->setCurrentIndex(Settings::rendering());

    #ifndef __APPLE__
        plugin_paths = new QListWidget(this);
        updatePath();
        addPathButton = new QPushButton("Add another path", this);
        connect(addPathButton, &QPushButton::pressed, this, &SettingsWidget::addPath);
        deletePathButton = new QPushButton("Delete selected paths", this);
        connect(deletePathButton, &QPushButton::pressed, this, &SettingsWidget::deletePath);
    #endif

    accept = new QPushButton("Accept", this);
    connect(accept, &QPushButton::pressed, this, &SettingsWidget::writeSettings);
    cancel = new QPushButton("Cancel", this);
    connect(cancel, &QPushButton::pressed, this, &SettingsWidget::close);

    layout->addRow(tr("Cubic Spline interpolation steps: "), cubic_spline_interpolation_steps);
    layout->addRow(tr("Function step size: "), function_step_size);
    layout->addRow(tr("Linear Interpolation step size: "), linear_interpolation_step_size);
    layout->addRow(tr("Theme: "), qt_theme);
    layout->addRow(tr("Rendering option: "), render_style);
    #ifndef __APPLE__
        layout->addRow(plugin_paths);
        layout->addRow(addPathButton);
        layout->addRow(deletePathButton);
    #endif
    layout->addRow(accept);
    layout->addRow(cancel);
}

void FACGUI::SettingsWidget::writeSettings() {
    Settings::cubic_spline_interpolation_steps = this->cubic_spline_interpolation_steps->text().toULongLong();
    Settings::function_step_size = this->function_step_size->text().toFloat();
    Settings::linear_interpolation_step_size = this->linear_interpolation_step_size->text().toFloat();
    Settings::qt_theme = qt_theme->currentText();
    Settings::renderingOption = static_cast<FACGUI::RenderingOption>(render_style->currentIndex());
    Settings::writeSettings();
    this->close();
}

void FACGUI::SettingsWidget::addPath() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select desired plugin location");
    Settings::addPath(dir);
    updatePath();
}

void FACGUI::SettingsWidget::deletePath() {
    QList<QListWidgetItem*> selected = this->plugin_paths->selectedItems();
    for(auto selectedItem : selected) {
        Settings::deletePath(selectedItem->text());
    }
    updatePath();
}

void FACGUI::SettingsWidget::updatePath() {
    plugin_paths->clear();
    for(auto path : Settings::paths()) {
        if(path.toString().isNull() || path.toString().isEmpty()) continue;
        plugin_paths->addItem(path.toString());
    }
}

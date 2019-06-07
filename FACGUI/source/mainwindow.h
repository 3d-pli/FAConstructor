#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QWidget>
#include <QToolBox>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QApplication>
#include <QCloseEvent>
#include <QTextStream>
#include <QStringList>
#include <QFile>
#include <QVector>
#include <QImage>
#include <QDebug>
#include <QGridLayout>
#include <iostream>
#include <locale>
#include "fiberpool.h"
#include "fileio.h"
#include "helpers.h"
#include "optionswidget.h"
#include "pluginloader.h"
#include "renderwidget.h"
#include "settings.h"
#include "settingswidget.h"
#include "version.h"
#include "widgetdef.h"

namespace FACGUI {
    class MainWindow : public QMainWindow
    {
        Q_OBJECT
        MAKE_TESTABLE(MainWindow)
    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

        void setProgramName(const QString& name);
        void resize(const QSize &size);
        void resize(int w, int h);
        bool event(QEvent *event);

        void openFileOnStartup(char* fileName);

    private slots:
        void newFile();
        void openFile();
        void importAdditional();
        void closeFile();
        bool save();
        bool saveAs();
        bool exportAs();
        void about();
        void documentWasModified();
        void openSettings();

    private:
        void createAndSetMenuBar();
        void createAndSetWidgets();
        void closeEvent(QCloseEvent *event);

        void setCurrentFile(const QString& file);
        bool maybeSave();
        bool saveFile(const QString& path);

        QString programName;
        QString file;
        PluginLoader loader;
        OptionsWidget *optionsWidget = nullptr;
        RenderWidget *renderWidget = nullptr;
        SettingsWidget *settingsWidget = nullptr;
        FAC::FiberPool * pool;
        bool modified = false;
    };
}

#endif // MAINWINDOW_H

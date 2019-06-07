#include "mainwindow.h"
#include "settings.h"
#include <QApplication>
#include <QSurfaceFormat>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Load settings and apply theming for the program.
    FACGUI::Settings::loadSettings();
    FACGUI::PluginLoader::setProgramPath((QFileInfo(argv[0]).absolutePath()));
    a.setStyle(QStyleFactory::create(FACGUI::Settings::theme()));

    /* Set different attributes for the OpenGL renderer such as the
     * OpenGL Version or the profile. Version 3.3 is used as it is
     * compatible with most current machines. */
    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
#ifdef __APPLE__
    format.setSwapInterval(2);
#endif
    QSurfaceFormat::setDefaultFormat(format);

    /* Create main component of the program. Set the standard program size.
     * Then the user inserts an argument when starting the program try to load
     * the corresponding file. */
    FACGUI::MainWindow w;
    w.setProgramName("FAConstructor");
    w.resize(QSize(1024, 768));
    if(argc > 1) {
        w.openFileOnStartup(argv[1]);
    }
    w.show();

    return a.exec();
} // main

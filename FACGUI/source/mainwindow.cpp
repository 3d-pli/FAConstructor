#include "mainwindow.h"
/*
 * PUBLIC:
 */
FACGUI::MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    /* Create a central widget to prevent misplacement of some elements
     * which could interfere with the menu bar based on the window manager
     * and operating system of the user */
    this->setCentralWidget(new QWidget);
    this->setLocale(QLocale::C);
    setlocale(LC_ALL, NULL);
    std::locale::global(std::locale::classic());
    this->setMinimumSize(1024, 768);

    this->pool = &FAC::FiberPool::getInstance();
    this->pool->addObserver(this);

    /* Initialize Layout, Widgets and OpenGL Context for later usage */
    Settings::loadSettings();

    createAndSetWidgets();
    createAndSetMenuBar();

} // MainWindow::MainWindow

FACGUI::MainWindow::~MainWindow() {
    pool->removeObserver(this);
} // MainWindow::~MainWindow

void FACGUI::MainWindow::resize(const QSize& size) {
    QMainWindow::resize(size);
} // MainWindow::resize

void FACGUI::MainWindow::resize(int w, int h) {
    QMainWindow::resize(w, h);
}

void FACGUI::MainWindow::setProgramName(const QString &name) {
    this->programName = name.split("/").last();
    this->setWindowTitle(this->programName + QString(" ") + this->file);
} // MainWindow::setProgramName


/*
 * PRIVATE SLOTS:
 */
void FACGUI::MainWindow::newFile() {
    if(this->modified) {
        this->maybeSave();
    }
    this->setCurrentFile("");
    this->pool->clearPool();
    this->pool->notifyObservers(FAC::UserEvent::FiberHidden);
} // MainWindow::newFile

void FACGUI::MainWindow::openFile() {
    // Delete previous data if present. Check if it needs to be saved beforehand.
    if(maybeSave()) {
        // Get path of file which will be read
        QString suffix;
        QString fileName = QFileDialog::getOpenFileName(nullptr, tr("Open Fiber Model Data"), QDir::homePath(), tr("Fiber Model Data (*.dat) ;; HDF5 File (*.h5) ;; All Files (*.*)"), &suffix, QFileDialog::DontUseNativeDialog);
        if(!fileName.isNull()) {
            if(FileIO::readFile(fileName)) {
                this->setCurrentFile(fileName);
                this->pool->notifyObservers(FAC::UserEvent::FiberHidden);
                this->renderWidget->resetView();
            }
        }
        this->modified = false;
    }
} // MainWindow::openFile

void FACGUI::MainWindow::openFileOnStartup(char* fileName) {
    QString file(fileName);
    if(FileIO::readFile(file)) {
        this->setCurrentFile(fileName);
        this->pool->notifyObservers(FAC::UserEvent::FiberHidden);
        this->renderWidget->resetView();
    } else {
        this->newFile();
    }
}

void FACGUI::MainWindow::importAdditional() {
    QString suffix;
    QString fileName = QFileDialog::getOpenFileName(nullptr, tr("Open Fiber Model Data"), QDir::homePath(), tr("Fiber Model Data (*.dat) ;; HDF5 File (*.h5) ;; All Files (*.*)"), &suffix, QFileDialog::DontUseNativeDialog);
    if(!fileName.isNull()) {
        if(FileIO::readFile(fileName, false)) {
            this->setCurrentFile(fileName);
            this->pool->notifyObservers(FAC::UserEvent::DocumentEdited);
        }
    }
}

void FACGUI::MainWindow::closeFile() {
    if(!this->modified) {
        this->newFile();
    } else if(maybeSave()) {
        setCurrentFile(QString(""));
        this->modified = false;
        this->newFile();
    }
} // MainWindow::closeFile

bool FACGUI::MainWindow::maybeSave() {
    if(this->modified) {
        QMessageBox::StandardButton reallyWantToSave = QMessageBox::question(this, "Exit without saving?", "Do you really want to close this file without saving?", QMessageBox::Yes | QMessageBox::No);
        if(reallyWantToSave == QMessageBox::Yes) {
            return true;
        } else {
            return this->saveAs();
        }
    }
    return true;
} // MainWindow::maybeSave

bool FACGUI::MainWindow::save() {
    if(this->modified) {
        if(!this->file.isEmpty()) {       
            this->modified = false;
            setCurrentFile(this->file);
            return FileIO::writeFile(this->file);
        } else {
            return this->saveAs();
        }
    }
    return false;
} // MainWindow::save

bool FACGUI::MainWindow::saveAs() {
    QString suffix;
    QString fileName = QFileDialog::getSaveFileName(this, ("Save Fiber Model Data"), QDir::homePath(), ("HDF5 File (*.h5) ;; Fiber Model Data (*.dat)"), &suffix, QFileDialog::DontUseNativeDialog);
    if(!fileName.isNull()) {
        if(!(fileName.contains(QString(".dat")) || fileName.contains(QString(".h5")))) {
            fileName = fileName + (suffix.contains(QString(".dat")) ? ".dat" : ".h5");
        }
		return saveFile(fileName);
    }
    return false;
} // MainWindow::saveAs

bool FACGUI::MainWindow::exportAs() {
    QString suffix;
    QString fileName = QFileDialog::getSaveFileName(this, ("Export Fiber Model Data as picture"), QDir::homePath(),
                                                    ("PNG (*.png) ;; JPEG (*.jpeg)"), &suffix, QFileDialog::DontUseNativeDialog);
    if(!fileName.isNull()) {
        if(!(fileName.contains(QString(".png")) || fileName.contains(QString(".jpeg")))) {
            fileName = fileName + (suffix.contains(QString(".png")) ? ".png" : ".jpeg");
        }
        auto image = renderWidget->getImages();
        image.save(fileName);
        return true;
    }
    return false;
} // MainWindow::exportAs

void FACGUI::MainWindow::about() {
    QMessageBox::about(this, QString("About") + this->programName, "Version " FAConstructor_VERSION_MAJOR "." FAConstructor_VERSION_MINOR "." FAConstructor_VERSION_PATCH "\n"
                                                                   "Author: Jan André Reuter\n"
                                                                   "Compiled with: " CMAKE_CXX_COMPILER " Version " CMAKE_CXX_COMPILER_VERSION "\n"
                                                                   "OpenMP enabled: " FAConstructor_WITH_OMP "\n"
                                                                   "Build Type: " FAConstructor_RELEASE_TYPE);
} // MainWindow::about

void FACGUI::MainWindow::openSettings() {
    if(settingsWidget) {
        settingsWidget->close();
        delete settingsWidget;
    }
    settingsWidget = new SettingsWidget();
    settingsWidget->show();
} // MainWindow::openSettings

void FACGUI::MainWindow::documentWasModified() {
    if(!this->modified) {
        this->modified = true;
        this->setWindowTitle(this->windowTitle() + QString(" *"));
    }
} // MainWindow::documentWasModified

void FACGUI::MainWindow::setCurrentFile(const QString& file) {
    this->file = file;
    this->setWindowTitle(this->programName + QString(" ") + this->file);
} // MainWindow::setCurrentFile

bool FACGUI::MainWindow::saveFile(const QString& path) {
	QString previousPath = this->file;
    this->file = path;
	this->modified = true;
	if (save()) {
		return true;
	}
	else {
		setCurrentFile(previousPath);
		return false;
	}
} // MainWindow::saveFile

/*
 * PRIVATE:
 */

/* Create and set menubar with corresponding shortcuts and
 * methods which get called on trifggering the menu entry */
void FACGUI::MainWindow::createAndSetMenuBar() {
    QAction* act;

    /* File Menu */
    QMenu *fileMenu = menuBar()->addMenu("File");
    act = fileMenu->addAction("New File");
    act->setShortcut(QKeySequence::New);
    connect(act, &QAction::triggered, this, &MainWindow::newFile);

    act = fileMenu->addAction("Open File");
    act->setShortcut(QKeySequence::Open);
    connect(act, &QAction::triggered, this, &MainWindow::openFile);

    act = fileMenu->addAction("Import additional");
    connect(act, &QAction::triggered, this, &MainWindow::importAdditional);

    fileMenu->addSeparator();

    act = fileMenu->addAction("Save");
    act->setShortcut(QKeySequence::Save);
    connect(act, &QAction::triggered, this, &MainWindow::save);

    act = fileMenu->addAction("Save As ...");
    act->setShortcut(QKeySequence::SaveAs);
    connect(act, &QAction::triggered, this, &MainWindow::saveAs);

    act = fileMenu->addAction("Export As ...");
    act->setShortcut(QKeySequence(Qt::CTRL | Qt::ShiftModifier | Qt::Key_E));
    connect(act, &QAction::triggered, this, &MainWindow::exportAs);

    fileMenu->addSeparator();

    act = fileMenu->addAction("Close File");
    act->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_W));
    connect(act, &QAction::triggered, this, &MainWindow::closeFile);

    /* Naming actions need to be different in certain situations when dealing with macOS.
     * When using Exit in an Apple environment, the desired destination is changed to the
     * default Apple one. This should be intended as the shortcut for the close action
     * is associated with it. That's why you don't need the setShortcut method. */
#ifndef __APPLE__
    act = fileMenu->addAction("Close Program");
    act->setShortcut(QKeySequence::Quit);
    connect(act, &QAction::triggered, this, &MainWindow::close);
#else
    act = fileMenu->addAction("Exit");
    connect(act, &QAction::triggered, this, &MainWindow::close);
#endif // __APPLE__

    /* Edit Menu */
    QMenu *editMenu = menuBar()->addMenu("Edit");
    QMenu *editGroup = editMenu->addMenu("Group");
    act = editGroup->addAction("Create");
    connect(act, &QAction::triggered, this->optionsWidget, &OptionsWidget::createGroup);
    act = editGroup->addAction("Add fibers");
    connect(act, &QAction::triggered, this->optionsWidget, &OptionsWidget::addToGroup);
    act = editGroup->addAction("Delete");
    connect(act, &QAction::triggered, this->optionsWidget, &OptionsWidget::deleteGroup);
    editGroup->addSeparator();
    act = editGroup->addAction("Rotate");
    connect(act, &QAction::triggered, this->optionsWidget, &OptionsWidget::rotateGroup);
    act = editGroup->addAction("Translate");
    connect(act, &QAction::triggered, this->optionsWidget, &OptionsWidget::transformGroup);
    act = editGroup->addAction("Duplicate");
    connect(act, &QAction::triggered, this->optionsWidget, &OptionsWidget::duplicateGroup);
    editGroup->addSeparator();
    act = editGroup->addAction("Join together");
    connect(act, &QAction::triggered, this->optionsWidget, &OptionsWidget::joinGroups);
    QMenu *fiberGroup = editMenu->addMenu("Fiber");
    act = fiberGroup->addAction("Import from file");
    connect(act, &QAction::triggered, this->optionsWidget, &OptionsWidget::generateFiberFromFile);
    act = fiberGroup->addAction("Import from input");
    connect(act, &QAction::triggered, this->optionsWidget, &OptionsWidget::openFiberEditNewFiber);
    fiberGroup->addSeparator();
    act = fiberGroup->addAction("Rotate selected");
    connect(act, &QAction::triggered, this->optionsWidget, &OptionsWidget::rotateFiber);
    act = fiberGroup->addAction("Translate selected");
    connect(act, &QAction::triggered, this->optionsWidget, &OptionsWidget::transformFiber);
    act = fiberGroup->addAction("Duplicate selected");
    connect(act, &QAction::triggered, this->optionsWidget, &OptionsWidget::duplicateFiber);
    act = fiberGroup->addAction("Thin out selected");
    connect(act, &QAction::triggered, this->optionsWidget, &OptionsWidget::thinFiber);
    act = fiberGroup->addAction("Delete selected");
    connect(act, &QAction::triggered, this->optionsWidget, &OptionsWidget::deleteFiber);
    fiberGroup->addSeparator();
    act = fiberGroup->addAction("Create bundle");
    connect(act, &QAction::triggered, this->optionsWidget, &OptionsWidget::createBundleFromFiber);
    editMenu->addSeparator();
    act = editMenu->addAction("Settings");
    connect(act, &QAction::triggered, this, &MainWindow::openSettings);

    /* View Menu */
    QMenu *viewMenu = menuBar()->addMenu("View");
    act = viewMenu->addAction("Restore view");
    act->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
    connect(act, &QAction::triggered, this->renderWidget, &RenderWidget::resetView);

    QMenu *showWindowMenu = viewMenu->addMenu("Show");
    act = showWindowMenu->addAction("Only X");
    act->setShortcut(Qt::Key_F6);
    connect(act, &QAction::triggered, this->renderWidget, &RenderWidget::showX);
    act = showWindowMenu->addAction("Only Y");
    act->setShortcut(Qt::Key_F7);
    connect(act, &QAction::triggered, this->renderWidget, &RenderWidget::showY);
    act = showWindowMenu->addAction("Only Z");
    act->setShortcut(Qt::Key_F8);
    connect(act, &QAction::triggered, this->renderWidget, &RenderWidget::showZ);
    act = showWindowMenu->addAction("Only 3D");
    act->setShortcut(Qt::Key_F9);
    connect(act, &QAction::triggered, this->renderWidget, &RenderWidget::show3D);
    act = showWindowMenu->addAction("All windows");
    act->setShortcut(Qt::Key_F10);
    connect(act, &QAction::triggered, this->renderWidget, &RenderWidget::showAll);

    /* Plugin menu */
    QMenu *pluginMenu = menuBar()->addMenu("Plugin");
    if(loader.plugins().empty()){
        act = pluginMenu->addAction("<none>");
        act->setDisabled(true);
    } else {
        for(auto plugin : loader.plugins()) {
            act = pluginMenu->addAction(plugin);
            act->setData(QVariant(plugin));
            connect(act, &QAction::triggered, &loader, &PluginLoader::runPlugin);
        }
    }

    /* Help Menu */
    QMenu *helpMenu = menuBar()->addMenu("Help");
    act = helpMenu->addAction("About Qt");
    connect(act, &QAction::triggered, this, &QApplication::aboutQt);
    act = helpMenu->addAction("About");
    connect(act, &QAction::triggered, this, &MainWindow::about);
    act = helpMenu->addAction("Documentation");


} // MainWindow::createAndSetMenuBar

void FACGUI::MainWindow::createAndSetWidgets() {
    // Horizontal Box layout
    QHBoxLayout *layout = new QHBoxLayout(this);

    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    //format.setSamples(4);
#ifdef __APPLE__
    format.setSwapInterval(2);
#endif

    RenderWidget::setFormat(format);
    this->renderWidget = new RenderWidget(this);
    QSizePolicy renderPolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    renderPolicy.setHorizontalStretch(3);
    this->renderWidget->setSizePolicy(renderPolicy);
    this->renderWidget->show();
    layout->addWidget(this->renderWidget);

    this->optionsWidget = new OptionsWidget(this);
    QSizePolicy optionsPolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    optionsPolicy.setHorizontalStretch(1);
    this->optionsWidget->setSizePolicy(optionsPolicy);
    layout->addWidget(this->optionsWidget);

    this->centralWidget()->setLayout(layout);
} // MainWindow::createAndSetWidgets

bool FACGUI::MainWindow::event(QEvent *event) {
    if(event->type() == FAC::UserEvent::DocumentEdited) {
        this->documentWasModified();
        return true;
    }
    return QMainWindow::event(event);
}

void FACGUI::MainWindow::closeEvent(QCloseEvent *event) {
    if(!maybeSave()) {
        event->ignore();
    } else {
        event->accept();
    }
} // MainWindow::closeEvent

#include "optionswidget.h"
#include <numeric>

FACGUI::OptionsWidget::OptionsWidget(QWidget* parent) : QWidget(parent)
{
    this->setLocale(QLocale::C);

    this->pool = &FAC::FiberPool::getInstance();
    this->pool->addObserver(this);
    this->fibereditwidget = nullptr;
    this->toNotify = RenderData::getInstance();

    layout = new QVBoxLayout;
    general_layout = new QFormLayout;
    general_layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    function_layout = new QFormLayout;
    function_layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine); // Horizontal line
    line->setFrameShadow(QFrame::Sunken);
    line->setLineWidth(1);

    QFrame *line2 = new QFrame(this);
    line2->setFrameShape(QFrame::HLine); // Horizontal line
    line2->setFrameShadow(QFrame::Sunken);
    line2->setLineWidth(1);

    this->colorBox = new QComboBox(this);
    colorBox->addItem(tr("RGB-Orientation"));
    colorBox->addItem(tr("HSV-Orientation"));
    colorBox->addItem(tr("Fiber"));
    colorBox->addItem(tr("Plain Gray"));
    connect(colorBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateColor()));

    this->groups = new QListWidget(this);
    groups->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(groups, &QListWidget::itemChanged, this, &FACGUI::OptionsWidget::setGroupAppearance);

    this->fibers = new QListWidget(this);
    fibers->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(fibers, &QListWidget::itemChanged, this, &FACGUI::OptionsWidget::setFiberAppearance);
    connect(fibers, &QListWidget::itemDoubleClicked, this, &FACGUI::OptionsWidget::openFiberEditForFiber);

    this->rotationSlider_x = new QSlider(this);
    rotationSlider_x->setRange(0, 359);
    rotationSlider_x->setOrientation(Qt::Horizontal);
    rotationSlider_x->setTickInterval(10);
    this->rotationValue_x = new QLabel("0", this);
    connect(rotationSlider_x, SIGNAL(valueChanged(int)), rotationValue_x, SLOT(setNum(int)));

    this->rotationSlider_y = new QSlider(this);
    rotationSlider_y->setRange(0, 359);
    rotationSlider_y->setOrientation(Qt::Horizontal);
    rotationSlider_y->setTickInterval(10);
    this->rotationValue_y = new QLabel("0", this);
    connect(rotationSlider_y, SIGNAL(valueChanged(int)), rotationValue_y, SLOT(setNum(int)));

    this->rotationSlider_z = new QSlider(this);
    rotationSlider_z->setRange(0, 359);
    rotationSlider_z->setOrientation(Qt::Horizontal);
    rotationSlider_z->setTickInterval(10);
    this->rotationValue_z = new QLabel("0", this);
    connect(rotationSlider_z, SIGNAL(valueChanged(int)), rotationValue_z, SLOT(setNum(int)));

    function_min = new QDoubleSpinBox(this);
    function_min->setRange(INT_MIN, INT_MAX);
    function_max = new QDoubleSpinBox(this);
    function_max->setRange(INT_MIN, INT_MAX);

    function_x_transformation = new QDoubleSpinBox(this);
    function_x_transformation->setRange(-10e6, 10e6);
    function_y_transformation = new QDoubleSpinBox(this);
    function_y_transformation->setRange(-10e6, 10e6);
    function_z_transformation = new QDoubleSpinBox(this);
    function_z_transformation->setRange(-10e6, 10e6);

    function_x = new QLineEdit(this);
    function_y = new QLineEdit(this);
    function_z = new QLineEdit(this);
    radius = new QLineEdit(this);
    radius->setText("0.1");

    generateFunction = new QPushButton("Generate function");

    general_layout->addRow(tr("Color: "), colorBox);
    general_layout->addRow(new QLabel("Groups:", this));

    general_layout->addRow(this->groups);
    general_layout->addRow(new QLabel("Fibers: ", this));
    general_layout->addRow(this->fibers);

    function_layout->addRow(tr("r(t) = "), radius);
    function_layout->addRow(line);
    function_layout->addRow(tr("Minimum: "), function_min);
    function_layout->addRow(tr("Maximum: "), function_max);
    function_layout->addRow(tr("x(t) = "), function_x);
    function_layout->addRow(tr("y(t) = "), function_y);
    function_layout->addRow(tr("z(t) = "), function_z);
    function_layout->addRow(tr("x-Axis "), rotationValue_x);
    function_layout->addRow(tr("rotation: "), rotationSlider_x);
    function_layout->addRow(tr("y-Axis "), rotationValue_y);
    function_layout->addRow(tr("rotation: "), rotationSlider_y);
    function_layout->addRow(tr("z-Axis "), rotationValue_z);
    function_layout->addRow(tr("rotation: "), rotationSlider_z);
    function_layout->addRow(tr("x translation: "), function_x_transformation);
    function_layout->addRow(tr("y translation: "), function_y_transformation);
    function_layout->addRow(tr("z translation: "), function_z_transformation);
    function_layout->addRow(generateFunction);
    connect(generateFunction, &QPushButton::pressed, this, &FACGUI::OptionsWidget::convertAndApplyFunctions);

    layout->addItem(general_layout);
    layout->addWidget(line2);
    layout->addItem(function_layout);
    this->setLayout(layout);
}

FACGUI::OptionsWidget::~OptionsWidget() {
    pool->removeObserver(this);
}

bool FACGUI::OptionsWidget::event(QEvent *event) {
    if(event->type() == FAC::UserEvent::FiberHidden || event->type() == FAC::UserEvent::DocumentEdited) {
        updateFibers();
        updateGroups();
        return true;
    }
    return QWidget::event(event);
}

void FACGUI::OptionsWidget::createGroup() {
    QList<QListWidgetItem*> selected = this->fibers->selectedItems();
    if(selected.size() == 0) {
        QMessageBox::information(this, "No fiber selected", "Plase select at least one fiber before using this action", QMessageBox::Ok);
        return;
    }
    QList<unsigned> selectedIndex;
    for(auto element = selected.begin(); element != selected.end(); ++element) {
        selectedIndex.push_back((*element)->text().toUInt());
    }
    if(!selectedIndex.empty()) this->pool->createGroup(selectedIndex);
    updateGroups();
}

void FACGUI::OptionsWidget::deleteGroup() {
    if(this->groups->selectedItems().empty()) {
        QMessageBox::information(this, "No group selected", "Plase select a group before using this action", QMessageBox::Ok);
        return;
    }
    auto selected = this->groups->selectedItems();
    // Sortiere von hinten nach vorne
    qSort(selected.begin(), selected.end(), [] (QListWidgetItem* left, QListWidgetItem* right) {
       return left->text().toUInt() > right->text().toUInt();
    });
    for(auto element = selected.begin(); element != selected.end(); ++element) {
        this->pool->deleteGroup((*element)->text().toUInt());
    }
    updateGroups();
}

void FACGUI::OptionsWidget::addToGroup() {
    if(this->groups->selectedItems().empty()) {
        QMessageBox::information(this, "No group selected", "Plase select a group before using this action", QMessageBox::Ok);
        return;
    }
    auto selectedFibers = this->fibers->selectedItems();
    auto selectedGroups = this->groups->selectedItems();
    QList<unsigned> selectedIndex;
    for(auto element = selectedFibers.begin(); element != selectedFibers.end(); ++element) {
        selectedIndex.push_back((*element)->text().toUInt());
    }
    for(auto selected = selectedGroups.begin(); selected != selectedGroups.end(); ++selected) {
        this->pool->addFibersToGroup((*selected)->text().toUInt(), selectedIndex);
    }
    updateGroups();
}

void FACGUI::OptionsWidget::updateGroups() {
    disconnect(groups, &QListWidget::itemChanged, this, &FACGUI::OptionsWidget::setGroupAppearance);
    this->groups->clear();
    for(unsigned i = 0; i < pool->getGroups().size(); ++i) {
        QListWidgetItem * item = new QListWidgetItem(QString::number(i), this->groups);
        auto group = pool->getGroups().at(i);
        bool fiberActive = false;
        for(auto element : group) {
            if(pool->fiberAt(element).isActive()) {
                fiberActive = true;
                break;
            }
        }
        item->setCheckState(fiberActive ? Qt::Checked : Qt::Unchecked);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    }
    connect(groups, &QListWidget::itemChanged, this, &FACGUI::OptionsWidget::setGroupAppearance);
}

void FACGUI::OptionsWidget::transformGroup() {
    if(this->groups->selectedItems().size() > 0) {
        FAC::float3 toTransform = transformationDialog();
        auto selectedGroups = this->groups->selectedItems();
        std::set<unsigned> translatedFibers;
        for(auto selected = selectedGroups.begin(); selected != selectedGroups.end(); ++selected) {
            auto group = this->pool->groupAt((*selected)->text().toUInt());
            for(auto fiber = group.begin(); fiber != group.end(); ++fiber) {
                // Check if fiber was already translated
                if(translatedFibers.find(*fiber) == translatedFibers.end()) {
                    translatedFibers.insert(*fiber);
                    // Translate fiber
                    pool->fiberAt(*fiber).translate(toTransform.x, toTransform.y, toTransform.z);
                }
            }
        }
        this->pool->notifyObservers(FAC::UserEvent::DocumentEdited);
    } else {
        QMessageBox::information(this, "No group selected", "Plase select a group before using this action", QMessageBox::Ok);
    }
}

void FACGUI::OptionsWidget::rotateGroup() {
    if(this->groups->selectedItems().size() > 0) {
        FAC::float3 toTransform = rotationDialog();
        auto selectedGroups = this->groups->selectedItems();
        std::set<unsigned> rotatedFibers;
        for(auto selected = selectedGroups.begin(); selected != selectedGroups.end(); ++selected) {
            auto group = this->pool->groupAt((*selected)->text().toUInt());
            for(auto fiber = group.begin(); fiber != group.end(); ++fiber) {
                if(rotatedFibers.find(*fiber) == rotatedFibers.end()) {
                    rotatedFibers.insert(*fiber);
                    pool->fiberAt(*fiber).rotate(toTransform.x, toTransform.y, toTransform.z);
                }
            }
        }
        this->pool->notifyObservers(FAC::UserEvent::DocumentEdited);
    } else {
        QMessageBox::information(this, "No group selected", "Plase select a group before using this action", QMessageBox::Ok);
    }
}

void FACGUI::OptionsWidget::duplicateGroup() {
    if(this->groups->selectedItems().size() == 1) {
        QDialog dialog(this);
        QFormLayout layout(&dialog);
        layout.addRow(new QLabel("Duplicate values as well?"));

        QDialogButtonBox buttonBox(QDialogButtonBox::Yes | QDialogButtonBox::No,
                                   Qt::Horizontal, &dialog);
        layout.addRow(&buttonBox);
        QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        auto selectedGroup = this->pool->groupAt(this->groups->selectedItems().at(0)->text().toUInt());
        if(dialog.exec() == QDialog::Accepted) {
            std::set<unsigned> toAddToGroup;
            for(auto fiber : selectedGroup) {
                FAC::Fiber f(pool->fiberAt(fiber));
                pool->addFiber(f);
                toAddToGroup.insert(pool->getFibers().size()-1);
            }
            pool->createGroup(toAddToGroup);
            this->pool->notifyObservers(FAC::UserEvent::DocumentEdited);
        } else {
            pool->createGroup(selectedGroup);
            updateGroups();
        }
    } else {
        QMessageBox::information(this, "Too many or no groups selected", "Plase select only one group before using this action", QMessageBox::Ok);
    }
}

void FACGUI::OptionsWidget::joinGroups() {
    auto groups = this->groups->selectedItems();
    if(groups.size() < 2) {
        QMessageBox::information(this, "Not enough groups selected.", "This action requires at least two selected groups.", QMessageBox::Ok);
    } else {
        // Create group and fill it with all selected groups
        pool->createGroup(pool->groupAt(groups.at(0)->text().toUInt()));
        for(int i = 1; i < groups.size(); ++i) {
            pool->addFibersToGroup(pool->getGroups().size()-1, pool->groupAt(groups.at(i)->text().toUInt()));
        }

        // Ask if other groups shall be maintained
        QDialog dialog(this);
        QFormLayout layout(&dialog);
        layout.addRow(new QLabel("Do you want to keep the groups?"));

        QDialogButtonBox buttonBox(QDialogButtonBox::Yes | QDialogButtonBox::No,
                                   Qt::Horizontal, &dialog);
        layout.addRow(&buttonBox);
        QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);


        if(dialog.exec() == QDialog::Rejected) {
            // Sort the groups so that the removal is done from the back.
            // ...->selectedItems() returns the elements in the order of selection.
            qSort(groups.begin(), groups.end(), [] (QListWidgetItem* left, QListWidgetItem* right) {
               return left->text().toUInt() > right->text().toUInt();
            });
            for(int i = 0; i < groups.size(); ++i)
                pool->deleteGroup(groups.at(i)->text().toUInt());
        }

        updateGroups();
    }
}

void FACGUI::OptionsWidget::updateFibers() {
    disconnect(fibers, &QListWidget::itemChanged, this, &FACGUI::OptionsWidget::setFiberAppearance);
    this->fibers->clear();
    for(unsigned i = 0; i < pool->getFibers().size(); ++i) {
        QListWidgetItem * item = new QListWidgetItem(QString::number(i), this->fibers);
        item->setCheckState(pool->fiberAt(i).isActive() ? Qt::Checked : Qt::Unchecked);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    }
    connect(fibers, &QListWidget::itemChanged, this, &FACGUI::OptionsWidget::setFiberAppearance);
}

void FACGUI::OptionsWidget::generateFiberFromFile() {
    //TODO
    QString fileName = QFileDialog::getOpenFileName(nullptr, tr("Open Fiber Model Data"), QDir::homePath(), tr("Comma Seperated Values (*.csv) ;; Text File (*.txt) ;; All Files (*.*)"), new QString("Text File (*.txt)"), QFileDialog::DontUseNativeDialog);
    if(!fileName.isNull()) {
        FAC::T radius = this->radius->text().toFloat();
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(nullptr, "File can't be read", file.errorString());
            return;
        } else {
            FAC::Fiber generatedFiber;
            QString line;

            while(!file.atEnd()) {
                if((line = file.readLine().simplified()).isEmpty()) continue;
                QStringList splitted = line.split(QRegExp("[\\t\\s;,]+"));
                if(splitted.size() < 3) {
                    QMessageBox::critical(nullptr, "Error while creating fiber", "Less than 3 values in one line. Cannot parse fiber.\nLine is: " + line);
                    return;
                } else if(splitted.size() > 3) {
                    radius = splitted.at(3).toFloat();
                }
                generatedFiber.addPoint(FAC::FiberPoint(splitted.at(0).toFloat(), splitted.at(1).toFloat(), splitted.at(2).toFloat(), radius));
            }
            openFiberEditNewFiber();
            this->fibereditwidget->setFiber(generatedFiber);
        }
    }
}

void FACGUI::OptionsWidget::rotateFiber() {
    if(this->fibers->selectedItems().size() > 0) {
        FAC::float3 toTransform = rotationDialog();
        auto selectedIndices = this->fibers->selectedItems();
        for(auto indice : selectedIndices) {
            this->pool->fiberAt(indice->text().toUInt()).rotate(toTransform.x, toTransform.y, toTransform.z);
        }
        this->pool->notifyObservers(FAC::UserEvent::DocumentEdited);
    } else {
        QMessageBox::information(this, "No fiber selected", "Plase select at least one fiber before using this action", QMessageBox::Ok);
    }
}

void FACGUI::OptionsWidget::transformFiber() {
    if(this->fibers->selectedItems().size() > 0) {
        FAC::float3 toTransform = transformationDialog();
        auto selectedIndices = this->fibers->selectedItems();
        for(auto indice : selectedIndices) {
            this->pool->fiberAt(indice->text().toUInt()).translate(toTransform.x, toTransform.y, toTransform.z);
        }
        this->pool->notifyObservers(FAC::UserEvent::DocumentEdited);
    } else {
        QMessageBox::information(this, "No fiber selected", "Plase select at least one fiber before using this action", QMessageBox::Ok);
    }
}

void FACGUI::OptionsWidget::duplicateFiber() {
    if(this->fibers->selectedItems().size() > 0) {
        auto selectedIndices = this->fibers->selectedItems();
        for(auto indice : selectedIndices) {
            FAC::Fiber fib(this->pool->fiberAt(indice->text().toUInt()));
            this->pool->addFiber(fib);
        }
        this->pool->notifyObservers(FAC::UserEvent::DocumentEdited);
    } else {
        QMessageBox::information(this, "No fiber selected", "Plase select at least one fiber before using this action", QMessageBox::Ok);
    }
}

void FACGUI::OptionsWidget::thinFiber() {
    QList<QListWidgetItem*> selected = this->fibers->selectedItems();
    if(selected.size() == 0) {
        QMessageBox::information(this, "No fiber selected", "Plase select at least one fiber before using this action", QMessageBox::Ok);
        return;
    }

    // Ask for parameters
    QDialog dialog(this);
    dialog.setLocale(QLocale::C);
    QFormLayout layout(&dialog);
    layout.addRow(new QLabel("Thinning out fibers. Start- and endpoint are kept in the process."));

    QSpinBox thin(&dialog);
    thin.setRange(0, INT_MAX);

    layout.addRow(tr("Remove every n-th data point: "), &thin);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    layout.addRow(&buttonBox);
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    dialog.setLayout(&layout);
    if(dialog.exec() == QDialog::Accepted) {
        FAC::Fiber * f;
        for(auto element : selected) {
            f = &pool->fiberAt(element->text().toUInt());
            f->thin(thin.text().toInt());
        }
        this->pool->notifyObservers(FAC::UserEvent::DocumentEdited);
    }
}

void FACGUI::OptionsWidget::deleteFiber() {
    QList<QListWidgetItem*> selected = this->fibers->selectedItems();
    if(selected.size() == 0) {
        QMessageBox::information(this, "No fiber selected", "Plase select at least one fiber before using this action", QMessageBox::Ok);
        return;
    }
    QList<unsigned> toDelete;
    for(auto element : selected) {
        toDelete.append(element->text().toUInt());
    }
    this->pool->deleteFiber(toDelete);
    this->pool->notifyObservers(FAC::UserEvent::DocumentEdited);
}

void FACGUI::OptionsWidget::createBundleFromFiber() {
    QList<QListWidgetItem*> selected = this->fibers->selectedItems();
    if(selected.size() == 0) {
        QMessageBox::information(this, "No fiber selected", "Plase select at least one fiber before using this action", QMessageBox::Ok);
        return;
    }
    // Ask for parameters
    QDialog dialog(this);
    dialog.setLocale(QLocale::C);
    QFormLayout layout(&dialog);
    layout.addRow(new QLabel("Create bundle from fiber"));

    QDoubleSpinBox radius(&dialog);
    radius.setRange(0.00, 10e9);
    QDoubleSpinBox distance(&dialog);
    distance.setRange(0.00, 10e9);

    layout.addRow(tr("Radius: "), &radius);
    layout.addRow(tr("Distance"), &distance);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    layout.addRow(&buttonBox);
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    dialog.setLayout(&layout);
    if(dialog.exec() == QDialog::Accepted) {
        // Sort from back to front
        qSort(selected.begin(), selected.end(), [] (QListWidgetItem* left, QListWidgetItem* right) {
           return left->text().toUInt() > right->text().toUInt();
        });
        // For every selected fiber
        #pragma omp parallel for
        for (unsigned i = 0; i < selected.size(); ++i) {
            auto element = selected.at(i);
            std::vector<FAC::Fiber> fibers = FAC::BundleCreation::CreateFibersFromFiberBundle(pool->fiberAt(element->text().toUInt()), radius.text().toFloat(), distance.text().toFloat());
            if(fibers.size() > 2) {
                // Add new fibers
                unsigned previousSize = pool->getFibers().size();
                // TODO Can be optimized
                #pragma omp critical
                {
                    pool->addFiber(fibers);
                }
                // Create group of all new fibers
                std::list<unsigned> indices;
                indices.resize(pool->getFibers().size() - previousSize);
                std::iota(std::begin(indices), std::end(indices), previousSize);
                pool->createGroup(indices);
            }
        }
        /*// Delete old fibers
        for(auto element : selected) {
            pool->deleteFiber(element->text().toUInt());
        }*/
        this->pool->notifyObservers(FAC::UserEvent::DocumentEdited);
    }
}

void FACGUI::OptionsWidget::convertAndApplyFunctions() {
    try {
        FAC::FiberFunction function(function_x->text(), function_y->text(), function_z->text(), function_min->text().toFloat(), function_max->text().toFloat(),
                               radius->text(), rotationValue_x->text().toFloat(), rotationValue_y->text().toFloat(),
                               rotationValue_z->text().toFloat(), function_x_transformation->text().toFloat(), function_y_transformation->text().toFloat(),
                               function_z_transformation->text().toFloat());
        FAC::Fiber fiber = FAC::Fiber::createByFunction(function, Settings::functionStepSize());
        this->pool->addFiber(fiber);
        this->pool->notifyObservers(FAC::UserEvent::DocumentEdited);

    } catch (QString error) {
        QMessageBox::critical(this, "Error while creating fiber bundle via functions", error);
    }

}

void FACGUI::OptionsWidget::setFiberAppearance(QListWidgetItem *item) {
    bool checkState = item->checkState() == Qt::Checked ? true : false;
    pool->fiberAt(item->text().toUInt()).setActive(checkState);
    this->pool->notifyObservers(FAC::UserEvent::FiberHidden);
}

void FACGUI::OptionsWidget::setGroupAppearance(QListWidgetItem * item) {
    auto group = pool->groupAt(item->text().toUInt());
    for(auto element = group.begin(); element != group.end(); ++element) {
        pool->fiberAt(*element).setActive(item->checkState() == Qt::Checked ? true : false);
    }
    updateFibers();
    this->pool->notifyObservers(FAC::UserEvent::FiberHidden);
}

void FACGUI::OptionsWidget::openFiberEditForFiber(QListWidgetItem * item) {
    if(fibereditwidget) {
        fibereditwidget->deleteLater();
    }
    fibereditwidget = new FiberEditWidget;
    if(item == nullptr) {
        fibereditwidget->openEmpty();
        fibereditwidget->show();
    } else {
        int index = item->text().toUInt();
        FAC::Fiber f = pool->fiberAt(index);
        if(!f.function().empty()) {
            auto reply = QMessageBox::question(this, "Fiber bundle was created with functions.", "This fiber bundle was created by the usage of parametric functions. Do you want to edit this function?");
            if(reply == QMessageBox::Yes) {
                functionEditDialog(f, index);
            } else {
                fibereditwidget->openGivenFiber(f, item->text().toUInt());
                fibereditwidget->show();
            }
        } else {
            fibereditwidget->openGivenFiber(f, item->text().toUInt());
            fibereditwidget->show();
        }
    }
}

void FACGUI::OptionsWidget::openFiberEditNewFiber() {
    if(fibereditwidget) {
        fibereditwidget->deleteLater();
    }
    fibereditwidget = new FiberEditWidget;
    fibereditwidget->openEmpty();
    fibereditwidget->show();
}

void FACGUI::OptionsWidget::updateColor() {
    if(toNotify) {
        toNotify->setColor(this->colorBox->currentIndex());
        this->pool->notifyObservers(FAC::UserEvent::FiberHidden);
    }
}

FAC::float3 FACGUI::OptionsWidget::transformationDialog() {
    QDialog dialog(this);
    dialog.setLocale(QLocale::C);
    QFormLayout layout(&dialog);
    layout.addRow(new QLabel("Values for the transformation:", &dialog));

    QDoubleSpinBox * x_value = new QDoubleSpinBox(&dialog);
    x_value->setRange(-100, 100);
    QDoubleSpinBox * y_value = new QDoubleSpinBox(&dialog);
    y_value->setRange(-100, 100);
    QDoubleSpinBox * z_value = new QDoubleSpinBox(&dialog);
    z_value->setRange(-100, 100);

    layout.addRow("x: ", x_value);
    layout.addRow("y: ", y_value);
    layout.addRow("z: ", z_value);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    layout.addRow(&buttonBox);
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    FAC::float3 returnValue;
    if(dialog.exec() == QDialog::Accepted) {
        returnValue.x = x_value->text().toFloat();
        returnValue.y = y_value->text().toFloat();
        returnValue.z = z_value->text().toFloat();
    }
    return returnValue;
}

FAC::float3 FACGUI::OptionsWidget::rotationDialog() {
    QDialog dialog(this);
    dialog.setLocale(QLocale::C);
    QFormLayout layout(&dialog);
    layout.addRow(new QLabel("Values for the rotation:", &dialog));

    QDoubleSpinBox * x_value = new QDoubleSpinBox(&dialog);
    x_value->setRange(0, 359);
    QDoubleSpinBox * y_value = new QDoubleSpinBox(&dialog);
    y_value->setRange(0, 359);
    QDoubleSpinBox * z_value = new QDoubleSpinBox(&dialog);
    z_value->setRange(0, 359);

    layout.addRow("x: ", x_value);
    layout.addRow("y: ", y_value);
    layout.addRow("z: ", z_value);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    layout.addRow(&buttonBox);
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    FAC::float3 returnValue;
    if(dialog.exec() == QDialog::Accepted) {
        returnValue.x = x_value->text().toFloat();
        returnValue.y = y_value->text().toFloat();
        returnValue.z = z_value->text().toFloat();
    }
    return returnValue;
}

void FACGUI::OptionsWidget::functionEditDialog(FAC::Fiber f, uint index) {
    QDialog dialog(this);
    dialog.setLocale(QLocale::C);
    QFormLayout layout(&dialog);
    layout.addRow(new QLabel("Function edit:", &dialog));

    FAC::FiberFunction function = f.function();

    QLineEdit * radius = new QLineEdit(this);
    radius->setText("0.1");

    QSlider * rotationSlider_x = new QSlider(&dialog);
    rotationSlider_x->setRange(0, 359);
    rotationSlider_x->setOrientation(Qt::Horizontal);
    rotationSlider_x->setTickInterval(10);
    rotationSlider_x->setValue(function.rotX());
    QLabel * rotationValue_x = new QLabel(QString::number(rotationSlider_x->value()), &dialog);
    QSlider * rotationSlider_y = new QSlider(&dialog);
    rotationSlider_y->setRange(0, 359);
    rotationSlider_y->setOrientation(Qt::Horizontal);
    rotationSlider_y->setTickInterval(10);
    rotationSlider_y->setValue(function.rotY());
    QLabel * rotationValue_y = new QLabel(QString::number(rotationSlider_y->value()), &dialog);
    QSlider * rotationSlider_z = new QSlider(&dialog);
    rotationSlider_z->setRange(0, 359);
    rotationSlider_z->setOrientation(Qt::Horizontal);
    rotationSlider_z->setTickInterval(10);
    rotationSlider_z->setValue(function.rotZ());
    QLabel * rotationValue_z = new QLabel(QString::number(rotationSlider_z->value()), &dialog);

    connect(rotationSlider_x, SIGNAL(valueChanged(int)), rotationValue_x, SLOT(setNum(int)));
    connect(rotationSlider_y, SIGNAL(valueChanged(int)), rotationValue_y, SLOT(setNum(int)));
    connect(rotationSlider_z, SIGNAL(valueChanged(int)), rotationValue_z, SLOT(setNum(int)));

    QDoubleSpinBox * function_min = new QDoubleSpinBox(&dialog);
    function_min->setRange(INT_MIN, INT_MAX);
    function_min->setValue(function.min());
    QDoubleSpinBox * function_max = new QDoubleSpinBox(&dialog);
    function_max->setRange(INT_MIN, INT_MAX);
    function_max->setValue(function.max());

    QDoubleSpinBox * function_x_transformation = new QDoubleSpinBox(&dialog);
    function_x_transformation->setRange(-10e6, 10e6);
    function_x_transformation->setValue(function.transformX());
    QDoubleSpinBox * function_y_transformation = new QDoubleSpinBox(&dialog);
    function_y_transformation->setRange(-10e6, 10e6);
    function_y_transformation->setValue(function.transformY());
    QDoubleSpinBox * function_z_transformation = new QDoubleSpinBox(&dialog);
    function_z_transformation->setRange(-10e6, 10e6);
    function_z_transformation->setValue(function.transformZ());

    QLineEdit * function_x = new QLineEdit(function.functionX(), &dialog);
    QLineEdit * function_y = new QLineEdit(function.functionY(), &dialog);
    QLineEdit * function_z = new QLineEdit(function.functionZ(), &dialog);

    layout.addRow(tr("r(t) = "), radius);
    layout.addRow(tr("Minimum: "), function_min);
    layout.addRow(tr("Maximum: "), function_max);
    layout.addRow(tr("x(t) = "), function_x);
    layout.addRow(tr("y(t) = "), function_y);
    layout.addRow(tr("z(t) = "), function_z);
    layout.addRow(tr("x-Axis "), rotationValue_x);
    layout.addRow(tr("rotation: "), rotationSlider_x);
    layout.addRow(tr("y-Axis "), rotationValue_y);
    layout.addRow(tr("rotation: "), rotationSlider_y);
    layout.addRow(tr("z-Axis "), rotationValue_z);
    layout.addRow(tr("rotation: "), rotationSlider_z);
    layout.addRow(tr("x translation: "), function_x_transformation);
    layout.addRow(tr("y translation: "), function_y_transformation);
    layout.addRow(tr("z translation: "), function_z_transformation);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    layout.addRow(&buttonBox);
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if(dialog.exec() == QDialog::Accepted) {
        FAC::FiberFunction function(function_x->text(), function_y->text(), function_z->text(), function_min->text().toFloat(), function_max->text().toFloat(),
                               radius->text(), rotationValue_x->text().toFloat(), rotationValue_y->text().toFloat(),
                               rotationValue_z->text().toFloat(), function_x_transformation->text().toFloat(), function_y_transformation->text().toFloat(),
                               function_z_transformation->text().toFloat());
        FAC::Fiber fiber = FAC::Fiber::createByFunction(function, Settings::functionStepSize());
        pool->replaceFiber(fiber, index);
        pool->notifyObservers(FAC::UserEvent::DocumentEdited);
    }
}

#include "fibereditwidget.h"

FACGUI::FiberEditWidget::FiberEditWidget() {
    this->table = new QTableWidget(this);
    table->setColumnCount(4);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QList<QString> names = {"x", "y", "z", "r"};
    table->setHorizontalHeaderLabels(QStringList(names));
    table->setShowGrid(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->insertRow(table->rowCount());
    connect(table, &QTableWidget::cellChanged, this, &FiberEditWidget::reactChanged);

    hlayout = new QHBoxLayout(this);
    hlayout->addWidget(table, 4);
    vlayout = new QVBoxLayout();
    doNothing = new QRadioButton("none", this);
    doNothing->setChecked(true);
    interpolate = new QRadioButton("cubic spline interpolation", this);
    regression = new QRadioButton("linear interpolation", this);
    QLabel * label = new QLabel("Interpolation method", this);
    label->setWordWrap(true);
    vlayout->addWidget(label);
    vlayout->addWidget(doNothing);
    vlayout->addWidget(regression);
    vlayout->addWidget(interpolate);
    hlayout->addLayout(vlayout, 1);

    changed = false;
    this->pool = &FAC::FiberPool::getInstance();
}

void FACGUI::FiberEditWidget::setFiber(FAC::Fiber& fiber) {
    disconnect(table, &QTableWidget::cellChanged, this, &FiberEditWidget::reactChanged);
    this->fib = &fiber;
    table->clear();
    int currentRow;
    std::vector<float> coords;
    for(auto point : fib->getParts()) {
        currentRow = table->rowCount() - 1;
        coords = {point.x(), point.y(), point.z(), point.r()};
        for(unsigned i = 0; i < coords.size(); ++i) {
            table->setItem(currentRow, int(i), new QTableWidgetItem(QString::number(coords.at(i))));
        }
        table->insertRow(table->rowCount());
    }
    connect(table, &QTableWidget::cellChanged, this, &FiberEditWidget::reactChanged);
}

void FACGUI::FiberEditWidget::openGivenFiber(FAC::Fiber& fiber, unsigned index) {
    this->setFiber(fiber);
    this->index = index;
    newFiber = false;
}

void FACGUI::FiberEditWidget::openEmpty() {
    this->fib = nullptr;
    newFiber = true;
}

void FACGUI::FiberEditWidget::closeEvent(QCloseEvent *event) {
    writeChangesToFiber();
}

void FACGUI::FiberEditWidget::keyPressEvent(QKeyEvent *event) {
    if(event->modifiers() & Qt::ControlModifier) {
        QItemSelectionModel* select = table->selectionModel();
        if(select->hasSelection()) {
            QModelIndexList row = select->selectedRows();
            switch(event->key()) {
                case Qt::Key_Delete:
                    table->removeRow(row.first().row());
                    this->changed = true;
                    break;
                case Qt::Key_J:
                    table->insertRow(row.first().row());
                    break;
                case Qt::Key_K:
                    table->insertRow(row.first().row()+1);
                    break;
                default: QWidget::keyPressEvent(event);
            }
        }
    } else {
        QWidget::keyPressEvent(event);
    }
}

void FACGUI::FiberEditWidget::reactChanged(int row, int column) {
    changed = true;
    if(row+1 == table->rowCount()) {
        table->insertRow(table->rowCount());
    }
}

void FACGUI::FiberEditWidget::writeChangesToFiber() {
    disconnect(table, &QTableWidget::cellChanged, this, &FiberEditWidget::reactChanged);
    // Read data from tables
    if(changed || newFiber || interpolate->isChecked() || regression->isChecked()) {
        data = std::vector<QVector4D>();
        float x, y, z, r;
        std::vector<float*> values = {&x, &y, &z, &r};
        for(int row = 0; row < table->rowCount()-1; ++row) {
            int column;
            for(column = 0; column < 4; ++column) {
                QTableWidgetItem * item = table->item(row, column);
                if(!item) {
                    table->removeRow(row);
                    if(row >= table->rowCount()-1) break;
                    column = -1;
                }
                else (*values.at(column)) = item->text().toFloat();
            }
            if(column == 4) data.push_back(QVector4D(x, y, z, r));
        }
        // Save data based on checked radio button
        if(doNothing->isChecked()) {
            transferFiberDataToPool();
        } else if(interpolate->isChecked()) {
            if(this->data.size() > 5) FAC::Interpolation::CubicInterpolate(this->data, Settings::cubSplineSteps());
            transferFiberDataToPool();
        } else if(regression->isChecked()) {
            FAC::Interpolation::LinearRegression(this->data, Settings::linInterpolationStepSize());
            transferFiberDataToPool();
        } else {
            qDebug() << "This cannot happen!";
        }
        this->pool->notifyObservers(FAC::UserEvent::DocumentEdited);
    }
    connect(table, &QTableWidget::cellChanged, this, &FACGUI::FiberEditWidget::reactChanged);
}

void FACGUI::FiberEditWidget::transferFiberDataToPool() {
    /* TODO: Optimize code for speed and ram usage */
    /* TODO: Multi core execution */
    if(data.size() < 2) {
        if(!newFiber) pool->deleteFiber(QList<unsigned> {index});
        return;
    }

    FAC::Fiber f(data.size());
    #pragma omp parallel for
    for(unsigned i = 0; i < data.size(); ++i) {
        f.setFiberPoint(i, static_cast<FAC::FiberPoint>(data.at(i)));
    }

    if(newFiber) {
        pool->addFiber(f);
    } else {
        pool->replaceFiber(f, index);
    }
}

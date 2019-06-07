#ifndef FIBEREDITWINDOW_H
#define FIBEREDITWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QGroupBox>
#include <QLabel>
#include <QVector4D>
#include <QDebug>
#include <QHeaderView>
#include "helpers.h"
#include "fiber.h"
#include "fiberpool.h"
#include "settings.h"
#include "widgetdef.h"

namespace FACGUI {
    class FiberEditWidget : public QWidget
    {
        Q_OBJECT
        MAKE_TESTABLE(FiberEditWidget)
    public:
        FiberEditWidget();
        void setFiber(FAC::Fiber& fiber);

    public slots:
        void openGivenFiber(FAC::Fiber& fiber, unsigned index);
        void openEmpty();

    private slots:
        void closeEvent(QCloseEvent *event);
        void keyPressEvent(QKeyEvent *event);
        void reactChanged(int row, int column);

    private:
        void writeChangesToFiber();
        void transferFiberDataToPool();

        QVBoxLayout * vlayout;
        QHBoxLayout * hlayout;
        QTableWidget * table;
        QRadioButton * doNothing;
        QRadioButton * interpolate;
        QRadioButton * regression;

        FAC::Fiber * fib;
        FAC::FiberPool * pool;
        std::vector<QVector4D> data;

        bool newFiber;
        bool changed;
        unsigned index;
    };
}

#endif // FIBEREDITWINDOW_H

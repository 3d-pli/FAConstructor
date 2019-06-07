#ifndef OPTIONSWIDGET_H
#define OPTIONSWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QSlider>
#include <QListWidget>
#include <QMessageBox>
#include <QSize>
#include <QFileDialog>
#include <QApplication>
#include <QDialogButtonBox>
#include <vector>
#include <climits>
#include <iostream>
#include "renderdata.h"
#include "fiberpool.h"
#include "fiberpoint.h"
#include "fiber.h"
#include "fibereditwidget.h"
#include "fiberfunction.h"
#include "settings.h"
#include "widgetdef.h"

namespace FACGUI {
    class OptionsWidget : public QWidget
    {
        Q_OBJECT
        MAKE_TESTABLE(OptionsWidget)

    public:
        OptionsWidget(QWidget* parent = nullptr);
        ~OptionsWidget();
        bool event(QEvent *event);

    public slots:
        void createGroup();
        void deleteGroup();
        void addToGroup();
        void updateGroups();
        void transformGroup();
        void rotateGroup();
        void duplicateGroup();
        void joinGroups();

        void updateFibers();
        void generateFiberFromFile();
        void openFiberEditForFiber(QListWidgetItem * item);
        void openFiberEditNewFiber();
        void rotateFiber();
        void transformFiber();
        void duplicateFiber();
        void deleteFiber();
        void thinFiber();
        void createBundleFromFiber();

    private slots:
        void convertAndApplyFunctions();
        void setGroupAppearance(QListWidgetItem * item);
        void setFiberAppearance(QListWidgetItem * item);
        void updateColor();

    private:
        QVBoxLayout * layout;
        QFormLayout * function_layout;
        QFormLayout * general_layout;

        QListWidget * groups;
        QListWidget * fibers;

        QComboBox * colorBox;
        QLineEdit * radius;
        QSlider * rotationSlider_x;
        QLabel * rotationValue_x;
        QSlider * rotationSlider_y;
        QLabel * rotationValue_y;
        QSlider * rotationSlider_z;
        QLabel * rotationValue_z;

        QFrame * seperatorShapes;
        QFrame * seperatorFunction;

        QDoubleSpinBox * function_min;
        QDoubleSpinBox * function_max;
        QDoubleSpinBox * function_x_transformation;
        QDoubleSpinBox * function_y_transformation;
        QDoubleSpinBox * function_z_transformation;
        QLineEdit * function_x;
        QLineEdit * function_y;
        QLineEdit * function_z;
        QPushButton * generateFunction;

        FAC::FiberPool * pool;
        RenderData * toNotify;
        FiberEditWidget * fibereditwidget;

        FAC::float3 transformationDialog();
        FAC::float3 rotationDialog();
        void functionEditDialog(FAC::Fiber f, uint index);
    };
}

#endif // OPTIONSWIDGET_H

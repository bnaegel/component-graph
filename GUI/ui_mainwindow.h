/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created: Mon Aug 25 10:29:43 2014
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>
#include <QSlider>
#include <QSpinBox>
#include <QStatusBar>
#include <QWidget>
#include <maindisplay.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionLoad_Image;
    QAction *actionSimplification;
    QAction *actionDetection;
    QAction *actionSynthetic_Image;
    QAction *actionImpulse_noise;
    QAction *actionGaussian_noise;
    QAction *actionSave_result;
    QWidget *centralwidget;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QScrollArea *scrollArea;
    MainDisplay *mainDisplay;
    QScrollArea *scrollArea_2;
    MainDisplay *mainDisplay2;
    QGroupBox *groupBox;
    QCheckBox *checkBoxOriginal;
    QCheckBox *checkBoxNoisy;
    QCheckBox *checkBoxDenoised;
    QSlider *areaMinSlider;
    QSlider *areaMaxSlider;
    QLabel *label_2;
    QLabel *label_3;
    QGroupBox *groupBox_2;
    QLabel *labelAreaMin;
    QLabel *labelAreaMax;
    QGroupBox *groupBox_3;
    QLabel *label_5;
    QSlider *contrastMaxSlider;
    QSlider *contrastMinSlider;
    QLabel *label_4;
    QLabel *labelContrastMin;
    QLabel *labelContrastMax;
    QPushButton *pushButton_2;
    QGroupBox *groupBox_4;
    QLabel *label_6;
    QLabel *label_7;
    QSpinBox *spinBoxNx;
    QSpinBox *spinBoxNy;
    QGroupBox *groupBox_5;
    QCheckBox *checkBoxRGB;
    QCheckBox *checkBoxHSV;
    QCheckBox *checkBoxSV;
    QGroupBox *groupBox_6;
    QCheckBox *checkBoxComponent;
    QCheckBox *checkBoxInverseComponent;
    QProgressBar *progressBar;
    QPushButton *adaptiveArea;
    QPushButton *adaptiveContrast;
    QSlider *adaptiveAreaSlider;
    QLabel *labelAdaptiveArea;
    QSlider *adaptiveContrastSlider;
    QLabel *labelAdaptiveContrast;
    QMenuBar *menubar;
    QMenu *menuComponent_Graph;
    QMenu *menuProcessing;
    QMenu *menuAdd_noise;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1345, 771);
        actionLoad_Image = new QAction(MainWindow);
        actionLoad_Image->setObjectName(QString::fromUtf8("actionLoad_Image"));
        actionSimplification = new QAction(MainWindow);
        actionSimplification->setObjectName(QString::fromUtf8("actionSimplification"));
        actionSimplification->setEnabled(false);
        actionDetection = new QAction(MainWindow);
        actionDetection->setObjectName(QString::fromUtf8("actionDetection"));
        actionDetection->setEnabled(false);
        actionSynthetic_Image = new QAction(MainWindow);
        actionSynthetic_Image->setObjectName(QString::fromUtf8("actionSynthetic_Image"));
        actionImpulse_noise = new QAction(MainWindow);
        actionImpulse_noise->setObjectName(QString::fromUtf8("actionImpulse_noise"));
        actionGaussian_noise = new QAction(MainWindow);
        actionGaussian_noise->setObjectName(QString::fromUtf8("actionGaussian_noise"));
        actionSave_result = new QAction(MainWindow);
        actionSave_result->setObjectName(QString::fromUtf8("actionSave_result"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        layoutWidget = new QWidget(centralwidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 40, 971, 471));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetMaximumSize);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        scrollArea = new QScrollArea(layoutWidget);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(scrollArea->sizePolicy().hasHeightForWidth());
        scrollArea->setSizePolicy(sizePolicy);
        scrollArea->setWidgetResizable(false);
        mainDisplay = new MainDisplay();
        mainDisplay->setObjectName(QString::fromUtf8("mainDisplay"));
        mainDisplay->setGeometry(QRect(0, 0, 365, 417));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(mainDisplay->sizePolicy().hasHeightForWidth());
        mainDisplay->setSizePolicy(sizePolicy1);
        mainDisplay->setMouseTracking(true);
        scrollArea->setWidget(mainDisplay);

        horizontalLayout->addWidget(scrollArea);

        scrollArea_2 = new QScrollArea(layoutWidget);
        scrollArea_2->setObjectName(QString::fromUtf8("scrollArea_2"));
        sizePolicy.setHeightForWidth(scrollArea_2->sizePolicy().hasHeightForWidth());
        scrollArea_2->setSizePolicy(sizePolicy);
        scrollArea_2->setWidgetResizable(false);
        mainDisplay2 = new MainDisplay();
        mainDisplay2->setObjectName(QString::fromUtf8("mainDisplay2"));
        mainDisplay2->setGeometry(QRect(0, 0, 364, 417));
        sizePolicy1.setHeightForWidth(mainDisplay2->sizePolicy().hasHeightForWidth());
        mainDisplay2->setSizePolicy(sizePolicy1);
        scrollArea_2->setWidget(mainDisplay2);

        horizontalLayout->addWidget(scrollArea_2);

        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 530, 121, 101));
        checkBoxOriginal = new QCheckBox(groupBox);
        checkBoxOriginal->setObjectName(QString::fromUtf8("checkBoxOriginal"));
        checkBoxOriginal->setGeometry(QRect(0, 30, 101, 18));
        checkBoxOriginal->setChecked(true);
        checkBoxOriginal->setAutoExclusive(true);
        checkBoxNoisy = new QCheckBox(groupBox);
        checkBoxNoisy->setObjectName(QString::fromUtf8("checkBoxNoisy"));
        checkBoxNoisy->setGeometry(QRect(0, 50, 101, 18));
        checkBoxNoisy->setAutoExclusive(true);
        checkBoxDenoised = new QCheckBox(groupBox);
        checkBoxDenoised->setObjectName(QString::fromUtf8("checkBoxDenoised"));
        checkBoxDenoised->setGeometry(QRect(0, 70, 101, 18));
        checkBoxDenoised->setAutoExclusive(true);
        areaMinSlider = new QSlider(centralwidget);
        areaMinSlider->setObjectName(QString::fromUtf8("areaMinSlider"));
        areaMinSlider->setGeometry(QRect(1030, 70, 221, 29));
        areaMinSlider->setOrientation(Qt::Horizontal);
        areaMinSlider->setTickPosition(QSlider::NoTicks);
        areaMaxSlider = new QSlider(centralwidget);
        areaMaxSlider->setObjectName(QString::fromUtf8("areaMaxSlider"));
        areaMaxSlider->setGeometry(QRect(1030, 120, 221, 29));
        areaMaxSlider->setOrientation(Qt::Horizontal);
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(990, 50, 66, 17));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(990, 100, 66, 17));
        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(979, 29, 281, 131));
        labelAreaMin = new QLabel(groupBox_2);
        labelAreaMin->setObjectName(QString::fromUtf8("labelAreaMin"));
        labelAreaMin->setGeometry(QRect(130, 30, 56, 13));
        labelAreaMax = new QLabel(groupBox_2);
        labelAreaMax->setObjectName(QString::fromUtf8("labelAreaMax"));
        labelAreaMax->setGeometry(QRect(130, 80, 56, 13));
        groupBox_3 = new QGroupBox(centralwidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(980, 190, 281, 131));
        label_5 = new QLabel(groupBox_3);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(10, 20, 91, 17));
        contrastMaxSlider = new QSlider(groupBox_3);
        contrastMaxSlider->setObjectName(QString::fromUtf8("contrastMaxSlider"));
        contrastMaxSlider->setGeometry(QRect(50, 90, 221, 29));
        contrastMaxSlider->setOrientation(Qt::Horizontal);
        contrastMinSlider = new QSlider(groupBox_3);
        contrastMinSlider->setObjectName(QString::fromUtf8("contrastMinSlider"));
        contrastMinSlider->setGeometry(QRect(50, 40, 221, 29));
        contrastMinSlider->setOrientation(Qt::Horizontal);
        label_4 = new QLabel(groupBox_3);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 70, 91, 17));
        labelContrastMin = new QLabel(groupBox_3);
        labelContrastMin->setObjectName(QString::fromUtf8("labelContrastMin"));
        labelContrastMin->setGeometry(QRect(120, 30, 56, 13));
        labelContrastMax = new QLabel(groupBox_3);
        labelContrastMax->setObjectName(QString::fromUtf8("labelContrastMax"));
        labelContrastMax->setGeometry(QRect(120, 80, 56, 13));
        pushButton_2 = new QPushButton(centralwidget);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(210, 520, 204, 31));
        pushButton_2->setMinimumSize(QSize(121, 0));
        groupBox_4 = new QGroupBox(centralwidget);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setGeometry(QRect(160, 550, 381, 80));
        label_6 = new QLabel(groupBox_4);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(10, 20, 111, 17));
        label_7 = new QLabel(groupBox_4);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(10, 40, 121, 17));
        spinBoxNx = new QSpinBox(groupBox_4);
        spinBoxNx->setObjectName(QString::fromUtf8("spinBoxNx"));
        spinBoxNx->setGeometry(QRect(140, 10, 60, 27));
        spinBoxNx->setValue(1);
        spinBoxNy = new QSpinBox(groupBox_4);
        spinBoxNy->setObjectName(QString::fromUtf8("spinBoxNy"));
        spinBoxNy->setGeometry(QRect(140, 40, 60, 27));
        spinBoxNy->setValue(1);
        groupBox_5 = new QGroupBox(centralwidget);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        groupBox_5->setGeometry(QRect(590, 550, 120, 121));
        checkBoxRGB = new QCheckBox(groupBox_5);
        checkBoxRGB->setObjectName(QString::fromUtf8("checkBoxRGB"));
        checkBoxRGB->setGeometry(QRect(10, 30, 101, 18));
        checkBoxRGB->setChecked(true);
        checkBoxRGB->setAutoExclusive(true);
        checkBoxHSV = new QCheckBox(groupBox_5);
        checkBoxHSV->setObjectName(QString::fromUtf8("checkBoxHSV"));
        checkBoxHSV->setGeometry(QRect(10, 50, 101, 18));
        checkBoxHSV->setAutoExclusive(true);
        checkBoxSV = new QCheckBox(groupBox_5);
        checkBoxSV->setObjectName(QString::fromUtf8("checkBoxSV"));
        checkBoxSV->setGeometry(QRect(10, 70, 101, 18));
        checkBoxSV->setAutoExclusive(true);
        groupBox_6 = new QGroupBox(centralwidget);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        groupBox_6->setGeometry(QRect(740, 550, 181, 80));
        checkBoxComponent = new QCheckBox(groupBox_6);
        checkBoxComponent->setObjectName(QString::fromUtf8("checkBoxComponent"));
        checkBoxComponent->setGeometry(QRect(10, 30, 161, 18));
        checkBoxComponent->setChecked(true);
        checkBoxComponent->setAutoExclusive(true);
        checkBoxInverseComponent = new QCheckBox(groupBox_6);
        checkBoxInverseComponent->setObjectName(QString::fromUtf8("checkBoxInverseComponent"));
        checkBoxInverseComponent->setGeometry(QRect(10, 50, 101, 18));
        checkBoxInverseComponent->setAutoExclusive(true);
        progressBar = new QProgressBar(centralwidget);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setGeometry(QRect(440, 525, 141, 21));
        progressBar->setInputMethodHints(Qt::ImhNone);
        progressBar->setValue(0);
        progressBar->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
        adaptiveArea = new QPushButton(centralwidget);
        adaptiveArea->setObjectName(QString::fromUtf8("adaptiveArea"));
        adaptiveArea->setGeometry(QRect(1000, 340, 201, 32));
        adaptiveContrast = new QPushButton(centralwidget);
        adaptiveContrast->setObjectName(QString::fromUtf8("adaptiveContrast"));
        adaptiveContrast->setGeometry(QRect(1000, 410, 201, 32));
        adaptiveAreaSlider = new QSlider(centralwidget);
        adaptiveAreaSlider->setObjectName(QString::fromUtf8("adaptiveAreaSlider"));
        adaptiveAreaSlider->setGeometry(QRect(1000, 370, 221, 29));
        adaptiveAreaSlider->setMaximum(99);
        adaptiveAreaSlider->setValue(99);
        adaptiveAreaSlider->setOrientation(Qt::Horizontal);
        labelAdaptiveArea = new QLabel(centralwidget);
        labelAdaptiveArea->setObjectName(QString::fromUtf8("labelAdaptiveArea"));
        labelAdaptiveArea->setGeometry(QRect(1220, 350, 56, 13));
        adaptiveContrastSlider = new QSlider(centralwidget);
        adaptiveContrastSlider->setObjectName(QString::fromUtf8("adaptiveContrastSlider"));
        adaptiveContrastSlider->setGeometry(QRect(1000, 440, 221, 29));
        adaptiveContrastSlider->setMaximum(99);
        adaptiveContrastSlider->setValue(99);
        adaptiveContrastSlider->setOrientation(Qt::Horizontal);
        labelAdaptiveContrast = new QLabel(centralwidget);
        labelAdaptiveContrast->setObjectName(QString::fromUtf8("labelAdaptiveContrast"));
        labelAdaptiveContrast->setGeometry(QRect(1220, 410, 56, 13));
        MainWindow->setCentralWidget(centralwidget);
        groupBox_2->raise();
        layoutWidget->raise();
        groupBox->raise();
        areaMinSlider->raise();
        areaMaxSlider->raise();
        label_2->raise();
        label_3->raise();
        groupBox_3->raise();
        pushButton_2->raise();
        groupBox_4->raise();
        groupBox_5->raise();
        groupBox_6->raise();
        progressBar->raise();
        adaptiveArea->raise();
        adaptiveContrast->raise();
        adaptiveAreaSlider->raise();
        labelAdaptiveArea->raise();
        adaptiveContrastSlider->raise();
        labelAdaptiveContrast->raise();
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1345, 22));
        menuComponent_Graph = new QMenu(menubar);
        menuComponent_Graph->setObjectName(QString::fromUtf8("menuComponent_Graph"));
        menuProcessing = new QMenu(menubar);
        menuProcessing->setObjectName(QString::fromUtf8("menuProcessing"));
        menuAdd_noise = new QMenu(menubar);
        menuAdd_noise->setObjectName(QString::fromUtf8("menuAdd_noise"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuComponent_Graph->menuAction());
        menubar->addAction(menuProcessing->menuAction());
        menubar->addAction(menuAdd_noise->menuAction());
        menuComponent_Graph->addAction(actionLoad_Image);
        menuComponent_Graph->addAction(actionSynthetic_Image);
        menuComponent_Graph->addAction(actionSave_result);
        menuProcessing->addAction(actionSimplification);
        menuProcessing->addAction(actionDetection);
        menuAdd_noise->addAction(actionImpulse_noise);
        menuAdd_noise->addAction(actionGaussian_noise);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        actionLoad_Image->setText(QApplication::translate("MainWindow", "Load Image", 0  ));
        actionSimplification->setText(QApplication::translate("MainWindow", "Simplification", 0   ));
        actionDetection->setText(QApplication::translate("MainWindow", "Detection", 0   ));
        actionSynthetic_Image->setText(QApplication::translate("MainWindow", "Synthetic Image", 0   ));
        actionImpulse_noise->setText(QApplication::translate("MainWindow", "Impulse noise", 0   ));
        actionGaussian_noise->setText(QApplication::translate("MainWindow", "Gaussian noise", 0   ));
        actionSave_result->setText(QApplication::translate("MainWindow", "Save result", 0   ));
        groupBox->setTitle(QApplication::translate("MainWindow", "Select image", 0   ));
        checkBoxOriginal->setText(QApplication::translate("MainWindow", "Original", 0   ));
        checkBoxNoisy->setText(QApplication::translate("MainWindow", "Noisy", 0   ));
        checkBoxDenoised->setText(QApplication::translate("MainWindow", "Denoised", 0   ));
        label_2->setText(QApplication::translate("MainWindow", "Area min", 0   ));
        label_3->setText(QApplication::translate("MainWindow", "Area max", 0   ));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Area", 0   ));
        labelAreaMin->setText(QString());
        labelAreaMax->setText(QString());
        groupBox_3->setTitle(QApplication::translate("MainWindow", "Contrast", 0   ));
        label_5->setText(QApplication::translate("MainWindow", "Contrast min", 0   ));
        label_4->setText(QApplication::translate("MainWindow", "Contrast max", 0   ));
        labelContrastMin->setText(QString());
        labelContrastMax->setText(QString());
        pushButton_2->setText(QApplication::translate("MainWindow", "Compute graph multi-thread", 0   ));
        groupBox_4->setTitle(QApplication::translate("MainWindow", "Patchs definition", 0   ));
        label_6->setText(QApplication::translate("MainWindow", "Division in width", 0   ));
        label_7->setText(QApplication::translate("MainWindow", "Division in height", 0   ));
        groupBox_5->setTitle(QApplication::translate("MainWindow", "Color space", 0   ));
        checkBoxRGB->setText(QApplication::translate("MainWindow", "RGB space", 0   ));
        checkBoxHSV->setText(QApplication::translate("MainWindow", "HSV space", 0   ));
        checkBoxSV->setText(QApplication::translate("MainWindow", "SV space + H", 0   ));
        groupBox_6->setTitle(QApplication::translate("MainWindow", "Ordering", 0   ));
        checkBoxComponent->setText(QApplication::translate("MainWindow", "Component-wise", 0   ));
        checkBoxInverseComponent->setText(QApplication::translate("MainWindow", "Inverse", 0   ));
        adaptiveArea->setText(QApplication::translate("MainWindow", "Adaptive filtering: area", 0   ));
        adaptiveContrast->setText(QApplication::translate("MainWindow", "Adaptive filtering: contrast", 0   ));
        labelAdaptiveArea->setText(QString());
        labelAdaptiveContrast->setText(QString());
        menuComponent_Graph->setTitle(QApplication::translate("MainWindow", "Component Graph", 0   ));
        menuProcessing->setTitle(QApplication::translate("MainWindow", "Processing", 0   ));
        menuAdd_noise->setTitle(QApplication::translate("MainWindow", "Add noise", 0   ));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

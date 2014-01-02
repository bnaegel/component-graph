/********************************************************************************
** Form generated from reading UI file 'dialogNoise.ui'
**
** Created: Thu Jan 2 17:02:18 2014
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGNOISE_H
#define UI_DIALOGNOISE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QLabel *label;
    QDoubleSpinBox *doubleSpinBoxMean;
    QLabel *label_2;
    QDoubleSpinBox *doubleSpinBoxStdDev;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(217, 128);
        buttonBox = new QDialogButtonBox(Dialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(10, 90, 171, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        groupBox = new QGroupBox(Dialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 0, 171, 80));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 30, 56, 13));
        doubleSpinBoxMean = new QDoubleSpinBox(groupBox);
        doubleSpinBoxMean->setObjectName(QString::fromUtf8("doubleSpinBoxMean"));
        doubleSpinBoxMean->setGeometry(QRect(70, 20, 62, 25));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 60, 56, 13));
        doubleSpinBoxStdDev = new QDoubleSpinBox(groupBox);
        doubleSpinBoxStdDev->setObjectName(QString::fromUtf8("doubleSpinBoxStdDev"));
        doubleSpinBoxStdDev->setGeometry(QRect(70, 50, 62, 25));
        doubleSpinBoxStdDev->setValue(10);

        retranslateUi(Dialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("Dialog", "Gaussian noise parameters", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Dialog", "Mean", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Dialog", "Std-dev", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGNOISE_H

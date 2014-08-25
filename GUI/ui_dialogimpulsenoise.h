/********************************************************************************
** Form generated from reading UI file 'dialogimpulsenoise.ui'
**
** Created: Mon Aug 25 10:29:43 2014
**      by: Qt User Interface Compiler version 4.7.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGIMPULSENOISE_H
#define UI_DIALOGIMPULSENOISE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_DialogImpulseNoise
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBox;
    QLabel *label;
    QSpinBox *spinBox;

    void setupUi(QDialog *DialogImpulseNoise)
    {
        if (DialogImpulseNoise->objectName().isEmpty())
            DialogImpulseNoise->setObjectName(QString::fromUtf8("DialogImpulseNoise"));
        DialogImpulseNoise->resize(201, 120);
        buttonBox = new QDialogButtonBox(DialogImpulseNoise);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(20, 80, 171, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        groupBox = new QGroupBox(DialogImpulseNoise);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(20, 0, 161, 81));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 30, 56, 13));
        spinBox = new QSpinBox(groupBox);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setGeometry(QRect(100, 20, 53, 25));
        spinBox->setValue(15);

        retranslateUi(DialogImpulseNoise);
        QObject::connect(buttonBox, SIGNAL(accepted()), DialogImpulseNoise, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DialogImpulseNoise, SLOT(reject()));

        QMetaObject::connectSlotsByName(DialogImpulseNoise);
    } // setupUi

    void retranslateUi(QDialog *DialogImpulseNoise)
    {
        DialogImpulseNoise->setWindowTitle(QApplication::translate("DialogImpulseNoise", "Dialog", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("DialogImpulseNoise", "Impulse noise parameters", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DialogImpulseNoise", "Rate:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DialogImpulseNoise: public Ui_DialogImpulseNoise {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGIMPULSENOISE_H

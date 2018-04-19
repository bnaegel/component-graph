#include "dialogNoise.h"
#include "ui_dialogNoise.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}

double Dialog::getMean()
{
    return ui->doubleSpinBoxMean->value();

}

double Dialog::getStdDev()
{
 return ui->doubleSpinBoxStdDev->value();

}

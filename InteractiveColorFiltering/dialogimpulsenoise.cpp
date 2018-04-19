#include "dialogimpulsenoise.h"
#include "ui_dialogimpulsenoise.h"

DialogImpulseNoise::DialogImpulseNoise(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogImpulseNoise)
{
    ui->setupUi(this);
}

DialogImpulseNoise::~DialogImpulseNoise()
{
    delete ui;
}

int DialogImpulseNoise::getRate()
{
    return ui->spinBox->value();
}

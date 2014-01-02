#include "maindisplay.h"

MainDisplay::MainDisplay(QWidget *parent):QWidget(parent)
{
    this->zoomFactor=1.0;
}

void MainDisplay::paintEvent(QPaintEvent *event)
{
    if(imageIsSet) {
        QPainter painter(this);
        painter.scale(zoomFactor,zoomFactor);
        painter.drawImage(0,0,im);
    }

}

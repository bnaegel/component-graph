#ifndef DISPLAY_H
#define DISPLAY_H

#include <QScrollArea>
#include <QPainter>

class MainDisplay : public QWidget
{
public:
    MainDisplay(QWidget *parent = 0);
    void setImage(QImage im) {this->im=im;
        this->resize((int)im.width()*zoomFactor,(int)im.height()*zoomFactor);
        imageIsSet=true;
        this->update();
        }
    void removeImage() {imageIsSet=false;}

    void zoomIn() {zoomFactor*=1.25; this->resize((int)im.width()*zoomFactor,(int)im.height()*zoomFactor); update();}
    void zoomOut() {zoomFactor*=0.8; this->resize((int)im.width()*zoomFactor,(int)im.height()*zoomFactor); update();}
protected:
    void paintEvent(QPaintEvent *);
private:
    QImage im;
    bool imageIsSet;
    double zoomFactor;

};

#endif // DISPLAY_H

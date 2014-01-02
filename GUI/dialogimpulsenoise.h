#ifndef DIALOGIMPULSENOISE_H
#define DIALOGIMPULSENOISE_H

#include <QDialog>

namespace Ui {
class DialogImpulseNoise;
}

class DialogImpulseNoise : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogImpulseNoise(QWidget *parent = 0);
    ~DialogImpulseNoise();
    int getRate();
    
private:
    Ui::DialogImpulseNoise *ui;
};

#endif // DIALOGIMPULSENOISE_H

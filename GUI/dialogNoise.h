#ifndef DIALOGNOISE_H
#define DIALOGNOISE_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    double getMean();
    double getStdDev();

private:
    Ui::Dialog *ui;
};

#endif // DIALOGNOISE_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMainWindow>
#include <QFileDialog>
#include <QPainter>
#include <QLabel>
#include <QFuture>
#include <QtConcurrentRun>
#include <QFutureWatcher>
#include <QTime>
#include <QObject>


#include "corefilter.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    

    void generateGaussianNoise(double mean, double stddev);
    void generateImpulseNoise(int rate);




protected :   void keyPressEvent(QKeyEvent *event);
private slots:
    //void on_pushButton_clicked();

    void on_actionLoad_Image_triggered();
    void on_actionSynthetic_Image_triggered();

    void onStateChanged(int s);
    void onProgressValueChanged(int value);
    void onGraphComputed();
    void onFinished();

    //void on_areaMinSlider_valueChanged(int value);
    //void on_areaMaxSlider_valueChanged(int value);

    void on_pushButton_2_clicked();

    void on_actionImpulse_noise_triggered();

    void on_actionGaussian_noise_triggered();


     void onFilteringValueChanged(int value);

    void on_checkBoxComponent_stateChanged(int arg1);

    void on_checkBoxHSV_stateChanged(int arg1);

    void on_actionSave_result_triggered();

    void onAdaptiveValueChanged(int value);

private:
    Ui::MainWindow *ui;


    void loadImage(QString &filename);
    QImage *convertLibTIMToQImage(Image<U8> &img);
    QImage *convertLibTIMToQImage(Image<RGB> &img);

    Image<RGB> convertQImageToLibTIM(QImage *img);
    CoreFilter *coreFilter;

    QImage qtImage;
    QImage qtNoisy;
    QImage qtResult;
    bool imageIsLoaded;
    bool resultComputed;
    QLabel qLabelSource;
    QLabel qLabelResult;

    QFutureWatcher<int> watcher;
    QFutureWatcher<int> watcherFilter;

    clock_t timeElapsed;
    QTime realTimeElapsed;

};

#endif // MAINWINDOW_H

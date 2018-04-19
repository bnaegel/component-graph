#include <QKeyEvent>

#include "noise.h"
#include "dialogimpulsenoise.h"
#include "ui_dialogimpulsenoise.h"
#include "dialogNoise.h"
#include "ui_dialogNoise.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),coreFilter(0)
{
    ui->setupUi(this);

    ui->statusbar->setSizeGripEnabled(false);
    connect(&watcher, SIGNAL(finished()), this, SLOT(onGraphComputed()));
    connect(&watcherFilter, SIGNAL(finished()), this, SLOT(onFinished()));

    connect(ui->checkBoxOriginal,SIGNAL(stateChanged(int)),this, SLOT(onStateChanged(int)));
    connect(ui->checkBoxNoisy,SIGNAL(stateChanged(int)),this, SLOT(onStateChanged(int)));
    connect(ui->checkBoxDenoised,SIGNAL(stateChanged(int)),this, SLOT(onStateChanged(int)));

    connect(ui->checkBoxRGB,SIGNAL(stateChanged(int)),this, SLOT(on_checkBoxHSV_stateChanged(int)));
    connect(ui->checkBoxHSV,SIGNAL(stateChanged(int)),this, SLOT(on_checkBoxHSV_stateChanged(int)));
    connect(ui->checkBoxSV,SIGNAL(stateChanged(int)),this, SLOT(on_checkBoxHSV_stateChanged(int)));

    connect(ui->checkBoxComponent,SIGNAL(stateChanged(int)),this, SLOT(onStateChanged(int)));
    connect(ui->checkBoxInverseComponent,SIGNAL(stateChanged(int)),this, SLOT(onStateChanged(int)));

    connect(ui->areaMaxSlider,SIGNAL(valueChanged(int)),this, SLOT(onFilteringValueChanged(int)));
    connect(ui->areaMinSlider,SIGNAL(valueChanged(int)),this, SLOT(onFilteringValueChanged(int)));

    connect(ui->contrastMaxSlider,SIGNAL(valueChanged(int)),this, SLOT(onFilteringValueChanged(int)));
    connect(ui->contrastMinSlider,SIGNAL(valueChanged(int)),this, SLOT(onFilteringValueChanged(int)));

    connect(ui->adaptiveAreaSlider,SIGNAL(valueChanged(int)),this, SLOT(onAdaptiveValueChanged(int)));
    connect(ui->adaptiveContrastSlider,SIGNAL(valueChanged(int)),this, SLOT(onAdaptiveValueChanged(int)));

    imageIsLoaded=false;
    resultComputed=false;

}

MainWindow::~MainWindow()
{
    delete ui;
    delete coreFilter;
}


QImage *MainWindow::convertLibTIMToQImage(Image<U8> &img)
{
    int dx=img.getSizeX();
    int dy=img.getSizeY();

    QImage *resImg=new QImage(dx, dy,QImage::Format_ARGB32);

    for(int y=0; y<dy; y++)
        for(int x=0; x<dx; x++)
        {
        U8 value=img(x,y);
            resImg->setPixel(x,y,qRgb(value,value,value));

        }
    return resImg;
}



QImage *MainWindow::convertLibTIMToQImage(Image<RGB> &img)
{
    int dx=img.getSizeX();
    int dy=img.getSizeY();

    QImage *resImg=new QImage(dx, dy,QImage::Format_ARGB32);

    for(int y=0; y<dy; y++)
        for(int x=0; x<dx; x++)
        {
        RGB value=img(x,y);
            resImg->setPixel(x,y,qRgb(value[0],value[1],value[2]));

        }
    return resImg;
}



Image<RGB> MainWindow::convertQImageToLibTIM(QImage *img)
{
    int dx=img->width();
    int dy=img->height();

    Image<RGB> resImg(dx, dy);

    for(int y=0; y<dy; y++)
        for(int x=0; x<dx; x++)
        {
        QRgb value=img->pixel(x,y);
        resImg(x,y)[0]=qRed(value);
        resImg(x,y)[1]=qGreen(value);
        resImg(x,y)[2]=qBlue(value);
        }
    return resImg;
}

void MainWindow::loadImage(QString &filename)
{

    if(qtImage.load(filename))
        {
        //qtImage=qtImage.convertToFormat(QImage::Format_ARGB32);
        Image<RGB> imSource=convertQImageToLibTIM(&qtImage);
        qtImage=*convertLibTIMToQImage(imSource);

        this->qtNoisy=qtImage;
        this->qtResult=qtImage;

        imageIsLoaded=true;
        resultComputed=false;

        int dx=imSource.getSizeX();
        int dy=imSource.getSizeY();

        ui->areaMinSlider->setMaximum(dx*dy+100);
        ui->areaMaxSlider->setMaximum(dx*dy+1);
        ui->areaMaxSlider->setValue(dx*dy);

        ui->contrastMinSlider->setMaximum(255*3);
        ui->contrastMaxSlider->setMaximum(255*3);
        ui->contrastMaxSlider->setValue(255*3);

        if(coreFilter!=0) delete coreFilter;

        this->coreFilter=new CoreFilter(imSource);
        connect(coreFilter->cgraphWatcher,SIGNAL(progress(int)),this,SLOT(onProgressValueChanged(int)));

        ui->scrollArea->setWidgetResizable(false);
        ui->mainDisplay->setImage(qtImage);
        ui->scrollArea->resize(dx,dy);

        ui->mainDisplay2->removeImage();
        ui->actionSimplification->setEnabled(true);
        ui->actionDetection->setEnabled(true);

        QString s("Image size ");
        s += QString::number(imSource.getSizeX());
        s+=",";
        s += QString::number(imSource.getSizeY());
        this->qLabelSource.setText(s);
        ui->statusbar->addWidget(&qLabelSource);

        ui->progressBar->hide();

        }

}


void MainWindow::keyPressEvent ( QKeyEvent * event  )
{
    //qDebug() <<"Key pressed: " << event->key() << "\n";
    switch(event->key())
        {
        case Qt::Key_Plus:
            this->ui->mainDisplay->zoomIn();
            this->ui->mainDisplay2->zoomIn();
            break;
        case Qt::Key_Minus:
            this->ui->mainDisplay->zoomOut();
            this->ui->mainDisplay2->zoomOut();
            break;
        default:
            QWidget::keyPressEvent(event);
        }
}

void MainWindow::on_actionLoad_Image_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
    tr("Open Image"), ".", tr("Image Files (*.*)"));
    if(!fileName.isEmpty())
        {
        this->loadImage(fileName);
        QDir d = QFileInfo(fileName).absoluteDir();
        if(QDir::setCurrent(d.absolutePath()));
        }

}

void MainWindow::on_actionSynthetic_Image_triggered()
{

}


void MainWindow::onGraphComputed()
{
    if(watcher.result()) {
     timeElapsed=clock()-timeElapsed;

    // ui->checkBoxDenoised->setChecked(true);
     ui->progressBar->hide();
     this->qLabelSource.setText(QString("CPU time: ") + QString::number((double)timeElapsed/CLOCKS_PER_SEC) + " s "+
                                QString(" Real time: ") +QString::number((double)realTimeElapsed.elapsed()/1000)+ " s" );

     //ui->labelMSE->setText(QString::number(algo->computeMSE()));
     resultComputed=true;

    }
}


void MainWindow::onFinished()
{
    if(watcher.result()) {
     timeElapsed=clock()-timeElapsed;

//      // for the moment : patch to compute image in HSV space
//        if(ui->checkBoxHSV->isChecked())  {
//        coreFilter->imResult=ColorUtils::convertHSVtoRGB(coreFilter->imResult);

//        }
//        if(ui->checkBoxSV->isChecked()) {
//            Image <RGB> imHSV=ColorUtils::mergeHwithSV(coreFilter->imHSV,coreFilter->imResult);
//            coreFilter->imResult=ColorUtils::convertHSVtoRGB(coreFilter->imResult);
//        }

     qtResult=*this->convertLibTIMToQImage(coreFilter->imResult);
     ui->checkBoxDenoised->setChecked(true);
     ui->progressBar->hide();
     this->qLabelSource.setText(QString("CPU time: ") + QString::number((double)timeElapsed/CLOCKS_PER_SEC) + " s "+
                                QString(" Real time: ") +QString::number((double)realTimeElapsed.elapsed()/1000)+ " s" );

     //ui->labelMSE->setText(QString::number(algo->computeMSE()));
     resultComputed=true;
    }
}



void MainWindow::onProgressValueChanged(int value)
{

    ui->progressBar->setValue(value);
}


//void MainWindow::on_pushButton_clicked()
//{
//    if(imageIsLoaded) {
//        this->qLabelSource.setText("Computing component-graph" );

//        QFuture<int> future = QtConcurrent::run(this->coreFilter->algo, &CGraph::computeGraphInverse, coreFilter->cgraphWatcher);

//        watcher.setFuture(future);
//        timeElapsed=clock();
//        if(realTimeElapsed.isNull())
//            realTimeElapsed.start();
//        else realTimeElapsed.restart();

//        ui->progressBar->show();
//    }
//}


void MainWindow::onStateChanged(int s)
{
    if(ui->checkBoxOriginal->isChecked()) {
        ui->mainDisplay2->setImage(qtImage);
    }

    if(ui->checkBoxNoisy->isChecked()) {
        ui->mainDisplay2->setImage(qtNoisy);
    }

    if(ui->checkBoxDenoised->isChecked()) {
        ui->mainDisplay2->setImage(qtResult);
    }
    if(ui->checkBoxRGB->isChecked()) {
        this->coreFilter->setColorSpaceRGB();
    }
    if(ui->checkBoxDenoised->isChecked()) {
        this->coreFilter->setColorSpaceHSV();
    }
}



void MainWindow::onFilteringValueChanged(int value)
{
    int areaMin=ui->areaMinSlider->value();
    int areaMax=ui->areaMaxSlider->value();

    int contrastMin=ui->contrastMinSlider->value();
    int contrastMax=ui->contrastMaxSlider->value();


    ui->labelAreaMin->setText(QString::number(areaMin));
    ui->labelAreaMax->setText(QString::number(areaMax));

    ui->labelContrastMin->setText(QString::number(contrastMin));
    ui->labelContrastMax->setText(QString::number(contrastMax));


    //qDebug()<< "Value changed : " << value << " " << min << "\n";
    if(resultComputed) {
        // QFuture<int> future = QtConcurrent::run(this->coreFilter, &CoreFilter::areaFiltering, min, 0);
        // watcherFilter.setFuture(future);
        coreFilter->resetFiltering();
        coreFilter->areaFiltering(areaMin,areaMax);
        coreFilter->contrastFiltering(contrastMin,contrastMax);
        if(ui->checkBoxHSV->isChecked()) {
            coreFilter->imResult=ColorUtils::convertHSVtoRGB(coreFilter->imResult);
        }

        if(ui->checkBoxSV->isChecked()) {
            //qDebug() << coreFilter->imHSV.getSizeX() << " "<< coreFilter->imHSV.getSizeY() << "\n";
            Image <RGB> imHSV=ColorUtils::mergeHwithSV(coreFilter->imHSV,coreFilter->imResult);
            coreFilter->imResult=ColorUtils::convertHSVtoRGB(imHSV);
             //coreFilter->imResult=imHSV;
        }
        qtResult=*this->convertLibTIMToQImage(coreFilter->imResult);
        ui->mainDisplay2->setImage(qtResult);
    }

}


void MainWindow::on_pushButton_2_clicked()
{
     if(imageIsLoaded) {
        int nx=ui->spinBoxNx->value();
        int ny=ui->spinBoxNy->value();
        this->qLabelSource.setText("Computing component-graph using " + QString::number(nx*ny+(nx-1)*(ny-1)) + " threads" );

//        // for the moment : patch to compute image in HSV space
//        if(ui->checkBoxHSV->isChecked())  {
//            coreFilter->imSource=ColorUtils::convertRGBtoHSV(coreFilter->imSource);

//        }

        QFuture<int> future = QtConcurrent::run(this->coreFilter, &CoreFilter::computeGraphMultiThread,nx,ny);

        watcher.setFuture(future);
        timeElapsed=clock();
        if(realTimeElapsed.isNull())
            realTimeElapsed.start();
        else realTimeElapsed.restart();

        ui->progressBar->show();

        ui->areaMinSlider->setMaximum((coreFilter->imSource.getSizeX()/nx)*(coreFilter->imSource.getSizeY()/ny)+10);
    }
}


void MainWindow::on_actionImpulse_noise_triggered()
{
    if(imageIsLoaded) {
    DialogImpulseNoise *dialog=new DialogImpulseNoise();
    dialog->exec();
    if(dialog->result()==QDialog::Accepted) {
    int rate=dialog->getRate();
    generateImpulseNoise(rate);
    ui->checkBoxNoisy->setChecked(true);
    //ui->labelMSE->setText(QString::number(coreFilter->computeMSE()));
    }
    delete dialog;
    }
}

void MainWindow::on_actionGaussian_noise_triggered()
{
    if(imageIsLoaded) {
    Dialog *dialog=new Dialog();
    dialog->exec();
    if(dialog->result()==QDialog::Accepted) {
    double mean=dialog->getMean();
    double stddev=dialog->getStdDev();
    generateGaussianNoise(mean,stddev);
    ui->checkBoxNoisy->setChecked(true);
    //ui->labelMSE->setText(QString::number(coreFilter->computeMSE()));
    }
    delete dialog;
    }
}

void MainWindow::generateGaussianNoise(double mean, double stddev)
{
    coreFilter->imNoisy=Noise::addGaussianNoise(coreFilter->imSource,mean,stddev);
    this->qtNoisy=*convertLibTIMToQImage(coreFilter->imNoisy);
}

void MainWindow::generateImpulseNoise(int rate)
{
    coreFilter->imNoisy=Noise::addImpulseNoise(coreFilter->imSource,rate);
    this->qtNoisy=*convertLibTIMToQImage(coreFilter->imNoisy);
}


void MainWindow::on_checkBoxComponent_stateChanged(int arg1)
{
    if(ui->checkBoxComponent->isChecked()) {
        if(coreFilter->order!=0) delete coreFilter->order;
        coreFilter->order=new ColorMarginalOrdering();
    }

    if(ui->checkBoxInverseComponent->isChecked()) {
        if(coreFilter->order!=0) delete coreFilter->order;
        coreFilter->order=new ColorMarginalInverseOrdering();
    }
    resultComputed=false;
}

void MainWindow::on_checkBoxHSV_stateChanged(int arg1)
{
    if(ui->checkBoxRGB->isChecked()) {
        if(coreFilter!=0) {
            Image<RGB> imSource=convertQImageToLibTIM(&qtImage);
            delete coreFilter;
            this->coreFilter=new CoreFilter(imSource);
            connect(coreFilter->cgraphWatcher,SIGNAL(progress(int)),this,SLOT(onProgressValueChanged(int)));

        }
        resultComputed=false;

    }

    if(ui->checkBoxHSV->isChecked()) {
        if(coreFilter!=0) {
            Image<RGB> imSource=convertQImageToLibTIM(&qtImage);
            Image <RGB> imHSV=ColorUtils::convertRGBtoHSV(imSource);
            delete coreFilter;
            this->coreFilter=new CoreFilter(imHSV);
            connect(coreFilter->cgraphWatcher,SIGNAL(progress(int)),this,SLOT(onProgressValueChanged(int)));

        }
         resultComputed=false;
    }

    if(ui->checkBoxSV->isChecked()) {
        if(coreFilter!=0) {
            Image<RGB> imSource=convertQImageToLibTIM(&qtImage);
            Image <RGB> imHSV=ColorUtils::convertRGBtoHSV(imSource);
            Image <RGB> imSV=ColorUtils::convertHSVtoSV(imHSV);
            delete coreFilter;
            this->coreFilter=new CoreFilter(imSV);
            this->coreFilter->imHSV=imHSV;
            connect(coreFilter->cgraphWatcher,SIGNAL(progress(int)),this,SLOT(onProgressValueChanged(int)));

            qDebug() << "Init coreFilter HSV : " << this->coreFilter->imHSV.getSizeX() << " " << this->coreFilter->imHSV.getSizeY()  << " \n";
        }

     resultComputed=false;
    }

    qDebug() << "Check box activated \n";
}

void MainWindow::on_actionSave_result_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
    tr("QFileDialog::getSaveFileName()"), ".",
    tr("Image Files (*.png *.ppm *.jpg)"));
    if(!fileName.isEmpty())
        this->qtResult.save(fileName);
}

void MainWindow::onAdaptiveValueChanged(int value)
{
    if(resultComputed) {
        // QFuture<int> future = QtConcurrent::run(this->coreFilter, &CoreFilter::areaFiltering, min, 0);
        // watcherFilter.setFuture(future);

        int p=ui->adaptiveAreaSlider->value();
        int q=ui->adaptiveContrastSlider->value();
        ui->labelAdaptiveArea->setText(QString::number(p));
        ui->labelAdaptiveContrast->setText(QString::number(q));

        coreFilter->resetFiltering();
        coreFilter->adaptiveAreaFiltering(p);
        coreFilter->adaptiveContrastFiltering(q);
        if(ui->checkBoxHSV->isChecked()) {
            coreFilter->imResult=ColorUtils::convertHSVtoRGB(coreFilter->imResult);
        }

        if(ui->checkBoxSV->isChecked()) {
            //qDebug() << coreFilter->imHSV.getSizeX() << " "<< coreFilter->imHSV.getSizeY() << "\n";
            Image <RGB> imHSV=ColorUtils::mergeHwithSV(coreFilter->imHSV,coreFilter->imResult);
            coreFilter->imResult=ColorUtils::convertHSVtoRGB(imHSV);
             //coreFilter->imResult=imHSV;
        }
        qtResult=*this->convertLibTIMToQImage(coreFilter->imResult);
        ui->mainDisplay2->setImage(qtResult);
    }

}

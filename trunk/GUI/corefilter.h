#ifndef COREFILTER_H
#define COREFILTER_H

#include <QObject>
#include <QtConcurrentRun>
#include <QFuture>
#include "Image.h"
#include "cgraph.h"


class ColorUtils {

public:
    static Image<RGB> convertHSVtoRGB(Image <RGB> &imSource);
    static Image<RGB> convertRGBtoHSV(Image <RGB> &imSource);
    static Image<RGB> mergeHwithSV(Image<RGB> &imH, Image<RGB> &imSV);
    static Image<RGB> convertHSVtoSV(Image <RGB> &imSource);
};


class CGraphQtWatcher :  public QObject, public CGraphWatcher {
    Q_OBJECT

    signals:
        void progress(int);

    public :
        void progressUpdate(int ncur, int nfinal) {
            int progressValue=(ncur*100)/nfinal;
            emit progress(progressValue);
        }


        void progressUpdate() {
            curProgress++;
            int progressValue=(curProgress*100)/finalProgress;
            emit progress(progressValue);
        }

    void reset() {curProgress=0;}
    CGraphQtWatcher(int finalProgress) : CGraphWatcher(finalProgress) {
        }
};


class CoreFilter : public QObject
{
    Q_OBJECT
public:
    explicit CoreFilter(QObject *parent = 0);
    CoreFilter(Image <RGB> &imSource);

    void setColorSpaceRGB() {}
    void setColorSpaceHSV() {

        }

    int areaFiltering(int min ,int max);
    int contrastFiltering(int min, int max);
    int resetFiltering();

    int adaptiveAreaFiltering(int p);

    int computeGraphMultiThread( int nx, int ny);

    CGraphQtWatcher *cgraphWatcher;
    CGraph *algo;
    CColorOrdering *order;

    Image<RGB> imSource;
    Image<RGB> imHSV;

    Image<RGB> imNoisy;
    Image<RGB> imResult;

    void imageToCoveringPatches(Image<RGB> &imSource, int nx, int ny);
    Image<RGB> coveringPatchesToImage();
    int adaptiveContrastFiltering(int p);
signals:
    
public slots:

private:
    class Patch {
        public :
        /*<!Size of patch*/
        int dx,dy;
         /*<!Top-left coordinates*/
        int rx,ry;
        /*<!Patch image*/
        Image<RGB> im;
        Image<RGB> imResult;

        Patch(Image <RGB> &imSource, int rx, int ry, int dx, int dy) {
            this->dx=dx;
            this->dy=dy;
            this->rx=rx;
            this->ry=ry;
            this->im=imSource.crop(rx,rx+dx,ry,ry+dy);
            this->imResult=imSource.crop(rx,rx+dx,ry,ry+dy);
        }

    };

    void imageToPatch(Image <RGB> &imSource, int nx, int ny);
    Image <RGB> patchsToImage();

    vector <CGraph *> cgraphs;
    vector<Patch *> patchs;


    
};

#endif // COREFILTER_H

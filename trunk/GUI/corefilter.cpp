#include "corefilter.h"

CoreFilter::CoreFilter(QObject *parent) :
    QObject(parent)
{
}

CoreFilter::CoreFilter(Image<RGB> &imSource):algo(0),cgraphWatcher(0),order(0)
{
    this->imSource=imSource;
    this->imNoisy=imSource;
    this->imResult=imSource;

    FlatSE connexity;
    connexity.make2DN8();
    algo=new CGraph(imSource,connexity);

    if(cgraphWatcher!=0) delete cgraphWatcher;
    cgraphWatcher=new CGraphQtWatcher(imSource.getBufSize());
}


int CoreFilter::areaFiltering(int min, int max)
{
    if(cgraphs.size()==0) {

    algo->areaFiltering(min,max);
    //algo->reconstructMin();
    assert(1==0);

    imResult=algo->constructImage(order);
    }
    else {
        for(int i=0; i< patchs.size(); i++) {
            cgraphs[i]->areaFiltering(min,max);

            patchs[i]->imResult=cgraphs[i]->constructImage(order);
        }
    //imResult=patchsToImage();
    imResult=coveringPatchesToImage();
    }
    return 1;
}


int CoreFilter::contrastFiltering(int min, int max)
{
    if(cgraphs.size()==0) {

    algo->contrastFiltering(min,max);
    //algo->reconstructMin();
    assert(1==0);

    imResult=algo->constructImage(order);
    }
    else {
        for(int i=0; i< patchs.size(); i++) {
            cgraphs[i]->contrastFiltering(min,max);

            patchs[i]->imResult=cgraphs[i]->constructImage(order);
        }
    //imResult=patchsToImage();
    imResult=coveringPatchesToImage();
    }
    return 1;
}

int CoreFilter::resetFiltering()
{
    for(int i=0; i< patchs.size(); i++)
        cgraphs[i]->resetFiltering();

}

int CoreFilter::adaptiveAreaFiltering(int p)
{
    for(int i=0; i< patchs.size(); i++) {
            cgraphs[i]->adaptiveAreaFiltering(p);
            patchs[i]->imResult=cgraphs[i]->constructImage(order);
        }
    imResult=coveringPatchesToImage();

    return 1;
}


int CoreFilter::adaptiveContrastFiltering(int p)
{
    for(int i=0; i< patchs.size(); i++) {
            cgraphs[i]->adaptiveContrastFiltering(p);
            patchs[i]->imResult=cgraphs[i]->constructImage(order);
        }
    imResult=coveringPatchesToImage();

    return 1;
}




/** Multi-thread computation of component-graph
*   @param nthread : number of threads
*   @param nx : number of cuts in dx
*   @param ny : number of cuts in dy
**/
int CoreFilter::computeGraphMultiThread(int nx, int ny)
{
    vector <QFuture<int> > futures;


    cgraphWatcher->reset();
    if(order==0)
        order=new CColorMarginalOrdering();

    //imageToPatch(imNoisy,nx,ny);

    imageToCoveringPatches(imNoisy,nx,ny);

    FlatSE connexity;
    connexity.make2DN8();

    for(int i=0; i<this->patchs.size(); i++) {
        CGraph *cgraph=new CGraph(patchs[i]->im,connexity);
        cgraphs.push_back(cgraph);
        QFuture<int> future = QtConcurrent::run(cgraphs.back(), &CGraph::computeGraph,this->order,this->cgraphWatcher);
        futures.push_back(future);
    }

    for(int i=0; i<futures.size(); i++) {
        futures[i].waitForFinished();
    }


    return 1;
}

void CoreFilter::imageToPatch(Image<RGB> &imSource, int nx, int ny)
{

    int dx=imSource.getSizeX()/nx;
    int dy=imSource.getSizeY()/ny;

    for(int i=0; i<cgraphs.size(); i++) {
        delete cgraphs[i];
        delete patchs[i];
    }
    cgraphs.clear();
    patchs.clear();

    for(int y=0; y<ny; y++) {
        for(int x=0; x<nx; x++) {
            int rx=x*dx;
            int ry=y*dy;
            Patch *tmp=new Patch(imSource,rx,ry,dx,dy);
            this->patchs.push_back(tmp);
            //qDebug() << "Patch " << patchs.size() - 1  << " " << "(" << rx<<","<<ry<<")   Size : " << dx << " " << dy << "\n";
        }
    }

}


void CoreFilter::imageToCoveringPatches(Image<RGB> &imSource, int nx, int ny)
{

    int dx=imSource.getSizeX()/nx;
    int dy=imSource.getSizeY()/ny;

    //for now : covering of 50% between patches


    for(int i=0; i<cgraphs.size(); i++) {
        delete cgraphs[i];
        delete patchs[i];
    }
    cgraphs.clear();
    patchs.clear();

    for(int y=0; y<ny; y++) {
        for(int x=0; x<nx; x++) {
            int rx=x*dx;
            int ry=y*dy;
            Patch *tmp=new Patch(imSource,rx,ry,dx,dy);

            this->patchs.push_back(tmp);
            //qDebug() << "Patch " << patchs.size() - 1  << " " << "(" << rx<<","<<ry<<")   Size : " << dx << " " << dy << "\n";
        }
    }
    //décalage de (dx/2,dy/2)
    for(int y=0; y<ny-1; y++) {
        for(int x=0; x<nx-1; x++) {
            int rx=x*dx+dx/2;
            int ry=y*dy+dy/2;
            Patch *tmp=new Patch(imSource,rx,ry,dx,dy);

            this->patchs.push_back(tmp);
            //qDebug() << "Patch " << patchs.size() - 1  << " " << "(" << rx<<","<<ry<<")   Size : " << dx << " " << dy << "\n";
        }
    }

}

Image<RGB> CoreFilter::coveringPatchesToImage()
{
    Image<RGB> imResult=imSource;
    imResult.fill(0);
    for(int i=0; i< patchs.size(); i++) {
        Patch *p=patchs[i];
        for(int y=0; y<p->dy; y++) {
            for(int x=0;x<p->dx; x++) {
                U8 R,G,B;
//            U8 R=std::max(imResult(x+p->rx,y+p->ry)[0],p->imResult(x,y)[0]);
//            U8 G=std::max(imResult(x+p->rx,y+p->ry)[1],p->imResult(x,y)[1]);
//            U8 B=std::max(imResult(x+p->rx,y+p->ry)[2],p->imResult(x,y)[2]);

//            U8 R=(imResult(x+p->rx,y+p->ry)[0]+p->imResult(x,y)[0])/2;
//            U8 G=(imResult(x+p->rx,y+p->ry)[1]+p->imResult(x,y)[1])/2;
//            U8 B=(imResult(x+p->rx,y+p->ry)[2]+p->imResult(x,y)[2])/2;
                if(imResult(x+p->rx,y+p->ry)[0]!=0 && imResult(x+p->rx,y+p->ry)[1]!=0 &&imResult(x+p->rx,y+p->ry)[2]!=0 ) {

                 R=(imResult(x+p->rx,y+p->ry)[0]+p->imResult(x,y)[0])/2;
                 G=(imResult(x+p->rx,y+p->ry)[1]+p->imResult(x,y)[1])/2;
                 B=(imResult(x+p->rx,y+p->ry)[2]+p->imResult(x,y)[2])/2;
                }
                else {
                 R=p->imResult(x,y)[0];
                 G=p->imResult(x,y)[1];
                 B=p->imResult(x,y)[2];
                }


                RGB value(R,G,B);
                imResult(x+p->rx,y+p->ry)=value;
            }
        }
    }
    return imResult;
}

Image<RGB> ColorUtils::convertRGBtoHSV(Image<RGB> &imSource)
{
    Image <RGB> imResult(imSource.getSize());

    for(int i=0; i<imSource.getBufSize(); i++) {
        double R=imSource(i)[0]/255.0;
        double G=imSource(i)[1]/255.0;
        double B=imSource(i)[2]/255.0;
        double M=std::max(std::max(R,G),B);
        double m=std::min(std::min(R,G),B);
        double C=M-m;

        double H,S,V;

        V=M;

        if(C==0 ) S=0;
        else S=C/V;

        if(C==0) H=6; //special value...
        else if(R==M) {
            H=(std::fmod((G-B)/C,6));
        }
        else if(G==M) {
            H=(2+(B-R)/C);
        }
        else if(B==M) {
            H=(4+(R-G)/C);

        }

        H=H*60; // 0<=H<=360

        U8 HH=(H*255.0/359.0);
        U8 SS=S*255;
        U8 VV=V*255;

        RGB value(HH,SS,VV);
        imResult(i)=value;
    }

    return imResult;
}

Image<RGB> ColorUtils::mergeHwithSV(Image<RGB> &imH, Image<RGB> &imSV)
{
    Image<RGB> imResult(imH.getSize());

    for(int i=0; i<imH.getBufSize(); i++) {

//        if(imSV(i)[1]==0 && imSV(i)[2]==0)  {

//            RGB value(imH(i)[0],0,imH(i)[2]);
//            imResult(i)=value;
//        }
//        else {
//            RGB value(imH(i)[0],imSV(i)[1],imSV(i)[2]);
//            imResult(i)=value;
//        }

        RGB value(imH(i)[0],imSV(i)[1],imSV(i)[2]);
            imResult(i)=value;
    }

   // qDebug() << imResult.getSizeX() << " "  <<  imResult.getSizeY();
    return imResult;
}

Image<RGB> ColorUtils::convertHSVtoSV(Image<RGB> &imSource)
{
    Image<RGB> imResult(imSource.getSize());

    for(int i=0; i<imSource.getBufSize(); i++) {
        RGB value(0,imSource(i)[1],imSource(i)[2]);
        imResult(i)=value;
    }

    return imResult;
}

Image<RGB> ColorUtils::convertHSVtoRGB(Image<RGB> &imSource)
{
    Image <RGB> imResult(imSource.getSize());

    for(int i=0; i<imSource.getBufSize(); i++) {
        U8 HH=imSource(i)[0];
        U8 SS=imSource(i)[1];
        U8 VV=imSource(i)[2];

        double H,S,V,C,X,R,G,B,m;

        H=(HH*359.0)/255.0; //0<H<=360
        //if(H==360) H=0;


        H=H/60; // 0<H<=6
        S=SS/255.0;
        V=VV/255.0;

        //std::cout << "H= " << H << " S=" << S << " V= " << V << "\n";

        C=V*S;

        X=C*(1-std::fabs(std::fmod(H,2)-1));

        if(H==6) {
        R=0; G=0; B=0;
        std::cout << "Special value\n";
        }
        else if(H>=0 && H<1) {
            R=C; G=X; B=0;
        }
        else if(H>=1 && H<2) {
            R=X; G=C; B=0;
        }
        else if(H>=2 && H<3) {
            R=0; G=C; B=X;
        }
        else if(H>=3 && H<4) {
            R=0; G=X; B=C;
        }
        else if(H>=4 && H<5) {
            R=X; G=0; B=C;
        }
        else if(H>=5 && H<6) {
            R=C; G=0; B=X;
        }

        m=V-C;
        R+=m; G+=m; B+=m;



        RGB value((U8)(R*255),(U8)(G*255),(U8)(B*255));
        //std::cout << "R= " << (int)value[0] << " G=" << (int)value[1] << " B= " << (int)value[2] << "\n";
        imResult(i)=value;
    }

    return imResult;
}


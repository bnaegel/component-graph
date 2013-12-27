//Copyright (C) 2013, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.


#include <ctime>

#include "cgraph.h"

using namespace std;

// for LibTIM classes
using namespace LibTIM;

class graphWatcher : public CGraphWatcher{
    public :
        void progressUpdate(int ncur, int nfinal) {
            if(ncur%100==0) {
                std::cout << ncur << " / " << nfinal << "\n";
            }
        }

        void progressUpdate() {
            curProgress++;
            if(curProgress%100==0) {
                std::cout << this->curProgress << " / " << this->finalProgress << "\n";
            }
        }
    graphWatcher(int finalProgress) : CGraphWatcher(finalProgress) {}
};


Image<RGB> convertRGBtoHSV(Image<RGB> &imSource)
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

Image<RGB> convertHSVtoRGB(Image<RGB> &imSource)
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


int main(int argc, char *argv[])
{
    if(argc<2)
    {
        cout<<"Usage: " << argv[0] << " <source_image.ppm> <area_min>\n";
        exit(1);
    }

    // Declaration of:
    // - imSrc (source image)

    Image <RGB> imSrc,imHSV,imOut;

    // Load source image imSrc, 2D RGB (8 bits)
    Image<RGB>::load(argv[1],imSrc);

    imHSV=convertRGBtoHSV(imSrc);
    imHSV.save("hsv.ppm");
    imOut=convertHSVtoRGB(imHSV);
    imOut.save("rgb.ppm");


//    int areaMin=atoi(argv[2]);

//    FlatSE connexity;
//    connexity.make2DN8();

//    Image <RGB> imTest=CGraph::syntheticImage2();

//    //CGraphSpace::Core *algo=new CGraphSpace::Core(imTest);

//    /** New algorithm
//    */

//    //CGraph *cgraph=new CGraph(imTest,connexity);
//    CGraph *cgraph=new CGraph(imSrc,connexity);
//    //CGraphSpace::Core *algo=new CGraphSpace::Core(imSrc);
//    graphWatcher *myWatcher=new graphWatcher(imSrc.getBufSize());
//    cgraph->computeGraph(myWatcher);
//    cgraph->writeDot("cgraph.dot");

//    std::cout << "Fast algorithm for component graph\n";
//    std::cout << "\nNumber of nodes of CGraph: " <<     cgraph->graph.size() << "\n";

////    CGraphSpace::Node::nbLinks=0;
////    CGraphSpace::Node::nbNodes=0;
////    CGraphSpace::Node::count=0;




//   // algo->componentGraphExp(connexity);

//    //algo->computeArea();




////    std::cout << "Naive algorithm\n";
////    CGraph *cgraph2=new CGraph(imSrc,connexity);
////    cgraph2->componentGraphNaive(connexity);
////    std::cout << "\nNumber of nodes: " <<     cgraph2->graph.size() << "\n";
////    cgraph2->writeDot("cgraphN1.dot");

//    //root->contrast=algo->computeContrast(root);

////    cgraph->computeArea();

//    cgraph->areaFiltering(areaMin);

//    // filtering active/non-active
//    cgraph->writeDot("cgraphF1.dot");

//    //cgraph->reconstructMin();

//        //cgraph->writeDot("cgraphF2.dot");


//    //algo->contrastFiltering(root,contrastMin,contrastMax);
//    // resolution of conflicts (min)
//    //cgraph->reconstructMin();


//   // algo->writeDot(root,"cgraphF2.dot");

//    Image<RGB> result=cgraph->constructImage();
//    result.save("result.ppm");

////    //algo->computeFlatZones();

////    //std::cout <<"Number of flat zones : " << algo->getNbFlatZones()<< "\n";

//    delete cgraph;
}

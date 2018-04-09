//Copyright (C) 2014, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.


#include <ctime>
//
#include "cgraph.h"
#include "cgraphwatcher.h"
#include "tuple.h"
#include "shaping.h"
#include "ComponentTree.h"

#include "ragraph.h"

using namespace std;

// for LibTIM classes
using namespace LibTIM;



class graphWatcher : public CGraphWatcher{
    public :
        void progressUpdate(int ncur, int nfinal) {
            if(ncur%100==0) {
                std::cout << ncur << " / " << nfinal << "\r";
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


template <class T1, class T2>
Image <T2> adjustContrast(Image <T1> &im, T2 A, T2  B)
{
    Image<T2> imRes(im.getSize());

    T1 b=im.getMax();
    T1 a=im.getMin();

    if( (b-a)!=0)
    {
        double ratio=(double)(B-A)/(b-a);
        for(int i=0; i<im.getBufSize(); i++) {
            imRes(i)=(T2)(A+(im(i)-a)*ratio);
            //std::cout << im(i) << " ";
        }
    }
    return imRes;
}

void printRAG(Rag<RGB> &rag)
{
    for (int i = 0; i < rag.vertices.size(); ++i)
    {
        std::cout << "Node : " << rag.vertices[i].n << "\n";
        std::cout << "\t Value : (" << (int)rag.vertices[i].value[0] << "," << (int)rag.vertices[i].value[1]
                  << "," << (int)rag.vertices[i].value[2] << ")\n";
        for (int j = 0; j < rag.vertices[i].edges.size(); ++j)
        {
            std::cout << "\t ->" << rag.vertices[i].edges[j] << "\n";
        }
    }
}

Image <U8> computeHistogram(const Image<RGB> &inputImage, const Image<U8> &mask, int histoBin)
{
    int dx=(int)(256.0/histoBin);
    int dy=(int)(256.0/histoBin);
    int dz=(int)(256.0/histoBin);

    Image<U8> imResult(dx,dy,dz);
    imResult.fill(0);

    for(int i=0; i<inputImage.getBufSize(); ++i){
        if(mask(i)>0) {
            int r = inputImage(i)[0];
            int g = inputImage(i)[1];
            int b = inputImage(i)[2];
            imResult(r / histoBin, g / histoBin, b / histoBin)++;
        }
    }

    return imResult;
}

std::vector<Image<U8>> computeMasks(Image<U8> &gtImage, int numberOfClasses)
{
    std::vector<Image <U8>> masks(numberOfClasses);
    for(int i=0; i<masks.size(); ++i) {
        masks[i].setSize(gtImage.getSizeX(), gtImage.getSizeY(),1);
        masks[i].fill(0);
    }

    for(int i=0; i<gtImage.getBufSize(); ++i) {
        U8 v=gtImage(i);
        if( v>0 &&  (v&(v-1))==0) {  // check if v is  a power of 2 (we discard intersections)
            int label = 0;      // compute the number of label by taking the log2 of value
            while (v >>= 1) ++label;

            masks[label](i)=255;
        }
    }

    return masks;
}

void saveMasks(std::vector<Image<U8>> &masks) {

    string s;

    for(int i=0; i<masks.size(); ++i) {
        s="mask_" + std::to_string(i) +".pgm";
        masks[i].save(s.c_str());
    }
}
void test1(int argc, char *argv[])
{
    if(argc!=6)
    {
        cout<<"Usage: " << argv[0] << " <inputImage> <ground truth> <test image> <number of bins> <number of classes> \n";
        exit(1);
    }

    Image <RGB> inputImage;
    Image<RGB>::load(argv[1],inputImage);

    Image <U8> gtImage;
    Image<U8>::load(argv[2],gtImage);

    Image <RGB> testImage;
    Image<RGB>::load(argv[3],testImage);

    auto numberOfBins=atoi(argv[4]);
    auto numberOfClasses=atoi(argv[5]);

    // Declaration of :
    // -connexity (8-adjacency)
    FlatSE connexity;
    connexity.make2DN8();

    FlatSE connexity26;
    connexity26.make3DN26();

    std::vector<Image<U8>> masks=computeMasks(gtImage,numberOfClasses);
    saveMasks(masks);

    std::vector<Image<U8>> histoClasses;

    for(int i=0; i<masks.size(); ++i) {
        Image<U8> imHisto=computeHistogram(inputImage,masks[i],numberOfBins);
        histoClasses.push_back(imHisto);
    }

    ComponentTree<U8> ctree(histoClasses[2],connexity26);

    ctree.writeDot("cthisto_0.dot");
    ctree.areaFiltering(100);
//    Image<U8> histoFiltered=ctree.constructImage(ComponentTree<U8>::DIRECT);

    Image<U8> histoFiltered=histoClasses[2];
    Image<U8> result(testImage.getSize());

    for(int i=0; i<testImage.getBufSize(); ++i) {
        RGB v=testImage(i);
        if(histoFiltered(v[0]/numberOfBins,v[1]/numberOfBins,v[2]/numberOfBins)>0) {
            result(i)=255;
        }
        else result(i)=0;

    }

    result.save("testResult.pgm");
}

void test2(int argc, char *argv[])
{
    if(argc!=4)
    {
        cout<<"Usage: " << argv[0] << " <inputImage> <shaping areamin> <shaping areamax>\n";
        exit(1);
    }

    Image <RGB> inputImage;
    Image<RGB>::load(argv[1],inputImage);

    auto areaMin=atoi(argv[2]);
    auto areaMax=atoi(argv[3]);

//    const int numBand=4;
//    typedef Tuple<int,numBand>  ImageType;
//
//    Image<ImageType> imTuple(10,10);
//    imTuple.fill(0);
//
//    for(int i=0; i<imTuple.getBufSize(); ++i){
//        for(int j=0; j<numBand; ++j) {
//            int v=rand()%2;
//            imTuple(i)[j]=v;
//        }
//    }

    // Declaration of :
    // -connexity (8-adjacency)
    FlatSE connexity;
    connexity.make2DN8();

    ColorMarginalOrdering order;
    Rag<RGB> rag=computeRag(inputImage,connexity);
    graphWatcher *myWatcher=new graphWatcher(inputImage.getBufSize());
    auto cgraph=CGraph<RGB>(inputImage,rag,order);
    cgraph.computeGraph(myWatcher);
    cgraph.computeAttributes();

    cgraph.writeDot<3>("cgraph.dot");

    cgraph.setShapingAttribute("fels contrast");
    Shaping<RGB> shaping(cgraph);
    shaping.computeShaping();
    shaping.computeArea();
    shaping.writeDot("shaping.dot");

    shaping.areaFiltering(areaMin,areaMax);

    shaping.writeDot("shapingFiltered.dot");

    // reconstruct graph
    shaping.constructGraph();

    // Compute resulting image from filtered cgraph
    Image<RGB> imResult=shaping.cgraph.constructImage(order);
    imResult.save("shaping_result.ppm");
}

/**
* JMIV experiments
**/

int main(int argc, char *argv[])
{
//    test1(argc,argv);

    test2(argc,argv);

//    TupleMarginalOrdering<numBand> order;

//    Rag<ImageType> rag=computeRag(imTuple,connexity);
//    printRAG(rag);
//
//    auto cgraph=CGraph<ImageType>(imTuple,rag,order);
//    graphWatcher *myWatcher=new graphWatcher(imTuple.getBufSize());
//
//    std::cout<<"computeGraph"<<std::endl;
//    cgraph.computeGraph(myWatcher);
//    cgraph.computeAttributes();
//
//    cgraph.writeDot<numBand>("cgraph.dot");



//    Rag<RGB> rag=computeRag(inputImage,connexity);
//    printRAG(rag);
//
//    ColorMarginalOrdering  order;
//    CGraph<RGB> cgraph=CGraph<RGB>(inputImage,rag,order);


    // Track computation progress
//    graphWatcher *myWatcher=new graphWatcher(inputImage.getBufSize());
//    // Set marginal ordering on RGB colour space

//    // Compute \ddot component-graph
//    std::cout<<"computeGraph"<<std::endl;
//    cgraph.computeGraph(myWatcher);
//    cgraph.computeAttributes();



//
//    //Filtering cgraph with area and contrast
//    std::cout<< "Area and contrast filtering\n";
//    cgraph.areaFiltering(areaMin);
//    Image <RGB> imResult=cgraph.constructImage(order);
//    imResult.save("imResult.ppm");
//    cgraph.contrastFiltering(contrastMin);

//    cgraph.writeDot("cgraph_filtered.dot");
//
//    std::cout << "computeShaping\n";
//
//    cgraph.setShapingAttribute("compacity");
//    Shaping<RGB> shaping(cgraph);
//    shaping.computeShaping();
//    shaping.computeArea();
//    shaping.writeDot("shaping.dot");
//    // compute shaping height (or contrast)
////    shaping->computeContrast();
////    shaping->writeDot("shaping.dot");
////
//    shaping.areaFiltering(areaMin);
////
//    shaping.writeDot("shapingFiltered.dot");
//
//    // reconstruct graph
//    shaping.constructGraph();
//    shaping.cgraph.writeDot("")
//
//    // Compute resulting image from filtered cgraph
//    Image<RGB> imResult=shaping.cgraph.constructImage(order);
//    imResult.save("shaping_result.ppm");

}


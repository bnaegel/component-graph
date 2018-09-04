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

#include "filters.hxx"

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

// demonstration of color image filtering based on shaping methodology
void test2(int argc, char *argv[])
{
    if(argc!=4)
    {
        cout<<"Usage: " << argv[0] << " <inputImage> <shaping areamin> <shaping areamax>\n";
        exit(1);
    }

    Image<RGB> inputImage;
    Image<RGB>::load(argv[1],inputImage);

    auto areaMin=atoi(argv[2]);
    auto areaMax=atoi(argv[3]);

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

    cgraph.writeDotFelsContrast<3>("cgraph.dot");

    // non-increasing attribute
    cgraph.setShapingAttribute("fels contrast");
    Shaping<RGB> shaping(cgraph);
    shaping.computeShaping();
    shaping.computeArea();
    shaping.computeContrast();
    shaping.writeDot("shaping.dot");

    shaping.contrastFiltering(areaMin,areaMax);

    shaping.writeDot("shapingFiltered.dot");

    // reconstruct graph
    shaping.constructGraph();

    // Compute resulting image from filtered cgraph
    Image<RGB> imResult=shaping.cgraph.constructImage(order);
    imResult.save("shaping_result.ppm");
}

Image<U8> binarizeSegmentation(Image<RGB> &inputImage)
{
    Image<U8> result(inputImage.getSizeX(), inputImage.getSizeY());

    for(int i=0; i<inputImage.getBufSize(); ++i) {
        if(inputImage(i)[0]==255 && inputImage(i)[1]==0 && inputImage(i)[2]==0){
            result(i)=255;
        }
        else result(i)=0;
    }
    return result;
}

void controlImTuple(Image<Tuple<int,3>> image)
{
    Image<U8> res1(image.getSize());
    Image<U8> res2(image.getSize());
    Image<U8> res3(image.getSize());

    for(int i=0; i<image.getBufSize(); ++i){
        res1(i)=image(i)[0] ? 255 : 0;
        res2(i)=image(i)[1] ? 255 : 0;
        res3(i)=image(i)[2] ? 255 : 0;
    }

    res1.save("res1.pgm");
    res2.save("res2.pgm");
    res3.save("res3.pgm");
}

// the number of segmentations should be known in advance, it defines the number of bands (type Tuple<int, numBand>)
void test3(int argc, char *argv[])
{
    // input object database : color images
    if(argc<3)
    {
        cout<<  "Usage: " << argv[0] << "<input image> [<segmentation 1> <segmentation 2>  <segmentation 3>] <cmin> <cmax>\n";
        cout << "\t\t segmentation image : 0=background, non 0=foreground\n";
        exit(1);
    }

    Image <U8> inputImage;
    Image <U8>::load(argv[1],inputImage);

    int dx=inputImage.getSizeX();
    int dy=inputImage.getSizeY();

    int contrastMin=atoi(argv[5]);
    int contrastMax=atoi(argv[6]);


    // load segmentation images (binary masks) 
    std::vector<Image<U8>> segImages; 
    // 3 bands 
    const int numBand=3;

    for(int i=0; i<numBand; ++i) {
        Image<RGB> tmp;
        Image<RGB>::load(argv[i+2],tmp);
        Image<U8> seg=binarizeSegmentation(tmp);
       
        segImages.push_back(seg);
    }

    typedef Tuple<int,numBand> ImageType;
    
    Image<ImageType> imTuple(dx,dy);
    imTuple.fill(0);

    for(int i=0; i<imTuple.getBufSize(); ++i){
        for(int j=0; j<numBand; ++j) {
            int v=segImages[j](i);
            imTuple(i)[j]=v>0 ? 1 : 0;
        }
        // // zone de consensus 
        // if(imTuple(i)[0]==1 && imTuple(i)[1]==1 && imTuple(i)[2]==1  ){
        //     imTuple(i)[0]=0; imTuple(i)[1]=0; imTuple(i)[2]=0;
        // }
    }

    controlImTuple(imTuple);

    Image<U8> gradImage=gradientSobel(inputImage);
    gradImage.save("gradSobel.pgm");

    // Declaration of :
    // -connexity (8-adjacency)
    FlatSE connexity;
    connexity.make2DN8();

    TupleMarginalOrdering<numBand> order;
    Rag<ImageType> rag=computeRag(imTuple,connexity);
    graphWatcher *myWatcher=new graphWatcher(inputImage.getBufSize());
    auto cgraph=CGraph<ImageType>(imTuple,rag,order);
    cgraph.computeGraph(myWatcher);
    cgraph.computeAttributes();
    cgraph.computeMeanGradientHelper(gradImage);

    cgraph.writeDot<3>("cgraph.dot");

    cgraph.setShapingAttribute("mean gradient");
    Shaping<ImageType> shaping(cgraph);
    shaping.computeShaping();
    shaping.computeContrast();
    shaping.computeArea();
    shaping.writeDot("shaping.dot");

    shaping.contrastFiltering(contrastMin,contrastMax);

    shaping.writeDot("shapingFiltered.dot");

    // reconstruct graph
    shaping.constructGraph();

    // Compute resulting image from filtered cgraph
    Image<ImageType> imResult=shaping.cgraph.constructImage(order);

    Image<U8> result(imResult.getSize());
    for(int i=0; i<result.getBufSize(); ++i) {
        if(imResult(i)[0]==1 || imResult(i)[1]==1 || imResult(i)[2]==1){
            result(i)=255;
        }
        else result(i)=0;
    }
    result.save("shaping_result.pgm");
}

/**
* JMIV experiments
**/

int main(int argc, char *argv[])
{
    // test2(argc,argv);
    test3(argc,argv);

   


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


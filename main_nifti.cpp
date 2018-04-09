//Copyright (C) 2014, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.


#include <ctime>
#include <nifti1_io.h>

#include "cgraph.h"
#include <cgraphwatcher.h>
#include "ordering.h"
#include "shaping.h"

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

template <typename T>
Image<T> readNifti(char *filename, nifti_1_header* nfHeader=NULL)
{

    nifti_image *nim=NULL;
    nim=nifti_image_read(filename,1);
    if (nfHeader != NULL)
		*nfHeader = nifti_convert_nim2nhdr(nim);
    if(nim!=0) {
        Image<T> inputImage(nim->nx,nim->ny,nim->nz);


        for(int i=0; i<inputImage.getBufSize(); i++) {
            inputImage(i)=((T *)nim->data)[i];
        }
        std::cout <<"Image dimensions : " << inputImage.getSizeX() << " " << inputImage.getSizeY() << " " << inputImage.getSizeZ() << "\n";
        free(nim);
        return inputImage;
    }
    else {
        std::cout << "read error \n";
        exit(1);
    }

}


void writeNifti(Image<unsigned char> &im, const char *filename, nifti_1_header* nfHeader=NULL)
{

    nifti_image *nim=NULL;
 	if (nfHeader != NULL)
 	{
		nim = nifti_convert_nhdr2nim(*nfHeader, filename); //cree image nifti avec le header de l'image d'entree
		std::cout<<"nifti_convert_nhdr2nim DONE" <<std::endl;
		int test = nifti_set_filenames(nim,filename,0,nim->byteorder);
		std::cout<<"test " << test << std::endl;
	} 
	else
	{
		std::cout << "ENTER ELSE " << std::endl;    
		int t[8] = {3,im.getSizeX(), im.getSizeY(), im.getSizeZ(), 0,0,0,0};
		nim = nifti_make_new_nim(t,2,1);
		int test = nifti_set_filenames(nim,filename,0,nim->byteorder);
	}
	std::cout<<"REMPLISSAGE"<<std::endl;
	std::cout<<"im.getBufSize() "<<im.getBufSize()<<std::endl; 
	// Attention : on suppose que le type de l'image de nim est short (a recup dans le nfHeader !)
	nim->data = new short[im.getSizeX()*im.getSizeY()*im.getSizeZ()];
    for(int i=0; i<im.getBufSize(); i++) {
        ((short*)(nim->data))[i]= im(i); // remplissage
    }
    std::cout<<"END REMPLISSAGE" << std::endl;

    nifti_image_write(nim);
}



// keep band=0,1,2 and return associate unsigned char image
Image<unsigned char> keepBand(Image<RGB> &im, int band)
{
    assert(band==0 || band==1 || band==2);
    Image<unsigned char> imRes(im.getSize());

    for(int i=0; i<imRes.getBufSize(); i++) {
        imRes(i)=im(i)[band];
    }

    return imRes;
}


// associate im1 and im2 to red band and green band of result image
Image<RGB> mergeBands(Image<unsigned char> &im1, Image <unsigned char> &im2)
{

    Image<RGB> imRes(im1.getSize());

    for(int i=0; i<imRes.getBufSize(); i++) {
        // red
        imRes(i)[0]=im1(i);
        // green
        imRes(i)[1]=im2(i);
        //blue
        imRes(i)[2]=0;
    }

    return imRes;
}

/**
* Colour image attribute filtering based on \ddot component-graph on RGB colour space
**/

int main(int argc, char *argv[])
{
    if(argc!=6)
    {
        cout<<"Usage: " << argv[0] << " <ct_image.nii> <pet_image.nii>  <area_min> <height_min> <elong_min>\n";
        exit(1);
    }

    // Declaration of:
    // - ctImage - petImage

    Image <short> ctImageOri=readNifti<short>(argv[1]);
	nifti_1_header petHeader;
    Image <short> petImageOri=readNifti<short>(argv[2], &petHeader);
    //std::cout <<"Image dimensions : " << ctImageOri.getSizeX() << " " << ctImageOri.getSizeY() << " " << ctImageOri.getSizeZ() << "\n";

    Image <unsigned char> ctImage;
    Image <unsigned char> petImage;

    ctImage=adjustContrast(ctImageOri,(unsigned char)0,(unsigned char)255);
    petImage=adjustContrast(petImageOri,(unsigned char)0,(unsigned char)255);

    Image<RGB> ctPetImage=mergeBands(ctImage,petImage);

    // Declaration of :
    // -connexity (26-adjacency)
    FlatSE connexity;
    connexity.make3DN26();


    int areaMin=atoi(argv[3]);
    //height or contrast
    int contrastMin=atoi(argv[4]);

    int elongMin=atoi(argv[5]);

    CGraph *cgraph=new CGraph(ctPetImage,connexity);

    // Track computation progress
    graphWatcher *myWatcher=new graphWatcher(ctPetImage.getBufSize());
    // Set marginal ordering on RGB colour space
    ColorMarginalOrdering  *order=new ColorMarginalOrdering();
    // Compute \ddot component-graph
    std::cout<<"computeGraph"<<std::endl;
    cgraph->computeGraph(order,myWatcher);
    
    
    // Compute elongation attribute
    std::cout<<"computeMU"<<std::endl;
    cgraph->computeMU();
    std::cout<<"computeElongation"<<std::endl;
	cgraph->computeElongation();
    cgraph->writeDot("cgraph.dot");

    //Filtering cgraph with area and contrast
    std::cout<< "Area and contrast filtering\n";
    cgraph->areaFiltering(areaMin);
    cgraph->contrastFiltering(contrastMin);

    std::cout << "computeShaping\n";
    Shaping *shaping=new Shaping(cgraph);
    shaping->computeShaping();

    std::cout << "compute shaping area and contrast";

    shaping->computeArea();
    // compute shaping height (or contrast)
    shaping->computeContrast();
    shaping->writeDot("shaping.dot");

    // filter shaping w.r.t. height (or contrast)
    //shaping->contrastFiltering(contrastMin);
    // maximum contrast can be specified optionnaly :
    //shaping->contrastFiltering(contrastMin,contrastMax);

    // Area and contrast filtering
    //shaping->areaFiltering(areaMin);
    //shaping->contrastFiltering(contrastMin);
    //shaping->contrastFilteringMax(contrastMin);
    shaping->elongFiltering(elongMin);

    shaping->writeDot("shapingFiltered.dot");

    // reconstruct graph
    shaping->constructGraph();


    // Write graph in dot format
    // Filtered nodes are shown in gray
    //cgraph->writeDot("cgraph.dot");

    // Compute resulting image from filtered cgraph
    Image<RGB> imResult=shaping->cgraph->constructImage(order);

    Image<unsigned char> imResultFirstBand=keepBand(imResult,0);

    // Save result in nifti format
    writeNifti(imResultFirstBand,"ctResult.nii", &petHeader);
     //writeNifti(imResultFirstBand,"ctResult.nii");
    //writeNifti(petImage,"petResult.nii");

    //delete cgraph;
}


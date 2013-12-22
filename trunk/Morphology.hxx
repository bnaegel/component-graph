#include <queue>
#include "FlatSE.h"
#include "Image.h"
#include <cmath>
#include <unistd.h>



namespace LibTIM {

/** \defgroup Morpho Morphological Operators
	Mathematical morphology operators
**/
/** \defgroup basisFunctions Basis Functions
*	\ingroup Morpho
**/
/*@{*/


template <class T>
void addBorders(Image<T> &im,
					const TCoord *preWidth,
					const TCoord *postWidth,
					T value)
{
	TSize newSize[3];
	const TSize *oriSize = im.getSize();
	
	for (int i = 0; i < 3; i++)
	{
		newSize[i] = oriSize[i] + preWidth[i] + postWidth[i];
	}

	Image<T> temp(newSize);
	
	typename Image<T>::iterator it;
	typename Image<T>::iterator end=temp.end();
	
	std::fill(temp.begin(), end,value);
	temp.copy(im, preWidth[0],preWidth[1],preWidth[2]);

	im= temp;
}

//Maybe should be put in another class "Border Image" or something
//Maybe attach, like in ITK, a region of interest for each image
template <class T>
void addBorders(Image<T> &im, FlatSE &se, T value)
{
	TSize newSize[3];
	const TSize *oriSize = im.getSize();
	
	TCoord backOffsets[3];
	TCoord frontOffsets[3];
	
	FlatSE::iterator_point itSe;
	FlatSE::iterator_point endSe=se.end_point();
	
	for (int i = 0; i < 3; i++) backOffsets[i] = 0;
	for (itSe=se.begin_point(); itSe!=endSe; ++itSe)
	{
		backOffsets[0] = std::min(backOffsets[0],(itSe->x));
		backOffsets[1] = std::min(backOffsets[1],(itSe->y));
		backOffsets[2] = std::min(backOffsets[2],(itSe->z));
	}
	
	for (int i = 0; i < 3; i++) frontOffsets[i] = 0;
	for (itSe=se.begin_point(); itSe!=endSe; ++itSe)
	{
		frontOffsets[0] = std::max(frontOffsets[0],(itSe->x));
		frontOffsets[1] = std::max(frontOffsets[1],(itSe->y));
		frontOffsets[2] = std::max(frontOffsets[2],(itSe->z));
	}
	
// 	std::cout << "Back offsets: " << backOffsets[0] << " " << backOffsets[1] << " "
// 	<< backOffsets[2] << "\n";
// 	std::cout << "Front offsets: " << frontOffsets[0] << " " << frontOffsets[1] << " " << frontOffsets[2] << "\n";
	
	for (int i = 0; i < 3; i++)
	{
		newSize[i] = oriSize[i]+(TSize)abs(backOffsets[i]) + (TSize) abs(frontOffsets[i]);
	}

	Image<T> temp(newSize);
	
	typename Image<T>::iterator it;
	typename Image<T>::iterator end=temp.end();
	
	std::fill(temp.begin(), end,value);
	temp.copy(im, abs(backOffsets[0]), abs(backOffsets[1]),abs(backOffsets[2]) );

	im= temp;
}

///Basic flat-dilation algorithm
/**
	Computes the dilation of im by flat structuring element se
	according to Heijman's definition (different from Soille)
**/

template <class T>
Image<T> dilation(Image<T> im, FlatSE se)
{
	Image<T> res=im;
	
	//Symmetric of structuring element, according to Heijman's definition of dilation
	se.makeSymmetric();
	
	const TCoord *back=se.getNegativeOffsets();
	const TCoord *front=se.getPositiveOffsets();
	
	T minValue=std::numeric_limits<T>::min();
	addBorders(im,back,front,minValue);
	se.setContext(im.getSize());
	
	typename Image<T>::iteratorXYZ it;
	typename Image<T>::iteratorXYZ end=res.end();
	
	FlatSE::iterator itSe;
	FlatSE::iterator endSe=se.end();
	
	T min=std::numeric_limits<T>::min();
	
	T currentMax;

	for(it=res.begin(); it!=end; ++it)
		{
		currentMax = min;
		TOffset offsetRes = im.getOffset(it.x + back[0], it.y + back[1], it.z + back[2]);
		for(itSe=se.begin(); itSe!=endSe; ++itSe)
			{
			TOffset offsetIm=offsetRes+*itSe;
			currentMax=std::max(currentMax,im(offsetIm) );
			}
		*it=currentMax;
		}
	
	return  res;
}

///Basic flat-erosion algorithm
/**
	Computes the erosion of im by flat structuring element se.
**/


template <class T>
Image<T> erosion(Image<T> im, FlatSE se)
{
	Image<T> res=im;
	
	const TCoord *back=se.getNegativeOffsets();
	const TCoord *front=se.getPositiveOffsets();
	
	T maxValue=std::numeric_limits<T>::max();
	addBorders(im,back,front,maxValue);
	se.setContext(im.getSize());
	
	typename Image<T>::iteratorXYZ it;
	typename Image<T>::iteratorXYZ end=res.end();
	
	FlatSE::iterator itSe;
	FlatSE::iterator endSe=se.end();
	
	T max=std::numeric_limits<T>::max();
	
	T currentMin;

	for(it=res.begin(); it!=end; ++it)
		{
		currentMin = max;
		TOffset offsetRes = im.getOffset(it.x + back[0], it.y + back[1], it.z + back[2]);
		for(itSe=se.begin(); itSe!=endSe; ++itSe)
			{
			TOffset offsetIm=offsetRes+*itSe;
			currentMin=std::min(currentMin,im(offsetIm) );
			}
		*it=currentMin;
		}
	
	return  res;
}

///Border max version of dilation
/** 
	Computes the dilation but with border set to the maximum possible value of the image type
	Useful for template matching, when one not want to detect something when hitting the border
**/

template <class T>
Image<T> dilationBorderMax(Image<T> im, FlatSE se)
{
	Image<T> res(im.getSize() );
	
	//Symmetric of structuring element, according to Heijman's definition of dilation
	se.makeSymmetric();
	
	const TCoord *back=se.getNegativeOffsets();
	const TCoord *front=se.getPositiveOffsets();
	
	T maxValue=std::numeric_limits<T>::max();
	addBorders(im,back,front,maxValue);
	se.setContext(im.getSize());
	
	typename Image<T>::iteratorXYZ it;
	typename Image<T>::iteratorXYZ end=res.end();
	
	FlatSE::iterator itSe;
	FlatSE::iterator endSe=se.end();
	
	T min=std::numeric_limits<T>::min();
	
	T currentMax;

	for(it=res.begin(); it!=end; ++it)
		{
		currentMax = min;
		TOffset offsetRes = im.getOffset(it.x + back[0], it.y + back[1], it.z + back[2]);
		for(itSe=se.begin(); itSe!=endSe; ++itSe)
			{
			TOffset offsetIm=offsetRes+*itSe;
			currentMax=std::max(currentMax,im(offsetIm) );
			}
		*it=currentMax;
		}
	
	return  res;
}

///Border min version of erosion
/** 
	Computes the erosion but with border set to the minimum possible value of the image type
	Useful for template matching, when one not want to detect something when hitting the border
**/
template <class T>
Image<T> erosionBorderMin(Image<T> im, FlatSE se)
{
	Image<T> res(im.getSize() );
	
	const TCoord *back=se.getNegativeOffsets();
	const TCoord *front=se.getPositiveOffsets();
	
	T minValue=std::numeric_limits<T>::min();
	addBorders(im,back,front,minValue);
	se.setContext(im.getSize());
	
	typename Image<T>::iteratorXYZ it;
	typename Image<T>::iteratorXYZ end=res.end();
	
	FlatSE::iterator itSe;
	FlatSE::iterator endSe=se.end();
	
	T max=std::numeric_limits<T>::max();
	
	T currentMin;

	for(it=res.begin(); it!=end; ++it)
		{
		currentMin = max;
		TOffset offsetRes = im.getOffset(it.x + back[0], it.y + back[1], it.z + back[2]);
		for(itSe=se.begin(); itSe!=endSe; ++itSe)
			{
			TOffset offsetIm=offsetRes+*itSe;
			currentMin=std::min(currentMin,im(offsetIm) );
			}
		*it=currentMin;
		}
	
	return  res;
}

///Opening
/** 
	Computes the opening of im by se
**/

template <class T>
Image <T> opening(Image<T> im, FlatSE se)
{
	return dilation(erosion(im,se),se);
}

///Closing
/** 
	Computes the closing of im by se
**/


template <class T>
Image <T> closing(Image<T> im, FlatSE se)
{
	return erosion(dilation(im,se),se);
}

///Morphological gradient
/** 
	Computes the morphological gradient (or Beucher gradient)
	@param im The source image (not modified)
	@param se The structuring element (not modified)
	@return The morphological gradient of im
**/

template <class T>
Image <T> morphologicalGradient(Image <T> im, FlatSE se)
{
	Image <T> tmp=erosion(im,se);
	im=dilation(im,se);
	im-=tmp;
	return im;
}

///Internal morphological gradient
/** 
	Computes the internal morphological gradient 
	@param im The source image (not modified)
	@param se The structuring element (not modified)
	@return The internal morphological gradient of im
**/

template <class T>
Image <T> internalMorphologicalGradient(Image <T> im, FlatSE se)
{
	Image <T> tmp=erosion(im,se);
	im-=tmp;
	return im;
}

///External morphological gradient
/** 
	Computes the external morphological gradient 
	@param im The source image (not modified)
	@param se The structuring element (not modified)
	@return The external morphological gradient of im
**/

template <class T>
Image <T> externalMorphologicalGradient(Image <T> im, FlatSE se)
{
	Image <T> tmp=dilation(im,se);
	tmp-=im;
	return tmp;
}

///Rank filter
/**
	Computes the rank filter.
	@param im The source image 
	@param se The structuring element
	@param rank The  rank of the filter(rank=0 is equivalent to erosion; rank=se.getNbPoints()-1 is equivalent to dilation)
	@return The filtered image.
**/
//Border problem solutions: enlarge image by copying border pixels, by mirroring the image, or simply using
//max or min global value, or max or min neighboring value
//For now: we take the n iest element, and fill the border with max value

template <class T>
Image<T> rankFilter(Image<T> im, FlatSE se, int rank)
{
	int neighborSize=se.getNbPoints();
	
	if(rank<=0)
		return erosion(im,se);
		
	if(rank>=neighborSize-1)
		return dilation(im,se);
	else
		{	
		Image<T> res(im.getSize() );
		
		const TCoord *back=se.getNegativeOffsets();
		const TCoord *front=se.getPositiveOffsets();
		
		T maxValue=std::numeric_limits<T>::max();
		addBorders(im,back,front,maxValue);
		se.setContext(im.getSize());
		
		typename Image<T>::iteratorXYZ it;
		typename Image<T>::iteratorXYZ end=res.end();
		
		FlatSE::iterator itSe;
		FlatSE::iterator endSe=se.end();
		
		T *neighborsPtr=new T[neighborSize];
		
		T *currentNeighbor;
		int nElt;
		
		for(it=res.begin(); it!=end; ++it)
			{
			currentNeighbor=neighborsPtr;
			nElt=0;
			TOffset offsetRes = im.getOffset(it.x + back[0], it.y + back[1], it.z + back[2]);
			for(itSe=se.begin(); itSe!=endSe; ++itSe)
				{
				TOffset offsetIm=offsetRes+*itSe;
				T el=im(offsetIm);
				*currentNeighbor=el;
				currentNeighbor++;
				}

			sort(neighborsPtr,neighborsPtr+neighborSize );
			*it=neighborsPtr[rank];
			}
		delete neighborsPtr;
		return  res;
	}
}

/*@}*/

/** \defgroup extremaExtraction Regional Extrema Extraction
	\ingroup Morpho
**/

/*@{*/

///Regional Minima Extraction
/**
Compute the regional minima of the source image. Returns a \e binary <U8> image with:
	\li 0 : not minima
	\li 255: is minima
	
Usually, you need to labelise the result with the labelConnectedComponents() function.
	@param img Source Image
	@param se Connexity used (use for example 4- or 8- connexity in 2D, see the FlatSE documentation)
	@return <U8> binary image (0=not minima, 255=minima)

Algorithm of Vincent
**/
template <class T>
Image <U8> regionalMinima(Image <T> img, FlatSE se)
{
	std::queue<TOffset> fifo;
	
	//std::fill(imgMinima, &imgMinima[img.getBufSize()],true);
	
	const TCoord *back=se.getNegativeOffsets();
	const TCoord *front=se.getPositiveOffsets();
	
	U8 isNonExtrema=0;
	U8 isExtrema=1;
	U8 isBorder=2;
	T maxValue=std::numeric_limits<T>::max();
	
	Image <T> im=img;
	Image<U8> imgMinima=img;
	imgMinima.fill(isExtrema);
	
	addBorders(im,back,front,maxValue);
	addBorders(imgMinima,back,front,isBorder);
	se.setContext(im.getSize());
	
	typename Image<T>::iterator it;
	typename Image<T>::iterator end=im.end();
	
	typename Image<T>::iteratorXYZ itxyz;
	
	typename FlatSE::iterator itSe;
	typename FlatSE::iterator endSe=se.end();
	
	TOffset curOffset=0;
 	for(it=im.begin(); it!=end; ++it,curOffset++)
 		{
 		TOffset p=curOffset;
		
 		if(imgMinima(p)==isExtrema)
			{
			for(itSe=se.begin();  itSe!=endSe; ++itSe)
  					{
  					TOffset  q= p+ *itSe;
 					if(im(q) < im(p))
  						{
  						//p has a neighbor q with lesser intensity: p belongs not to a regional minima
 						//we propagate to eliminate all neighbors having an intensity equal or superior
  							
 						fifo.push(p);
  						break;
  						}
  					}	
				while(fifo.empty()!=true)
					{
					TOffset p=fifo.front();
					fifo.pop();
					imgMinima(p)=isNonExtrema;
					
					for(itSe=se.begin();  itSe!=endSe; ++itSe)
						{
						TOffset q= p + *itSe;
						if(im(q)>=im(p) && imgMinima(q)==isExtrema)
							{
							imgMinima(q)=isNonExtrema;
							fifo.push(q);
							}
						}
					}
				}
		}
	
	//Reste le recadrage dans les dimensions originales.
	//utiliser classe spécifique?
	
	Image <U8> result=img;
	typename Image<U8>::iteratorXYZ itLabelXYZ;
	typename Image<U8>::iteratorXYZ endRes=result.end();
	
	for(itLabelXYZ=result.begin(); itLabelXYZ!=endRes; ++itLabelXYZ)
		{
		if(imgMinima(itLabelXYZ.x+back[0], itLabelXYZ.y+back[1], itLabelXYZ.z+back[2])==true)
			*itLabelXYZ=255;
		else *itLabelXYZ=0;
		}
	
	
	return result;	
}

///Regional Maxima Extraction
/**
Compute the regional maxima of the source image. Returns a \e binary <U8> image with:
	\li 0 : not maxima
	\li 255: is maxima
	
Usually, you need to labelise the result with the labelConnectedComponents() function.
	@param img Source Image
	@param se Connexity used (use for example 4- or 8- connexity in 2D, see the FlatSE documentation)
	@return <U8> binary image (0=not maxima, 255=maxima)

Algorithm of Vincent
**/

template <class T>
Image <U8> regionalMaxima(Image <T> img, FlatSE se)
{
	std::queue<TOffset> fifo;
	
	//std::fill(imgMinima, &imgMinima[img.getBufSize()],true);
	
	const TCoord *back=se.getNegativeOffsets();
	const TCoord *front=se.getPositiveOffsets();
	
	U8 isNonExtrema=0;
	U8 isExtrema=1;
	U8 isBorder=2;
	T minValue=std::numeric_limits<T>::min();
	
	Image <T> im=img;
	Image<U8> imgMaxima(img.getSize());
	imgMaxima.fill(isExtrema);
	
	addBorders(im,back,front,minValue);
	addBorders(imgMaxima,back,front,isBorder);
	se.setContext(im.getSize());
	
	typename Image<T>::iterator it;
	typename Image<T>::iterator end=im.end();
	
	typename Image<T>::iteratorXYZ itxyz;
	
	typename FlatSE::iterator itSe;
	typename FlatSE::iterator endSe=se.end();
	
	TOffset curOffset=0;
 	for(it=im.begin(); it!=end; ++it,curOffset++)
 		{
 		TOffset p=curOffset;
		
 		if(imgMaxima(p)==isExtrema)
			{
			for(itSe=se.begin();  itSe!=endSe; ++itSe)
  					{
  					TOffset  q= p+ *itSe;
 					if(im(q) > im(p))
  						{
  						//p has a neighbor q with higher intensity: p belongs not to a regional minima
 						//we propagate to eliminate all neighbors having an intensity equal or superior
  							
 						fifo.push(p);
  						break;
  						}
  					}	
				while(fifo.empty()!=true)
					{
					TOffset p=fifo.front();
					fifo.pop();
					imgMaxima(p)=isNonExtrema;
					
					for(itSe=se.begin();  itSe!=endSe; ++itSe)
						{
						TOffset q= p + *itSe;
						if(im(q)<=im(p) && imgMaxima(q)==isExtrema)
							{
							imgMaxima(q)=isNonExtrema;
							fifo.push(q);
							}
						}
					}
				}
		}
	
	//Reste le recadrage dans les dimensions originales.
	//utiliser classe spécifique?
	
	Image <U8> result=img;
	typename Image<U8>::iteratorXYZ itLabelXYZ;
	typename Image<U8>::iteratorXYZ endRes=result.end();
	
	for(itLabelXYZ=result.begin(); itLabelXYZ!=endRes; ++itLabelXYZ)
		{
		if((imgMaxima(itLabelXYZ.x+back[0], itLabelXYZ.y+back[1], itLabelXYZ.z+back[2])==true) )
			*itLabelXYZ=255;
		else *itLabelXYZ=0;
		}

	return result;	
}

/*@}*/

/** \defgroup reconstruction Geodesic Reconstruction
	\ingroup Morpho
**/

/*@{*/

/// Geodesic reconstruction by erosion 
/** 
	Marker must be above the mask
	@param marker The marker image. 
	At the end of function marker is modified and contains the result of reconstruction.
	@param mask The mask image (not modified).
	
	Vincent's algorithm. In this implementation \e all points are inserted in the priority queue
**/
template <class T>
void geodesicReconstructionByErosion(Image <T> &marker, Image <T> mask, FlatSE &se)
{
	OrderedQueue <TOffset> oq;
	
	const TCoord *back=se.getNegativeOffsets();
	const TCoord *front=se.getPositiveOffsets();
	
	Image <T> imInf=mask;
	Image <T> imSup=marker;
	Image <bool> imActivePoints(mask.getSize());
	imActivePoints.fill(true);
	
	T maxValue=std::numeric_limits<T>::max(); 
	
	addBorders(imInf,back,front,maxValue);
	addBorders(imSup,back,front,maxValue);
	addBorders(imActivePoints,back,front,false);
	
	se.setContext(imSup.getSize());
	
	//put all marker points in the queue
	//lowest points are more prioritary
	//note that only extrema are necessary: 
	typename Image<T>::iterator it;
	typename Image<T>::iterator end=imSup.end();
	typename Image<T>::iteratorXYZ itxyz;
	
	TOffset offset=0;
	for(itxyz=imSup.begin(); itxyz!=end; ++itxyz,offset++)
		{
		if(imActivePoints(offset)==true)
			oq.put(*itxyz,offset);
		}
		
	FlatSE::iterator itSe;
	FlatSE::iterator endSe=se.end();
	
	while(!oq.empty())
		{
		TOffset p=oq.get();
		for(itSe=se.begin(); itSe!=endSe; ++itSe)
			{
			TOffset q=p+ *itSe;
			if(imActivePoints(q)==true)
				{ 
				imSup(q)=std::max(imSup(p),imInf(q) );
				oq.put(imSup(q),q);
				imActivePoints(q)=false;
				}
			}
		}
		
	Image <T> result=marker;
	typename Image<T>::iteratorXYZ itLabelXYZ;
	for(itLabelXYZ=result.begin(); itLabelXYZ!=result.end(); ++itLabelXYZ)
		{
		*itLabelXYZ=imSup(itLabelXYZ.x+back[0], itLabelXYZ.y+back[1], itLabelXYZ.z+back[2]);
		}
	marker=result;
}

/// Geodesic reconstruction by erosion 
/** 
	Marker must be above the mask
	@param marker The marker image. 
	At the end of function marker is modified and contains the result of reconstruction.
	@param mask The mask image (not modified).
	
	Vincent's algorithm. In this implementation \e all points are inserted in the priority queue
**/
template <class T>
void geodesicReconstructionByErosion(Image <T> &marker, Image <T> mask, FlatSE &se, T borderValue)
{
	OrderedQueue <TOffset> oq;
	
	const TCoord *back=se.getNegativeOffsets();
	const TCoord *front=se.getPositiveOffsets();
	
	Image <T> imInf=mask;
	Image <T> imSup=marker;
	Image <bool> imActivePoints(mask.getSize());
	imActivePoints.fill(true);
	
	T maxValue=std::numeric_limits<T>::max(); 
	
	addBorders(imInf,back,front,borderValue);
	addBorders(imSup,back,front,borderValue);
	addBorders(imActivePoints,back,front,false);
	
	se.setContext(imSup.getSize());
	
	//put all marker points in the queue
	//lowest points are more prioritary
	//note that only extrema are necessary: 
	typename Image<T>::iterator it;
	typename Image<T>::iterator end=imSup.end();
	typename Image<T>::iteratorXYZ itxyz;
	
	TOffset offset=0;
	for(itxyz=imSup.begin(); itxyz!=end; ++itxyz,offset++)
		{
		if(imActivePoints(offset)==true)
			oq.put(*itxyz,offset);
		}
		
	FlatSE::iterator itSe;
	FlatSE::iterator endSe=se.end();
	
	while(!oq.empty())
		{
		TOffset p=oq.get();
		for(itSe=se.begin(); itSe!=endSe; ++itSe)
			{
			TOffset q=p+ *itSe;
			if(imActivePoints(q)==true)
				{ 
				imSup(q)=std::max(imSup(p),imInf(q) );
				oq.put(imSup(q),q);
				imActivePoints(q)=false;
				}
			}
		}
		
	Image <T> result=marker;
	typename Image<T>::iteratorXYZ itLabelXYZ;
	for(itLabelXYZ=result.begin(); itLabelXYZ!=result.end(); ++itLabelXYZ)
		{
		*itLabelXYZ=imSup(itLabelXYZ.x+back[0], itLabelXYZ.y+back[1], itLabelXYZ.z+back[2]);
		}
	marker=result;
}

/// Geodesic reconstruction by dilation 
/** 
	Marker must be under the mask
	@param marker The marker image. 
	At the end of function marker is modified and contains the result of reconstruction.
	@param mask The mask image (not modified).
	
	Vincent's algorithm. In this implementation \e all points are inserted in the priority queue
**/
template <class T>
void geodesicReconstructionByDilation(Image <T> &marker, Image <T> mask, FlatSE &se)
{
	OrderedQueue <TOffset> oq;
	
	const TCoord *back=se.getNegativeOffsets();
	const TCoord *front=se.getPositiveOffsets();
	
	Image <T> imSup=mask;
	Image <T> imInf=marker;
	Image <bool> imActivePoints(mask.getSize());
	imActivePoints.fill(true);
	
	T minValue=std::numeric_limits<T>::min(); 
	
	addBorders(imInf,back,front,minValue);
	addBorders(imSup,back,front,minValue);
	addBorders(imActivePoints,back,front,false);
	
	se.setContext(imInf.getSize());
	
	//put all marker points in the queue
	//highest points are more prioritary, so priority is reversed
	//note that only extrema are necessary: 
	typename Image<T>::iterator it;
	typename Image<T>::iterator end=imInf.end();
	typename Image<T>::iteratorXYZ itxyz;
	
	TOffset offset=0;
	for(itxyz=imInf.begin(); itxyz!=end; ++itxyz,offset++)
		{
		if(imActivePoints(offset)==true)
			//we reverse priority for geodesic dilation
			oq.put(-*itxyz,offset);
		}
		
	FlatSE::iterator itSe;
	FlatSE::iterator endSe=se.end();
	
	while(!oq.empty())
		{
		TOffset p=oq.get();
		for(itSe=se.begin(); itSe!=endSe; ++itSe)
			{
			TOffset q=p+ *itSe;
			if(imActivePoints(q)==true)
				{ 
				imInf(q)=std::min(imInf(p),imSup(q) );
				//we reverse priority for geodesic dilation
				oq.put(-imInf(q),q);
				imActivePoints(q)=false;
				}
			}
		}
		
	Image <T> result=marker;
	typename Image<T>::iteratorXYZ itLabelXYZ;
	for(itLabelXYZ=result.begin(); itLabelXYZ!=result.end(); ++itLabelXYZ)
		{
		*itLabelXYZ=imInf(itLabelXYZ.x+back[0], itLabelXYZ.y+back[1], itLabelXYZ.z+back[2]);
		}
	marker=result;
}

/*@}*/

/** \defgroup connectedOperators Connected Operators
	\ingroup Morpho
**/

/*@{*/

///h-Min filter
/**
	@param img Source Image. 
	At the end of function, img is modified and contains the result
	@param h h parameter of filter
	
	Warning: potential overflow problem due to type limitation (when using U8).
**/
//Warning: potential overflow problem due to type limitation (when using U8)
template <class T>
void hMinFilter(Image <T> &img, FlatSE &se, int h)
{
	Image <T> mask=img;
	typename Image<T>::iterator it;
	typename Image<T>::iterator end=img.end();
	for(it=img.begin(); it!=end ; ++it)
		*it+=h;
	geodesicReconstructionByErosion(img,mask,se);	
}

///h-Max filter
/**
	@param img Source Image. 
	At the end of function, img is modified and contains the result
	@param h h parameter of filter
	
	Warning: potential overflow problem due to type limitation (when using U8).
**/

template <class T>
void hMaxFilter(Image <T> &img, FlatSE &se, int h)
{
	Image <T> mask=img;
	typename Image<T>::iterator it;
	typename Image<T>::iterator end=img.end();
	for(it=img.begin(); it!=end ; ++it)
		*it-=h;
	geodesicReconstructionByDilation(img,mask,se);
}

/*@}*/


/** \defgroup interval Interval Operators
	\ingroup Morpho
**/

/*@{*/

///Hit-or-miss difference Image
/**
	@param im The source image (not modified)
	@param seA The first (foreground) structuring element
	@param seB The second (background) structuring element
	@return An image of type <int> being the arithmetic difference:
	\f$I=(im\ominus seA)-(im\oplus seB) \f$ 
**/

//Be warned: seB is symmetrized in the function (according to definition)
//Functions use erosionBorderMin: when template hits the border, no detection occurs
//First version: returns an image <int> being the aritmetic difference between erosion and dilation
//Resulting image can be used afterwards to search the most responding point to the operator

template <class T>
Image <int> hitOrMissDifferenceImage(Image <T> &im, FlatSE &seA, FlatSE &seB)
{
	Image <T> eroA=im;
	Image <T> dilB=im;
	
	FlatSE seBSym=seB;
	seBSym.makeSymmetric();
	
	eroA=erosionBorderMin(im,seA);
	dilB=dilationBorderMax(im,seBSym);
		
	Image <int> res(im.getSize());
	
	typename Image<T>::iterator itRes;
	typename Image<T>::iterator endRes=im.end();
	
	typename Image<T>::iterator itEro=eroA.begin();
	typename Image<T>::iterator itDil=dilB.begin();
	
	int minValue=std::numeric_limits<int>::min();
	
	for(itRes=im.begin(); itRes!=endRes; ++itRes,++itEro,++itDil)
		{
		if(*itEro!= minValue)
			*itRes=(int)*itEro-(int)*itDil;
		else *itRes=*itEro;
		}
		
	return res;
}

/// Maximum of the hitOrMissDifferenceImage
/**
	This function returns the maximum (scalar )of the hit-or-miss difference image:
	\f$I=(im\ominus seA)-(im\oplus seB) \f$ 
**/
template <class T>
int hitOrMissMaximumDifference(Image <T> im, FlatSE &seA, FlatSE &seB)
{
	Image <T> eroA=im;
	Image <T> dilB=im;
	
	FlatSE seBSym=seB;
	seBSym.makeSymmetric();
	
	eroA=erosionBorderMin(im,seA);
	dilB=dilation(im,seBSym);
	
	typename Image<T>::iterator itRes;
	typename Image<T>::iterator endRes=im.end();
	
	typename Image<int>::iterator itEro=eroA.begin();
	typename Image<int>::iterator itDil=dilB.begin();
	
	int currentMax=std::numeric_limits<int>::min();
	
	for(itRes=im.begin(); itRes!=endRes; ++itRes,++itEro,++itDil)
		{
		int diff=*itEro-*itDil;
		
		if(diff>currentMax) currentMax= diff;
		}
	
	return currentMax;
}


///Grey-level hit-or-miss: Soille's version
/**
	This function implements the Soille's version of the grey-level hit-or-miss.
**/

template <class T>
Image <T> hitOrMissIntegralK(Image <T> &im, FlatSE &seA, FlatSE &seB)
{
	Image <T> res(im.getSize());
	Image <T> eroA(im.getSize());
	Image <T> dilB(im.getSize());
	
	FlatSE seBSym=seB;
	seBSym.makeSymmetric();
	
	eroA=erosion(im,seA);
	dilB=dilation(im,seBSym);
	
	typename Image<T>::iterator itRes;
	typename Image<T>::iterator endRes=res.end();
	
	typename Image<T>::iterator itEro=eroA.begin();
	typename Image<T>::iterator itDil=dilB.begin();
	
	int currentMax=std::numeric_limits<int>::min();
	
	for(itRes=res.begin(); itRes!=endRes; ++itRes,++itEro,++itDil)
		{
		int diff=(int)*itEro-(int)*itDil;
		
		if(diff > 0) *itRes=diff;
		else {*itRes=T(0); }
		
		if(diff>currentMax) currentMax= diff;
		}
	//std::cout <<"Integral HMT: Max difference: " << currentMax << "\n";
	return res;
}

///Grey-level hit-or-miss: Ronse's version
/**
	This function implements the Ronse's version of the grey-level hit-or-miss.
**/

template <class T>
Image <T> hitOrMissSupremalH(Image <T> &im, FlatSE &seA, FlatSE &seB)
{
	Image <T> res(im.getSize());
	Image <T> eroA(im.getSize());
	Image <T> dilB(im.getSize());
	
	FlatSE seBSym=seB;
	seBSym.makeSymmetric();
	
	eroA=erosion(im,seA);
	dilB=dilation(im,seBSym);
	
	//eroA.save("eroA.pgm");
	//dilB.save("dilB.pgm");
	
	T minValue=std::numeric_limits<T>::min();
	
	typename Image<T>::iterator itRes;
	typename Image<T>::iterator endRes=res.end();
	
	typename Image<T>::iterator itEro=eroA.begin();
	typename Image<T>::iterator itDil=dilB.begin();
	
	int currentMax=std::numeric_limits<int>::min();
	
	for(itRes=res.begin(); itRes!=endRes; ++itRes,++itEro,++itDil)
		{
		int diff=(int)*itEro-*itDil;
		
		if(diff >= 0) *itRes=*itEro;
		else {*itRes=minValue; }
		
		if(diff>currentMax) currentMax= diff;
		}
	//std::cout <<"Supremal H  HMT: Max difference: " << currentMax << "\n";
	
	return res;
}

///Supremal K version of grey-level hit-or-miss
/**
	\note For details, see article: 
	B.Naegel N.Passat C.Ronse.Grey-level hit-or-miss transforms - Part I: Unified theory
	Pattern Recognition, In Press.
**/
template <class T>
Image <T> hitOrMissSupremalK(Image <T> &im, FlatSE &seA, FlatSE &seB)
{
	Image <T> res(im.getSize());
	Image <T> eroA(im.getSize());
	Image <T> dilB(im.getSize());
	
	FlatSE seBSym=seB;
	seBSym.makeSymmetric();
	
	eroA=erosion(im,seA);
	dilB=dilation(im,seBSym);
	
	//eroA.save("eroA.pgm");
	//dilB.save("dilB.pgm");
	
	T minValue=std::numeric_limits<T>::min();
	
	typename Image<T>::iterator itRes;
	typename Image<T>::iterator endRes=res.end();
	
	typename Image<T>::iterator itEro=eroA.begin();
	typename Image<T>::iterator itDil=dilB.begin();
	
	int currentMax=std::numeric_limits<int>::min();
	
	for(itRes=res.begin(); itRes!=endRes; ++itRes,++itEro,++itDil)
		{
		int diff=(int)*itEro-*itDil;
		
		if(diff > 0) *itRes=*itEro;
		else {*itRes=minValue; }
		
		if(diff>currentMax) currentMax= diff;
		}
	//std::cout <<"Supremal K HMT: Max difference: " << currentMax << "\n";
	
	return res;
}


///Morphological probing (Barat et al)
// template <class VoxelType>
// void hitOrMissProbing(GImage <VoxelType> &im, GFlatSE &seA)
// {
// 	GImage <int> eroA=im;
// 	GImage <int> dilB=im;
// 	
// 	GFlatSE seBSym=seA;
// 	seBSym.makeSymmetric();
// 	
// 	erosionBorderMin(eroA,seA);
// 	dilation(dilB,seBSym);
// 	
// 	GImage <int> res=im;
// 	res=dilB-eroA;
// 	
// 	im=res;
// }

///Grey-level hit-or-miss opening: Soille's version
/**
	\note For details, see article: 
	B.Naegel N.Passat C.Ronse.Grey-level hit-or-miss transforms - Part I: Unified theory
	Pattern Recognition, In Press.
**/

template <class T>
Image <T> hitOrMissIntegralKOpening(Image <T> &im, FlatSE &seA, FlatSE &seB)
{
	Image <T> res(im.getSize() );
	res=hitOrMissIntegralK(im,seA,seB);
	res=dilation(res,seA);
	
	return res;
}

///Grey-level hit-or-miss opening: Supremal K version
/**
	\note For details, see article: 
	B.Naegel N.Passat C.Ronse.Grey-level hit-or-miss transforms - Part I: Unified theory
	Pattern Recognition, In Press.
**/

template <class T>
Image <T> hitOrMissSupremalKOpening(Image <T> &im, FlatSE &seA, FlatSE &seB)
{
	Image <T> res(im.getSize());
	res=hitOrMissSupremalK(im,seA,seB);
	res=dilation(res,seA);
	
	return res;
}

///Grey-level hit-or-miss opening: Ronse's version
/**
	\note For details, see article: 
	B.Naegel N.Passat C.Ronse.Grey-level hit-or-miss transforms - Part I: Unified theory
	Pattern Recognition, In Press.
**/

template <class T>
Image <T> hitOrMissSupremalHOpening(Image <T> &im, FlatSE &seA, FlatSE &seB)
{
	Image <T> res(im.getSize());
	res=hitOrMissSupremalH(im,seA,seB);
	res=dilation(res,seA);
	
	return res;
}

/*@}*/
} //namespace


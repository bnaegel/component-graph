//Copyright (C) 2012, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.

#include <assert.h>

namespace LibTIM {

static int getOffset(int x,  int y, int z,
			         int tx, int ty)
{
	return x + y*tx + z*tx*ty;
}


template <class VoxelType> 
template <class VoxelType2>
void Image<VoxelType>::setImageInfos(Image <VoxelType2> &im)
{
	setVx(im.getVx());
	setVy(im.getVy());
	setVz(im.getVz());
	
	setSize(im.getSize());
}


template <class T> 
Image<T>::Image(const TSize *size)
{
	for (int i = 0; i < 3; i++)
	{
		this->size[i] = size[i];
	}
	
	for (int i = 0; i < 3; i++)
	{
		this->spacing[i] = 1.0;
	}
	
	this->dataSize=this->size[0]*this->size[1]*this->size[2];
	try {
		this->data = new T [this->dataSize];
		}
	catch (std::exception &e)
		{
		std::cerr << "Image<T>::Image(const TSize *size) : could not allocate buffer : " << e.what() << std::endl;
    	exit(-1);
		}
}


//Default: xSize=1, ySize=1, zSize=1
template <class T> 
Image<T>::Image(const TSize xSize, const TSize ySize, const TSize zSize)
{
	this->size[0] = xSize;
	this->size[1] = ySize;
	this->size[2] = zSize;
	
	for (int i = 0; i < 3; i++)
	{
		this->spacing[i] = 1.0;
	}
	
	this->dataSize=this->size[0]*this->size[1]*this->size[2];
	try {
		this->data = new T [this->dataSize];
		}
	catch (std::exception &e)
		{
		std::cerr << "Image<T>::Image(TSize xSize, TSize ySize, TSize zSize) : could not allocate buffer : " << e.what() << std::endl;
    	exit(-1);
		}
}

///Construct an image from a buffer *data
///Tab data "must" be allocated and large enough (min bufSize) 
template <class T>
Image<T>::Image(const TSize *size, const TSpacing *spacing, const T *data)
{
	for (int i = 0; i < 3; i++) this->size[i] = size[i];
	for (int i = 0; i < 3; i++) this->spacing[i] = spacing[i];
	this->dataSize=this->size[0]*this->size[1]*this->size[2];
	
	try {
		this->data=new T [this->dataSize];
		}
	catch(std::exception & e)
  		{
    	std::cerr << "Image<T>::Image(const TSize *size, const TSpacing *spacing, const T *data) : could not allocate buffer : " << e.what() << std::endl;
    	exit(-1);
  		}
	
	for(int i=0; i<this->dataSize; i++) this->data[i]=data[i];
}

//Copy ctor
template <class T> 
Image<T>::Image(const Image<T> &im)
{
	for (int i = 0; i < 3; i++) this->size[i] = im.size[i];
	for (int i = 0; i < 3; i++) this->spacing[i] = im.spacing[i];
	
	dataSize=im.size[0]*im.size[1]*im.size[2];
	try {
		this->data=new T [this->dataSize];
		}
	catch(std::exception & e)
  		{
    	std::cerr << "Image<T>::Image(const Image<T> &im) : could not allocate buffer : " << e.what() << std::endl;
    	exit(-1);
  		}

	for (int i=0; i<this->dataSize; i++)
		data[i] = im.data[i];
}

//Assignment operator
template <class T>
Image <T> & Image<T>::operator=(const Image <T> &im)
{
	if(this != &im)
		{
		for (int i = 0; i < 3; i++) this->size[i] = im.size[i];
		for (int i = 0; i < 3; i++) this->spacing[i] = im.spacing[i];
		if(this->data != 0)
			{
			delete[] this->data;
			this->data=0;
			}
		this->dataSize=im.size[0]*im.size[1]*im.size[2];
		try {
			this->data=new T [this->dataSize];
			}
		catch(std::exception & e)
  			{
    		std::cerr << "Image <T> & Image<T>::operator=(const Image <T> &im) : could not allocate buffer : " << e.what() << std::endl;
    		exit(-1);
  			}

		for (int i=0; i<this->dataSize; i++)
			this->data[i] = im.data[i];
		}
	return *this;
}


///Type conversion

template<class T>
template <class T2>
Image<T>::Image( const Image<T2> &im)
{
	this->size[0]=im.getSizeX();
	this->size[1]=im.getSizeY();
	this->size[2]=im.getSizeZ();
	
	this->spacing[0]=im.getSpacingX();
	this->spacing[1]=im.getSpacingY();
	this->spacing[2]=im.getSpacingZ();
	
	this->dataSize=this->size[0]*this->size[1]*this->size[2];
	try {
		this->data=new T [this->dataSize];
		}
	catch(std::exception & e)
  		{
    	std::cerr << "Image<T>::Image( Image<T2> &im): could not allocate buffer : " << e.what() << std::endl;
    	exit(-1);
  		}
	
	for (int i=0; i < this->dataSize; i++)
		this->data[i] = static_cast<T> (im(i));
}

///Image operators 

template <class T>
Image <T> &Image<T>::operator+=(Image <T> &op)
{
	for(int i=0; i<dataSize; i++)
		this->data[i]+=op(i);
	return *this;
}

template <class T>
Image <T> &Image<T>::operator-=(Image <T> &op)
{
	for(int i=0; i<dataSize; i++)
		this->data[i]-=op(i);
	return *this;
}

template <class T>
Image <T> &Image<T>::operator*=(Image <T> &op)
{
	for(int i=0; i<dataSize; i++)
		this->data[i]*=op(i);
	return *this;
}

template <class T>
Image <T> &Image<T>::operator&=(Image <T> &op)
{
	for(int i=0; i<dataSize; i++)
		this->data[i]=std::min(this->data[i],op(i));
	return *this;
}

template <class T>
Image <T> &Image<T>::operator|=(Image <T> &op)
{
	for(int i=0; i<dataSize; i++)
		this->data[i]=std::max(this->data[i],op(i));
	return *this;
}

template <class T>
Image <T> &Image<T>::operator!()
{
	T max=std::numeric_limits<T>::max();
	T min=std::numeric_limits<T>::min();
	
	for(int i=0; i<this->dataSize; i++)
		this->data[i]=max+min-this->data[i];
	return *this;
}

/// Min and max
template <class T>
T Image<T>::getMax(void) const
{ 
T max=std::numeric_limits<T>::min(); 
for (int i=0;i<dataSize; i++) 
	if(this->data[i]>max) 
		max=this->data[i]; 
return max;
}

template <class T>
T Image<T>::getMin(void) const
{ 
T min=std::numeric_limits<T>::max(); 
for (int i=0;i<this->dataSize; i++) 
	if(this->data[i]<min) 
		min=this->data[i]; 
return min;
}

///Image misc

template <class T> 
void Image<T>::fill(const T value)
{
	for(int i=0; i<this->dataSize; i++)
		this->data[i]=value;
}


template <class T> 
Image<T>  Image<T>::crop(const TCoord fromX, const TCoord toX, 
					const TCoord fromY, const TCoord toY, 
					const TCoord fromZ, const TCoord toZ)
{
	TCoord dx=toX-fromX;
	TCoord dy=toY-fromY;
	TCoord dz=toZ-fromZ;
	
	assert(dx>=0 && dy>=0 && dz>=0);
	
	Image <T> res(dx,dy,dz);
	T min=std::numeric_limits<T>::min();
	res.fill(min);
	for(TCoord z=fromZ; z<toZ; z++)
		for(TCoord y=fromY; y<toY; y++)
			for(TCoord x=fromX; x<toX; x++)
				{
				if(this->isPosValid(x,y,z) )
					res(x-fromX,y-fromY,z-fromZ)=(*this)(x,y,z);
				}
	return res;
}
 
// template <> 
// inline GImage<RGB>  GImage<RGB>::crop(int fromX, int toX, int fromY, int toY, int fromZ, int toZ)
// {
// 	int dx=toX-fromX;
// 	int dy=toY-fromY;
// 	int dz=toZ-fromZ;
// 	assert(dx>=0 && dy>=0 && dz>=0);
// 	
// 	GImage <RGB> res(dx,dy,dz);
// 	//VoxelType min=std::numeric_limits<VoxelType>::min();
// 	//res.fill(min);
// 	for(int z=fromZ; z<toZ; z++)
// 		for(int y=fromY; y<toY; y++)
// 			for(int x=fromX; x<toX; x++)
// 				{
// 				if(this->isPosValid(x,y,z) )
// 					{
// 					res(x-fromX,y-fromY,z-fromZ)[0]=(*this)(x,y,z)[0];
// 					res(x-fromX,y-fromY,z-fromZ)[1]=(*this)(x,y,z)[1];
// 					res(x-fromX,y-fromY,z-fromZ)[2]=(*this)(x,y,z)[2];
// 					}
// 				}
// 	return res;
// }


template <class VoxelType> 
void Image<VoxelType>::copy(Image<VoxelType> &im,
 			                 int x1, int y1, int z1,
		                     int x2, int y2, int z2,
			                 int px, int py, int pz)
{
	/*
	assert( im.isPosValid(x1,y1,z1) );
	assert( im.isPosValid(x2,y2,z2) );

	assert( isPosValid(px + x2 - x1,
		               py + y2 - y1,
		               pz + z2 - z1) );
*/
	for (int x = 0; x <= x2-x1; x++)
	{
		for (int y = 0; y <= y2-y1; y++)
		{
			for (int z = 0; z <= z2-z1; z++)
			{				
				this->operator()(px + x, py + y, pz + z)=im(x + x1, y + y1, z + z1);
			}
		}
	}
}

template <class VoxelType> 
void Image<VoxelType>::copyFast(Image<VoxelType> &im,
 			                 int x1, int y1, int z1,
		                     int x2, int y2, int z2,
			                 int px, int py, int pz)
{
	/*
	assert( im.isPosValid(x1,y1,z1) );
	assert( im.isPosValid(x2,y2,z2) );

	assert( isPosValid(px + x2 - x1,
		               py + y2 - y1,
		               pz + z2 - z1) );
*/
	typename Image<VoxelType>::iteratorXYZ it;
	typename Image<VoxelType>::iteratorXYZ end=this->end();
	for (it=this->begin(); it!=end; ++it)
		this->operator()(px + it.x, py + it.y, pz + it.z)=im(it.x + x1, it.y + y1, it.z + z1);
}

template <class VoxelType> 
void Image<VoxelType>::copyFast(Image<VoxelType> &im,
			                 TCoord px, TCoord py, TCoord pz)
{
	TSize *taille = im.getSize();
	copyFast(im, 0,0,0, taille[0]-1,taille[1]-1,taille[2]-1, px,py,pz);
}


template <class VoxelType> 
void Image<VoxelType>::copy(Image<VoxelType> &im,
			                 TCoord px, TCoord py, TCoord pz)
{
	const TSize *taille = im.getSize();
	copy(im, 0,0,0, taille[0]-1,taille[1]-1,taille[2]-1, px,py,pz);
}

template <class VoxelType> 
Image<VoxelType> Image<VoxelType>::getReflection()
{
	Image<VoxelType> im(*this);

	int *taille = getSize();

	for (int x = 0; x < taille[0]; x++)
	{
		for (int y = 0; y < taille[1]; y++)
		{
			for (int z = 0; z < taille[2]; z++)
			{
				im(x,y,z)=im(taille[0]-x-1,taille[1]-y-1,taille[2]-z-1);
			}
		}
	}

	return im;
}

template <class VoxelType> 
bool Image<VoxelType>::isPosValid(TCoord x, TCoord y, TCoord z) const 
{
	return (
			(x >= 0) && (x < size[0]) &&
			(y >= 0) && (y < size[1]) &&
			(z >= 0) && (z < size[2])
		   );
}

}

//Copyright (C) 2012, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef Image_h
#define Image_h

#include <iostream>
#include <limits>
#include <vector>
#include <cstdlib>
#include <typeinfo>


#include "Types.h"
#include "Point.h"
#include "OrderedQueue.h"

#define Image_internal_h
#include "ImageIterators.h"
#undef  Image_internal_h


namespace LibTIM {

/** \defgroup DataStructures Data Structures
**/

/** \defgroup Image
	\ingroup  DataStructures
**/

/*@{*/

///Container base for images of generic type T in LibTIM
/** This structure represents the base image class of LibTIM.
	It contains a buffer of elements of type T of size size[0]*size[1]*size[2]
	x,y and z voxels spacings are recorded in spacing[]
	\par Accessing elements
	To access element (x,y,z) or offset (q), use:
	\verbatim val=im(x,y,z) \endverbatim
	\verbatim val=im(q) \endverbatim
	To write element (x,y,z) or offset (q), use:
	\verbatim im(x,y,z)=val \endverbatim
	\verbatim im(q)=val \endverbatim

	\par Iterators
	To iterate through elements, you should use iterators.
	There is two types of iterator:
		\li \verbatim iterator \endverbatim to scan image in raster scan
		\li \verbatim reverse_iterator \endverbatim to scan image in anti-raster scan
		\li \verbatim iteratorXYZ \endverbatim to scan image in raster scan with the
		knowledge of coordinates
		\li \verbatim reverse_iteratorXYZ \endverbatim to scan image in anti-raster scan with
		the knowledge of coordinates

	begin() and end() methods return iterators on the beginning and the end of the Image
	rbegin() and rend() methods are used with reverse_iterators
	\par Example:
	This piece of code initialized all elements of Image to 0.
	(note that to do this you should better use the fill method)
	\verbatim
	Image <U8> im;
	...
	Image<U8>::iterator it;
	Image<U8>::iterator end=im.end();
	for(it=im.begin(); it!=end; ++it)
		*it=0
	\endverbatim

	Same thing with coordinates access (slower):

	\verbatim
	Image <U8> im;
	...
	Image<U8>::iteratorXYZ it;
	Image<U8>::iteratorXYZ end=im.end();
	for(it=im.begin(); it!=end; ++it)
		im(it.x,it.y,it.z)=0
	\endverbatim

	Note that (as in these examples) it is faster to put im.end() in a variable.

**/

template <class T>
class Image
{
	T *data;
	TSize size [3];
	TSpacing spacing [3];
	TOffset dataSize;

public:

	///Image file loader for 2D images
	/*! Use as follows:
	  \verbatim
	  Image <U8> myIm;
	  Image <U8>::load("myFile.pgm", myIm);
	  \endverbatim
	!*/
	static int load(const char *filename, Image <T> &im);

	///Image file loader for 3D images (Inrimage format: 'inr.gz' extension)
	/*! Use as follows:
	  \verbatim
	  Image <U8> myIm;
	  Image <U8>::load("myFile.inr.gz", myIm);
	  \endverbatim
	!*/
	static int loadInrGz(const char *filename, Image <T> &im);

	///Save image file
	int save(const char * filename);
	int saveInrGz(const char *filename);

	///Constructors
	Image(const TSize *size);
	Image(const TSize xSize=1, const TSize ySize=1, const TSize zSize=1);
	Image(const TSize *size, const TSpacing *spacing, const T *data);

	///Destructor (delete the buffer)
	~Image() { if(this->data!=0) delete[] this->data; data=0;}

	///Copy constructor
	Image(const Image<T> &im);

	///Assignment operator
	Image <T> & operator=(const Image <T> &im);

	///Type conversion constructor
	template <class T2> Image(const Image<T2> &im);

	const TSize *getSize() const {return size;}
	const TSize &getSizeX() const {return size[0];}
	const TSize &getSizeY() const {return size[1];}
	const TSize &getSizeZ() const {return size[2];}

	void setSize(const TSize *size)
			{
			this->size[0]=size[0];
			this->size[1]=size[1];
			this->size[2]=size[2];
			this->dataSize=this->size[0]*this->size[1]*this->size[2];
			if(this->data != 0)
				{
				delete[] this->data;
				this->data=0;
				}

			try {
				this->data = new T [this->dataSize];
				}
			catch(std::exception & e)
  				{
    			std::cerr << "Image::Image(...) : could not allocate buffer : " << e.what() << std::endl;
    			exit(-1);
  				}
			}
	void setSize(TSize x, TSize y, TSize z)
			{
			this->size[0]=x;
			this->size[1]=y;
			this->size[2]=z;
			this->dataSize=this->size[0]*this->size[1]*this->size[2];
			if(this->data != 0)
				{
				delete[] this->data;
				this->data=0;
				}
			try {
				this->data = new T [this->dataSize];
				}
			catch(std::exception & e)
  				{
    			std::cerr << "Image::Image(...) : could not allocate buffer : " << e.what() << std::endl;
    			exit(-1);
  				}
			}

	const TSpacing *getSpacing() const {return spacing;}
	const TSpacing &getSpacingX() const {return spacing[0];}
	const TSpacing &getSpacingY() const {return spacing[1];}
	const TSpacing &getSpacingZ() const {return spacing[2];}

	void setSpacingX(const TSpacing &vx) {spacing[0]=vx;}
	void setSpacingY(const TSpacing &vy) {spacing[1]=vy;}
	void setSpacingZ(const TSpacing &vz) {spacing[2]=vz;}
	void setSpacing(const TSpacing &vx=1.0, const TSpacing &vy=1.0, const TSpacing &vz=1.0)
		{
		spacing[0]=vx; spacing[1]=vy;  spacing[2]=vz;
		}

	const TOffset &getBufSize() const {return dataSize;}

	inline  T *getData() {return this->data;}

	///Iterators
	typedef ImageIterator <Image,T> iterator;
	typedef ImageIterator <const Image, const T> const_iterator;

	typedef ImageIteratorXYZ <Image,T> iteratorXYZ;
	typedef ImageIteratorXYZ <const Image, const T> const_iteratorXYZ;

	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
	typedef std::reverse_iterator<iterator>       reverse_iterator;

	inline iterator begin() {return iterator(this,this->data);}
	inline const_iterator begin() const {return const_iterator(this,this->data);}

	inline iterator end() {return iterator(this,this->data+this->dataSize);}
	inline const_iterator end() const {return const_iterator(this,this->data+this->dataSize);}

	inline       reverse_iterator rbegin()         { return       reverse_iterator(end()); }
	inline const_reverse_iterator rbegin() const   { return const_reverse_iterator(end()); }

	inline       reverse_iterator rend()           { return       reverse_iterator(begin()); }
	inline const_reverse_iterator rend() const     { return const_reverse_iterator(begin()); }

	///Unsafe data accessors

	///Coordinates write version
	inline T &operator()(TCoord x, TCoord y, TCoord z=0) {return data[x + y*size[0] + z*size[0]*size[1]];}

	///Coordinates read-only version
	inline T operator()(TCoord x, TCoord y, TCoord z=0) const {return data[x + y*size[0] + z*size[0]*size[1]];}

	///Offset write version
	inline T &operator()(TOffset offset) {return data[offset];}

	///Offset read-only version
	inline T operator()(TOffset offset) const {return data[offset];}

	///Point write version
	inline T &operator()(Point <TCoord> p) {return data[p.x + p.y*size[0] + p.z*size[0]*size[1]];}

	///Point read-only version
	inline T operator()(Point <TCoord> p) const {return data[p.x + p.y*size[0] + p.z*size[0]*size[1]];}

	///Operators overloading

	Image& operator+=(Image <T> &op);
	Image& operator-=(Image <T> &op);
	Image& operator*=(Image <T> &op);
	Image& operator/=(Image <T> &op);

	///Pointwise minimum and maximum
	Image& operator&=(Image <T> &op);
	Image& operator|=(Image <T> &op);

	///Negative
	Image& operator!(void);

	bool operator==(Image <T> &op)
		{
		typename Image<T>::iterator it;
		typename Image<T>::iterator itOp=op.begin();
		typename Image<T>::iterator end=this->end();
		for(it=this->begin(); it!=end; ++it,++itOp)
			if(*it!=*itOp)
				return false;
		return true;
		}

	bool operator!=(Image <T> &op)
		{
		return !operator==(op);
		}

	///
	template <class T2> void setImageInfos(Image <T2> &im);

	///Return image min and max
	/// Warning: scan the image each time
	T getMax() const;
	T getMin() const;

	//Image utilities
	void fill(const T value);

	Image<T> crop( const TCoord fromX=0, const TCoord toX=1,
					const TCoord fromY=0, const TCoord toY=1,
					const TCoord fromZ=0, const TCoord toZ=1);

	void copy(Image<T> &im,
			  TCoord x1, TCoord y1, TCoord z1,
		      TCoord x2, TCoord y2, TCoord z2,
			  TCoord px, TCoord py, TCoord pz);

	void copyFast(Image<T> &im,
 			                 int x1, int y1, int z1,
		                     int x2, int y2, int z2,
			                 int px, int py, int pz);
	void copyFast(Image<T> &im,	 TCoord px, TCoord py, TCoord pz);
	void copy(Image<T> &im,
			  TCoord px, TCoord py, TCoord pz);

	void enlarge();

	int getOffset(TCoord x, TCoord y=0, TCoord z=0) {return x+y*size[0]+z*size[0]*size[1];}

	int getOffset(Point <TCoord> p) {return p.x+p.y*size[0]+p.z*size[0]*size[1];}

	const Point<TCoord> getCoord  (TOffset offset) const {
		Point <TCoord> res;
		res.z=offset/(getSizeX()*getSizeY());
		res.y=(offset%(getSizeX()*getSizeY()))/getSizeX();
		res.x=offset % getSizeX();
		return res;
		}

	Image<T> getReflection();

	void print()
		{
		for (int y=0; y<getSizeY(); y++)
			{
			for(int x=0; x<getSizeX(); x++)
				std::cout << (int) operator()(x,y,0)<< "\t";
			std::cout << "\n";
			}
		std::cout << "\n";
		}

	bool isPosValid  (TCoord x, TCoord y, TCoord z=0) const ;
	bool isPosValid  (TOffset offset) const {return (offset>=0) && (offset<dataSize) ; };
	bool isPosValid(Point <TCoord> p) const {return ((p.x >= 0) && (p.x < size[0]) && (p.y >= 0) && (p.y < size[1])
	&& (p.z >= 0) && (p.z < size[2]));};


};

///Image operators
template <class T>
Image <T> operator+(Image <T> &a, Image <T> &b)
{
	Image <T> res=a;
	res+=b;
	return res;
}

template <class T>
Image <T> operator-(Image <T> &a, Image <T> &b)
{
	Image <T> res=a;
	res-=b;
	return res;
}

template <class T>
Image <T> operator*(Image <T> &a, Image <T> &b)
{
	Image <T> res=a;
	res*=b;
	return res;
}

///Mixed mode arithmetic: operations with a scalar

template <class T>
Image <T> operator+(Image <T> &a, T s)
{
	Image <T> res=a;
	for(int i=0; i<a.getBufSize(); i++)
		res(i)+=s;
	return res;
}

template <class T>
Image <T> operator-(Image <T> &a, T s)
{
	Image <T> res=a;
	for(int i=0; i<a.getBufSize(); i++)
		res(i)-=s;
	return res;
}

template <class T>
Image <T> operator*(Image <T> &a, T s)
{
	Image <T> res=a;
	for(int i=0; i<a.getBufSize(); i++)
		res(i)*=s;
	return res;
}

/*@]*/

}

#include "Image.hxx"
#include "ImageIO.hxx"

#ifdef SUPPORT_INRIMAGE
#include "ImageIO_InrGz.hxx"
#endif

#endif

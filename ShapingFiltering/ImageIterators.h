//Copyright (C) 2012, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.


#ifndef Image_internal_h
#error ImageIterators should only be included by Image.h
#endif

#include "Image.h"


namespace LibTIM {

template <class TImage, class T> 
class ImageIterator: public std::iterator <std::forward_iterator_tag, T>
	{
	public:
	T *ptr;
	TImage *im;
	
	public:
		ImageIterator() {}
		inline ImageIterator(TImage *im, T *x) {
			this->im=im;
			this->ptr=x;
			}
	
	inline T &operator*() {return *ptr;}
	inline T *operator->() {return ptr;}
	//++it
	inline ImageIterator<TImage, T> &operator++() {ptr++; return *this;}
	//it++
	inline ImageIterator<TImage, T>  operator++(int)
		{
		ImageIterator<TImage, T> tmp=*this; 
		ptr++;return tmp;
		}
	inline bool operator==(const ImageIterator &x) {return ptr==x.ptr;}
	inline bool operator!=(const ImageIterator &x) {return ptr!=x.ptr;}
		
	};

template <class TImage,class T> 
class ImageIteratorXYZ: public ImageIterator <TImage,T>
	{
	
	public:
	TCoord x;
	TCoord y;
	TCoord z;
	
	ImageIteratorXYZ()  {}
	ImageIteratorXYZ(T *x){}
	inline void operator=(const ImageIterator <TImage,T> &other)
		 {
		 this->ptr=other.ptr;
		 this->im=other.im;
		
		 if(this->ptr==this->im->getData() )
		 	{
			x=0;
			y=0;
			z=0;
			}
		 }
	inline ImageIteratorXYZ(const ImageIterator <TImage,T> &other)
		{
		operator=(other);
		}
		
	
	inline T &operator*() {return *(this->ptr);}
	inline T *operator->() {return this->ptr;}
	
	inline ImageIteratorXYZ<TImage, T> &operator++() {
		this->ptr++; 
		this->x++;
		if(this->x == this->im->getSizeX())
			{
			this->x=0;
			this->y++;
			}
		if(this->y == this->im->getSizeY())
			{
			x=0;
			y=0;
			z++;
			}
		return *this;
		}
	inline ImageIteratorXYZ<TImage, T>  operator++(int) {
		ImageIteratorXYZ tmp=*this;
		++(*this);
		return tmp;
		}
	};
};

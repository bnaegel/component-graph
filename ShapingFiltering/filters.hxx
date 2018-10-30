//Copyright (C) 2018, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.

#include "Image.h"
#include <cmath>


template <class T>
Image <double> convolve(const Image<T> &image, const Image<double> &mask){
	int dx=image.getSizeX(); int dy=image.getSizeY();
	int nx=mask.getSizeX();  int ny=mask.getSizeY();
	int cx=floor(nx/2);   int cy=floor(ny/2);

	Image<double> result(dx,dy);

	double sum;

	for(int y=0; y<dy; ++y) {
		for(int x=0; x<dx; ++x) {
			sum=0.0;
			for(int j=0; j<ny; ++j) {
				for(int i=0; i<nx; ++i) {
					if(x-i+cx >= 0 && x-i+cx <dx && y-j+cy >= 0 && y-j+cy<dy) {
						sum+=image(x-i+cx,y-j+cy)*mask(i,j);
					}
				}
			}
			result(x,y)=sum;	
		}
	}
	return result;
}

template <class T>
Image <U8> gradientSobel(const Image<T> &image)
{
	Image <double> SobelX(3,1);
	Image <double> SobelY(1,3);
	

	SobelX(0,0)=1; SobelX(1,0)=0; SobelX(2,0)=-1;
	SobelY(0,0)=1; SobelY(0,1)=2; SobelY(0,2)=1;

	Image <double> sx=convolve(image,SobelX);
	Image <double> sy=convolve(sx,SobelY);

	Image <double> gradient(image.getSizeX(),image.getSizeY());

	for(int i=0; i<sy.getBufSize(); ++i) {
		sy(i)=sy(i) < 0 ? -sy(i) : sy(i);
		sy(i)/=4;
	}

	Image <double> resultX=sy;


	SobelX(0,0)=1; SobelX(1,0)=2; SobelX(2,0)=1;
	SobelY(0,0)=1; SobelY(0,1)=0; SobelY(0,2)=-1;

	sx=convolve(image,SobelX);
	sy=convolve(sx,SobelY);

	for(int i=0; i<sy.getBufSize(); ++i) {
		sy(i)=sy(i) < 0 ? -sy(i) : sy(i);
		sy(i)/=4;
	}

	Image <double> resultY=sy;

	for(int i=0; i<gradient.getBufSize(); ++i) {
		gradient(i)=sqrt(resultX(i)*resultX(i)+resultY(i)*resultY(i));
	}
	Image<U8> result=gradient;

	return result;
}
 
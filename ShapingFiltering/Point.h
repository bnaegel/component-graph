//Copyright (C) 2012, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef Point_h
#define Point_h
// ----------------------------------

namespace LibTIM {

/** \defgroup Point Point 
	\ingroup DataStructures 
**/

/*@{*/

/// Point Structure
/** Basic structure to manipulate 3D points of type T
**/

template <class T>
struct Point
{
	T x, y, z;
	
	Point(T x=0, T y=0, T z=0) {this->x=x; this->y=y; this->z=z;}
	Point& operator+=(Point <T> q) {x+=q.x; y+=q.y; z+=q.z; return *this;}
	Point& operator-=(Point <T> q) {x-=q.x; y-=q.y; z-=q.z; return *this;}
	bool operator==(Point <T> q) {return (x==q.x && y== q.y && z==q.z);}
	
	void operator=(const Point <T> &q) {
		this->x=q.x;
		this->y=q.y;
		this->z=q.z;
		}
	
	Point(const Point &q) {operator=(q);}
	void operator()(T x, T y, T z) {this->x=x; this->y=y; this->z=z; }
	
	void print() {std::cout << x << " " << y << " " << z <<"\n";}
};

template <class T>
Point<T> operator+(Point <T> p, Point <T> q)
{
	Point <T> r=p;
	return r+=q;
}

template <class T>
Point<T> operator-(Point <T> p, Point <T> q)
{
	Point <T> r=p;
	return r-=q;
}

/*@}*/

}

#endif

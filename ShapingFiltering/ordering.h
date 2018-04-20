//Copyright (C) 2014, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.


#ifndef ORDERING_H
#define ORDERING_H

#include <algorithm>
#include "Types.h"
#include "tuple.h"

using namespace LibTIM;

template <class T>
class Ordering
{
 public:
         inline virtual bool islessequal(T &v, T &w)=0;
         inline virtual bool isequal(T &v, T &w)=0;
         inline virtual int getPriority(T &v)=0;
};

class ColorMarginalOrdering : public Ordering<RGB> {
    public:
          inline bool islessequal(RGB &v, RGB &w) {
            if(v[0]<=w[0] && v[1] <= w[1] && v[2]<=w[2])
                return true;
            else return false;
         }

         inline bool isequal(RGB &v, RGB &w) {
            if(v[0] == w[0] && v[1]==w[1] && v[2]==w[2]) return true;
            else return false;
         }

         inline int getPriority(RGB &v) {
            return -(v[0]+v[1]+v[2]);
         }
};

class ColorMarginalInverseOrdering : public Ordering<RGB> {
    public:
          bool islessequal(RGB &v, RGB &w) {
            if(v[0]>=w[0] && v[1] >= w[1] && v[2]>=w[2])
                return true;
            else return false;
         }

         bool isequal(RGB &v, RGB &w) {
            if(v[0] == w[0] && v[1]==w[1] && v[2]==w[2]) return true;
         else return false;
         }

          int getPriority(RGB &v) {
            return (v[0]+v[1]+v[2]);
         }
};


class ColorSatLumOrdering : public Ordering<RGB> {
     double getS(RGB &v) {
            U8 maxi=std::max(std::max(v[0],v[1]),v[2]);
            U8 mini=std::min(std::min(v[0],v[1]),v[2]);
        if(maxi!=0)
            return (255.0*(maxi-mini)/maxi);
        else return 0;
     }
     double getV(RGB &v) {
        return std::max(std::max(v[0],v[1]),v[2]);
     }

    public:

          bool islessequal(RGB &v, RGB &w) {

            if(getS(v)<=getS(w))
                return true;
            else return false;
         }

         bool isequal(RGB &v, RGB &w) {
            //if(v[0] == w[0] && v[1]==w[1] && v[2]==w[2]) return true;
            if(getS(v)==getV(v)) return true;
         else return false;
         }

          int getPriority(RGB &v) {
            return -(getS(v)+getV(v));
         }
};

template <int N>
class TupleMarginalOrdering : public Ordering <Tuple <int, N> >{
public:
    inline bool islessequal(Tuple <int,N> &v, Tuple <int,N> &w) {

        for(int i=0; i<N; ++i) {
            if(v[i]>w[i])
                return false;
        }
        return true;
    }

    inline bool isequal(Tuple <int,N> &v, Tuple <int,N> &w) {
        for(int i=0; i<N; ++i) {
            if(v[i]!=w[i])
                return false;
        }
        return true;
    }

    inline int getPriority(Tuple <int,N> &v) {
        int tmp=0;
        for(int i=0; i<N; ++i){
            tmp+=v[i];
        }
        return -tmp;
    }
};

#endif // COLORORDERING_H

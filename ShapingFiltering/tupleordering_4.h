//Copyright (C) 2018, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.


#ifndef TUPLEORDERING_4_H
#define TUPLEORDERING_4_H

#include <algorithm>
#include "Types.h"

using namespace LibTIM;

class TupleOrdering_4
{
 public:
         inline virtual bool islessequal(Tuple_4 &v, Tuple_4 &w)=0;
         inline virtual bool isequal(Tuple_4 &v, Tuple_4 &w)=0;
         inline virtual int getPriority(Tuple_4 &v)=0;
};

class MarginalOrderingTuple : public TupleOrdering_4 {
    public:
          inline bool islessequal(Tuple_4 &v, Tuple_4 &w) {
            if(v[0]<=w[0] && v[1] <= w[1] && v[2]<=w[2] && v[3]<=w[3] )
                return true;
            else return false;
         }

         inline bool isequal(Tuple_4 &v, Tuple_4 &w) {
            if(v[0] == w[0] && v[1]==w[1] && v[2]==w[2] && v[3]==w[3] ) return true;
            else return false;
         }

         inline int getPriority(Tuple_4 &v) {
            return -(v[0]+v[1]+v[2]+v[3]);
         }
};

#endif //

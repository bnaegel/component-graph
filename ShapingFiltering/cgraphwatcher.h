//Copyright (C) 2014, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef CGRAPHWATCHER_H
#define CGRAPHWATCHER_H

class CGraphWatcher {

    protected:
        int curProgress;
        int finalProgress;
    public:
        virtual void progressUpdate(int ncur, int nfinal)=0;
        virtual void progressUpdate()=0;
    CGraphWatcher(int finalProgress) {
        this->curProgress=0;
        this->finalProgress=finalProgress;
    }
};

#endif // CGRAPHWATCHER_H

//Copyright (C) 2014, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef RAGRAPH_H
#define RAGRAPH_H


#include "Image.h"
#include "FlatSE.h"

using namespace LibTIM;

/** Region adjacency graph **/

class RAGraph {

    Image <RGB> imSource;
    FlatSE connexity;
    Image <TLabel> imFlatZones;

public :
    struct Vertex {
        int index;

        RGB color;

        std::vector <int> compNb;
        std::vector <int> nonCompNb;

        std::vector <int> allNb;  /*!< All neighbors */
        std::vector <Point <TCoord> > pixels;

        Vertex(int n) {
            this->index=n;
        }

        Vertex() {

        }

        void insertCompNb(int index) {

            for(int i=0; i<compNb.size(); i++) {
                if(this->compNb[i]==index) return;
            }
            compNb.push_back(index);
            allNb.push_back(index);
        }

        void insertNonCompNb(int index) {
            for(int i=0; i<nonCompNb.size(); i++) {
                if(nonCompNb[i]==index) return;
            }
            nonCompNb.push_back(index);
            allNb.push_back(index);
        }
    };

    template <class T> void addBorders(Image<T> &im,
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

        im=temp;
    }


    void computeRAGraph();
    void computeFlatZones();

public :
    std::vector <Vertex *> nodes;

    RAGraph(Image <RGB> imSource, FlatSE connexity) {
        this->imSource=imSource;
        this->connexity=connexity;
        computeRAGraph();
    }
    ~RAGraph() {
        for(int i=1; i<nodes.size(); i++) delete nodes[i];
    }

    void writeDot(const char *fileName);
    void print();
};

#endif // RAGRAPH_H

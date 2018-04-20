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
#include "tuple.h"

using namespace LibTIM;

/** Region adjacency graph **/

template <typename T>
struct Rag
{

    struct Vertex
    {
        // index
        int n;
        // value of flat zone
        T value;
        // list of adjacent flat zones
        std::vector<int> edges;

        // list of pixels belonging to flat zone
        std::vector<int> pixels;

        Vertex() = default;

        Vertex(int n, T value) : n(n), value(value) {}

        void addEdge(int index)
        {
            if (std::find(edges.begin(), edges.end(), index) == edges.end())
            {
                edges.push_back(index);
            }
        }
    };

    Image<int> labelImage;
    FlatSE connexity;
    Rag() = default;

    Rag(const Image<int> &labelImage) : labelImage(labelImage) {}
    std::vector<Vertex> vertices;
};

#include "ragraph.hxx"

#endif // RAGRAPH_H

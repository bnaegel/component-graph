//Copyright (C) 2018, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.

#include "ragraph.h"


template <class T>
Image<int> computeFlatZones(const Image<T> &img, const FlatSE &connexity)
{
    int dx = img.getSizeX();
    int dy = img.getSizeY();

    Image<int> labelImage(dx, dy);
    labelImage.fill(-1);

    std::queue<Point<TCoord> > fifo;

    int currentLabel = 0;

    for (int y = 0; y < dy; ++y)
    {
        for (int x = 0; x < dx; ++x)
        {
            if (labelImage(x, y) == -1)
            {
                // begin propagation
                fifo.push(Point<TCoord> (x, y));
                labelImage(x, y) = currentLabel;


                while (not fifo.empty())
                {
                    Point<TCoord> p = fifo.front();
                    fifo.pop();
                    for (int i = 0; i < connexity.getNbPoints(); ++i)
                    {
                        Point<TCoord> q=p+connexity.getPoint(i);

                        if (q.x >= 0 && q.x < dx && q.y >= 0 && q.y < dy)
                        {
                            if (labelImage(q.x, q.y) == -1 && (img(q.x, q.y) == img(p.x, p.y)))
                            {
                                fifo.push(q);
                                labelImage(q.x, q.y) = currentLabel;
                            }
                        }
                    }
                }
                currentLabel++;
            }
        }
    }
    return labelImage;
}



// compute region adjacency graph from flat zones image
template <class T>
Rag<T> computeRag(const Image<T> &img,  const FlatSE &connexity)
{

    int dx = img.getSizeX();
    int dy = img.getSizeY();

    Image<int> labelImage = computeFlatZones(img,connexity);

    Rag<T> rag(labelImage);
    rag.connexity=connexity;

    int max = labelImage(0);
    for (int i = 1; i < labelImage.getBufSize(); ++i)
    {
        max = std::max(max, labelImage(i));
    }

    rag.vertices.resize(max + 1);

    // compute vertices and adjacencies
    for (int y = 0; y < dy; ++y)
    {
        for (int x = 0; x < dx; ++x)
        {
            int index = labelImage(x, y);
            rag.vertices[index].n = index;
            rag.vertices[index].value = img(x, y);
            rag.vertices[index].pixels.push_back(y*dx+x);

            for (int j = 0; j < 8; ++j)
            {
                Point<TCoord> p(x,y);
                Point<TCoord> q=p+connexity.getPoint(j);

                if (q.x >= 0 && q.x < dx && q.y >= 0 && q.y < dy)
                    if (labelImage(q.x, q.y) != index)
                    {
                        rag.vertices[index].addEdge(labelImage(q.x, q.y));
                    }
            }
        }
    }
    return rag;
}

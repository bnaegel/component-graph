//Copyright (C) 2014, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.

#include "ragraph.h"

void RAGraph::computeFlatZones()
{
    imFlatZones.setSize(imSource.getSize());

    FlatSE se=this->connexity;

    std::queue<TOffset> fifo;

    const TCoord *back=se.getNegativeOffsets();
    const TCoord *front=se.getPositiveOffsets();

    TLabel BORDER=-1;

    Image<RGB> imBorder=imSource;
    Image<TLabel> resBorder(imSource.getSize() );
    resBorder.fill(0);

    addBorders(imBorder,back,front,RGB(0));
    addBorders(resBorder,back,front,BORDER);

    se.setContext(imBorder.getSize());

     Image<RGB>::iterator it;
     Image<RGB>::iterator end=imBorder.end();

    FlatSE::iterator itSe;
    FlatSE::iterator endSe=se.end();

    TLabel currentLabel=1;

    TOffset curOffset=0;
    for(it=imBorder.begin(); it!=end; ++it,curOffset++)
        {
        if(resBorder(curOffset) ==0)
            {
            fifo.push(curOffset);

            while(!fifo.empty())
                {
                TOffset p=fifo.front();
                fifo.pop();

                resBorder(p)=currentLabel;

                for(itSe=se.begin(); itSe!=endSe; ++itSe)
                    {
                    TOffset q=p+ *itSe;

                    if(imBorder(q)==imBorder(p) && resBorder(q)==0)
                        {

                        resBorder(q)=currentLabel;
                        fifo.push(q);
                        }
                    }
                }
            currentLabel++;
            }
        }

     Image<TLabel>::iteratorXYZ itLabelXYZ;
     Image<TLabel>::iteratorXYZ endRes=imFlatZones.end();
    for(itLabelXYZ=imFlatZones.begin(); itLabelXYZ!=endRes; ++itLabelXYZ)
        {
        *itLabelXYZ=resBorder(itLabelXYZ.x+back[0], itLabelXYZ.y+back[1], itLabelXYZ.z+back[2]);
        }
}

void RAGraph::computeRAGraph() {

    int dx=imSource.getSizeX();
    int dy=imSource.getSizeY();
    int dz=imSource.getSizeZ();

    computeFlatZones();
    Image<U8> tmp=imFlatZones;
    tmp.save("labels.pgm");
    TLabel nbFlatZones=imFlatZones.getMax();
    // flat-zones are numbered 1,...
    // to achieve simple indexing we reserve nbFlatZones+1 nodes
    // so nodes[FZi] stores the flat-zone numbered i
    nodes.resize(nbFlatZones+1);
    for(int i=1; i<nodes.size(); i++) nodes[i]=new Vertex(i);

    for(int z=0; z<dz; z++)
        for(int y=0; y<dy; y++)
            for(int x=0; x<dx; x++) {

                Point <TCoord> p(x,y,z);
                // add pixel to corresponding node
                nodes[imFlatZones(p)]->pixels.push_back(p);
                nodes[imFlatZones(p)]->color=imSource(p);

                //scan the neighbors
                for(int i=0; i<connexity.getNbPoints(); i++) {
                    Point <TCoord> q=p+connexity.getPoint(i);

                    if(imSource.isPosValid(q)) {
                        if(imSource(p)!=imSource(q)) {
                         Vertex *v=nodes[imFlatZones(x,y,z)];
                         // search for comparable neighbor
                         if(imSource(p)<imSource(q) || imSource(p)>imSource(q)) {
                            v->insertCompNb(imFlatZones(q));

                         }
                         // non-comparable neighbor
                         else
                            v->insertNonCompNb(imFlatZones(q));
                        }
                    }
                }

            }
}


void RAGraph::writeDot(const char *fileName) {



}

void RAGraph::print() {
    for(int i=1; i<nodes.size(); i++) {
        assert(i=nodes[i]->index);
        std::cout << "Node : " << nodes[i]->index << "\n";
        std::cout << "\t Color : (" << (int)nodes[i]->color[0] << "," << (int)nodes[i]->color[1] << "," << (int)nodes[i]->color[2]   << ")\n";
        for(int j=0; j<nodes[i]->compNb.size(); j++) {
            std::cout << "\t ->" << nodes[i]->compNb[j]<< "\n";
        }

        for(int j=0; j<nodes[i]->nonCompNb.size(); j++) {
            std::cout << "\t ~~" << nodes[i]->nonCompNb[j]<< "\n";
        }
    }
}

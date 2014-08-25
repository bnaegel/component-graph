//Copyright (C) 2014, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.


#include <ctime>

#include "cgraph.h"
#include "cgraphwatcher.h"
#include "colorordering.h"

using namespace std;

// for LibTIM classes
using namespace LibTIM;

class graphWatcher : public CGraphWatcher{
    public :
        void progressUpdate(int ncur, int nfinal) {
            if(ncur%100==0) {
                std::cout << ncur << " / " << nfinal << "\r";
            }
        }

        void progressUpdate() {
            curProgress++;
            if(curProgress%100==0) {
                std::cout << this->curProgress << " / " << this->finalProgress << "\n";

            }
        }
    graphWatcher(int finalProgress) : CGraphWatcher(finalProgress) {}
};


/**
* Colour image attribute filtering based on \ddot component-graph on RGB colour space
**/

int main(int argc, char *argv[])
{
    if(argc!=4)
    {
        cout<<"Usage: " << argv[0] << " <source_image.ppm> <area_min> <contrast_min>  \n";
        exit(1);
    }

    // Declaration of:
    // - imSrc (source image)

    Image <RGB> imSrc;

    // Declaration of :
    // -connexity (8-adjacency)
    FlatSE connexity;
    connexity.make2DN8();

    // Load source image imSrc in ppm format, 2D RGB (8 bits)
    Image<RGB>::load(argv[1],imSrc);

    int areaMin=atoi(argv[2]);
    int contrastMin=atoi(argv[3]);

    CGraph *cgraph=new CGraph(imSrc,connexity);

    // Track computation progress
    graphWatcher *myWatcher=new graphWatcher(imSrc.getBufSize());
    // Set marginal ordering on RGB colour space
    ColorMarginalOrdering  *order=new ColorMarginalOrdering();
    // Compute \ddot component-graph
    cgraph->computeGraph(order,myWatcher);

    // Area and contrast filtering
    cgraph->areaFiltering(areaMin);
    cgraph->contrastFiltering(contrastMin);

    // Write graph in dot format
    // Filtered nodes are shown in gray
    cgraph->writeDot("cgraph.dot");

    // Compute resulting image
    Image<RGB> result=cgraph->constructImage(order);
    // Save result in ppm format
    result.save("result.ppm");

    delete cgraph;
}


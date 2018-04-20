//Copyright (C) 2013, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.


#ifndef CGRAPH_H
#define CGRAPH_H

#include <vector>
#include <map>

#include "Image.h"
#include "FlatSE.h"
#include "ragraph.h"
#include "cgraphwatcher.h"
#include "colorordering.h"

using namespace LibTIM;
using namespace std;

/** Component-graph storage and computation **/

class CGraph
{
public:
    /** A node of the directed graph */
    struct Node {
        int index;
        RGB color;
        RGB dispColor;
        Node(int index, RGB color, int area) {this->index=index; this->color=color; this->area=area; active=true;}
        std::vector<Node *> childs;
        std::vector<Node *> fathers;
        // list of flat-zones belonging to node and having same value
        std::vector<int > regions;
        std::vector<int> pixels;
        int area;
        int contrast;
        bool active;

        void addChild(Node *child) {
            this->childs.push_back(child);
            child->fathers.push_back(this);
        }
    };

public :
    RAGraph *rag; /*!< Region adjacency graph on which is computed the component-graph */

    Image<RGB> imSource; /*! Source image */
    FlatSE connexity;   /*!< Underlying connexity (4- or 8-) */

    /** Container of the nodes of the (directed) graph **/
    std::vector<Node *> graph;
    /** Root of the graph **/
    Node *root;

public:

    CGraph(Image <RGB> &imSource, FlatSE &connexity) {
        this->imSource=imSource;
        this->connexity=connexity;
        //std::cout << "Compute RAG\n";
        this->rag=new RAGraph(imSource,connexity);
        //std::cout << "RAG computed\n";
        }
    CGraph(RAGraph *rag) : rag(rag) {}
    ~CGraph() { delete rag;}

    Node *componentGraphNaive(FlatSE &connexity);

    /** Component-graph \ddot G **/
    int computeGraph(ColorOrdering *order, CGraphWatcher *watcher);
    /** Component-graph G **/
    int computeGraphFull(ColorOrdering *order, CGraphWatcher *watcher);

    /** Return synthetic images */
    static Image<RGB> syntheticImage();
    static Image<RGB> syntheticImage2();

    /** Write graph in dot format **/
    int writeDot(const char *filename);

    void areaFiltering(int areaMin);
    void reconstructMin();

    Image<RGB> constructImage(ColorOrdering *order);

    /** Helper functions **/
    vector<CGraph::Node *> computeComponents(ColorOrdering *order, OrderedQueue<RGB> &pq);
    static bool notComparable(Image<RGB> &im, Point<TCoord> &p, Point<TCoord> &q);

    bool isEqual(Node *n, Node *m);
    bool isIncluded(Node *n, Node *m);
    bool isIncludedFast(Node *n, Node *m, vector<bool> &tmp);
    void computeLinks(ColorOrdering *order, vector<Node *> &nodes);
    Node *addRoot(vector<Node *> &nodes);
    vector<Node *> minimalElements(ColorOrdering *order, vector<Node *> &nodes, vector<bool> &tmp);
    void computeTransitiveReduction(ColorOrdering *order, vector<Node *> &nodes);

    int computeGraphInverse(CGraphWatcher *watcher);
    Image<RGB> constructImageInf();
    void areaFiltering(int areaMin, int areaMax);
    bool isLTE(RGB &v, RGB &w);
    Image<RGB> constructImageInverse();

    void contrastFiltering(int contrastMin);
    void contrastFiltering(int contrastMin, int contrastMax);
    void resetFiltering();

    /** Adaptive filtering **/
    void adaptiveAreaFiltering(int p);
    void adaptiveContrastFiltering(int p);

    bool isIncluded(Node *n, Node *m, vector<bool> &tmp);
private:
    void paintNode(Image<RGB> &im, Node *n, RGB &value);
    void paintNodeSup(Image<RGB> &imRes, Node *n, RGB &value);

    vector<Node *> computeComponents(Image<RGB> &im, FlatSE &connexity);
};

#endif // CGRAPH_H

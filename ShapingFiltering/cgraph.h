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
#include "ordering.h"

using namespace LibTIM;
using namespace std;


/** Component-graph storage and computation **/
template <class T>
class CGraph
{
public:
    /** A node of the directed graph */
    struct Node {
        int index;
        T value;
        T dispColor;
        Node(int index, T value, int area=-1) {this->index=index; this->value=value; this->area=area; active=true; active2=true;
       }
        std::vector<Node *> childs;
        std::vector<Node *> fathers;
        // list of flat-zones belonging to node and having same value
        std::vector<int > regions;
        std::vector<int> pixels;
        int area;
        int contrast;
        int compacity;
        int fels_contrast; // contrast using felszenswab strategy (max int - min ext)
        int mean_gradient; // mean gradient along the contour of the node
        int attr; // abstract attribute used in shaping
        
        // Eloise add
//        int elong;
//        int elong2;
//        int flatn;
//        int flatn2;
//        int noncompacity;
//
        // moments (dans chaque direction)
//        long double m100;
//        long double m010;
//		long double m001;
//		long double m110;
//		long double m011;
//		long double m101;
//		long double m200;
//		long double m020;
//		long double m002;
//
		// mu
//		long double mu200;
//		long double mu020;
//		long double mu002;
//		long double mu110;
//		long double mu011;
//		long double mu101;
		
        //moment d'inertie (first Hu's invariant moment)
//        double I;
        
        bool active;
        // if active2==false, the node will not be reconstructed in the result image
        // (used for graph filtering before shaping)
        bool active2;

        void addChild(Node *child) {
            this->childs.push_back(child);
            child->fathers.push_back(this);
        }
    };

public :
    Rag<T> &rag; /*!< Region adjacency graph on which is computed the component-graph */

    Image<T> &imSource; /*! Source image */

    /** Container of the nodes of the graph **/
    std::vector<Node *> graph;
    /** Ordering **/
    Ordering<T> &order;
    /** Root of the graph (i.e. unique node with no fathers) **/
    Node *root;

    CGraph(Image <T> &imSource, Rag<T> &rag, Ordering<T> &order) : imSource(imSource),rag(rag),order(order) {}

    /** Component-graph \ddot G **/
    int computeGraph(CGraphWatcher *watcher);

    /** Compute attributes **/
    int computeAttributes();

    /** Return synthetic images */
    static Image<RGB> syntheticImage();
    static Image<RGB> syntheticImage2();

    /** Write graph in dot format **/
    template <int N>
    int writeDot(const char *filename);
    template <int N>
    int writeDotFelsContrast(const char *filename);

    void areaFiltering(int areaMin);
//    void elongFiltering(int elongMin);

    Image<T> constructImage(Ordering<T> &order);

//    Image<RGB> constructImageInf();
    void areaFiltering(int areaMin, int areaMax);
//    Image<RGB> constructImageInverse();

    void contrastFiltering(int contrastMin);
    void contrastFiltering(int contrastMin, int contrastMax);
    void resetFiltering();

    /** Adaptive filtering **/
    void adaptiveAreaFiltering(int p);
    void adaptiveContrastFiltering(int p);

    /** set abstract attribute used for shaping **/
    void setShapingAttribute(const std::string &attrname);

public:
    void constructNode(Image<T> &im, Node *n, T &value);
//    void paintNodeSup(Image<RGB> &imRes, Node *n, RGB &value);
    template <int N>
    int diff_L1(T v1, T v2);
    template <int N>
    int diff_L2(T v1, T v2);

    void computeAreaHelper();
    void computeCompacityHelper();
    template <int N> void computeFelsContrastHelper();
    void computeMeanGradientHelper(Image<U8> &gradImage);

};

#include "cgraph.hxx"

#endif // CGRAPH_H

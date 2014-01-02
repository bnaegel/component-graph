#ifndef CORE_H
#define CORE_H

#include <QDebug>
#include <iostream>
#include "Common/Image.h"
#include "Common/FlatSE.h"
#include "Common/OrderedQueue.h"
//#include "Algorithms/Morphology.h"
//#include "Algorithms/RegionGrowing.h"
//#include "Algorithms/ConnectedComponents.h"
//#include "Algorithms/ComponentTree.h"

#include <vector>
#include <set>

#include <time.h>

using namespace std;
using namespace LibTIM;

namespace CGraph {

struct Node {
    static int nbLinks;
    static int nbNodes;
    static int count;

    Node(int l) {this->label=l;}
    Node(RGB v, int rep)
        {
        this->area=0;
        this->contrast=0;
        this->value=v;
        this->rep=rep;
        this->label=count++;
        this->isPresent=true;
        this->active=true;
        this->isAncestor=true;
       // this->ancestors.push_back(this);
        }
    void addChild(Node *n) {
        this->childs.push_back(n);

        n->fathers.push_back(this);
        vector <Node *> tmp;
        bool flag=false;
//        for(int i=0; i<ancestors.size(); i++)
//            {
//            if(ancestors[i]->isPresent==true)
//                {
//                if(this->value < ancestors[i]->value)
//                    flag=true;
//                else tmp.push_back(ancestors[i]);
//                }
//            }
//       if(flag) tmp.push_back(this);
//       n->ancestors=tmp;
       nbLinks++;
       }

    int label;
    RGB value;

    bool active;
    bool isAncestor;

    bool isPresent;
    bool isRoot;

    int area;
    int contrast;

    vector <TOffset> pixels;

    vector<Node *> fathers;  // size 0 if no father

    vector<Node *> childs;

   // vector <Node *> ancestors;

    // offset appartenant au noeud (représentant)
    int rep;

};

class Core
{
private:
    Image <RGB> imSource;
    vector <Node *> nodes;
    vector <Node *> imageNodes;
    vector <int> par;
    vector <set <int> > graph;
    vector<bool> active;
    vector<bool> isAncestor;

    vector<map <int, vector <int> > > lowestNodesTab;

public:
    Core() {}
    Core(Image <RGB> &imSource);
    void compute(int value, int value2, int value3, int value4);

    void setSyntheticImage();

    void printColor(RGB value);

    int colorToIndex(RGB &value);
    RGB indexToColor(int &colorKey);

    void printVectorNodes(vector <Node *> v);
    int findNode(int n, vector<int> &par);
    int findNodeExp(int n, vector<int> &par);
    vector <int> findNode(int n, vector<vector <int> > &par);
    int linkNode(int n, int m, vector<int> &par, vector <int> &rank);
    void link(int n,int m,vector<int> &par, vector <int> &rank);
    void makeSet(int n, vector<int> &par, vector<int> &rank);
    void makeSet(int n, vector<vector <int> > &par);

    Node *makeNode(RGB value, int rep);
    void mergeNodes(Node *finalNode, Node *inputNode);
    void printPar(vector<int> par);
    int writeDot(Node *root, const char *filename);
    int writeDot(const char *filename);
    void findMinAncestors(Node *n, RGB v, vector<Node *> &ancestors);
    void findMinAncestors2(Node *n, RGB v, vector <bool> &active, vector <bool> &isAncestor, vector<Node *> &ancestors);
    void findMinAncestors3(Node *n,  vector<Node *> &ancestors);
    void findMinAncestors4( vector <Node *> &adjNodes, RGB v, vector <bool> &active, vector <bool> &isAncestor, vector<Node *> &ancestors);
    Node * componentGraph(Image <RGB> &im, FlatSE &connexity);
    Node * componentGraph2(Image <RGB> &im, FlatSE &connexity);
    Node * componentGraph3(Image <RGB> &im, FlatSE &connexity);
    void componentGraphExp(Image <RGB> &im, FlatSE &connexity);
    vector <int> lowestNodesExp(Image <RGB> &im, RGB v, int q, vector <int > &par);
    vector <int> lowestNodesExpOpti(Image <RGB> &im, RGB v, int q, vector <int > &par);
    vector <int> lowestNodes(Image<RGB> &im, int p,  vector <int> &par, vector <vector <int> > &graph , vector <vector <int> > &minanc, RGB &v);

    void areaFiltering(Node *graph, int value, int value2);
    void contrastFiltering(Node *graph, int value, int value2);
    void printNodes();
    void printGraph();
    void computeArea();
    void computeContrast(Node *graph);
    int computeContrastExp(Node *graph);
    void reconstructMin();
    void constructImage();
    void constructImageExp();

    Node *graphToNode();
};



}  // end namespace

#endif // CORE_H

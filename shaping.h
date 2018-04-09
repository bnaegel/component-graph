//Copyright (C) 2014, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef SHAPING_H
#define SHAPING_H

#include "cgraph.h"


/** Component-tree of a  weighted graph (weight=attribute of the node)
    Compute on each node another attribute based on different information (area ...)
**/

//typedef CGraph::Node Node;


template <class T>
class Shaping
{
public:
    struct ShapingNode {
        int index;
        double weight;


        ShapingNode(int index, double weight, int area):father(0) {
            this->index=index;
            this->weight=weight;
            this->area=area;
            this->contrast=weight;
            active=true;
            }
        std::vector<ShapingNode *> childs;
        ShapingNode * father;
        // list of first graph nodes belonging to current node and having same attribute
        std::vector<int > nodes;
        int area;
        int contrast;
        bool active;

        void addChild(ShapingNode *child) {
            this->childs.push_back(child);
            child->father=this;
        }
    };

public:
    CGraph<T> &cgraph;
private:

    ShapingNode *root;
    //CGraph *shaping;
public:
    Shaping(CGraph<T> &graph) : cgraph(graph) {}

    void computeShaping(); /*!< Compute max-tree of the underlying weighted graph */
    void areaFiltering(int areaMin);
    void areaFiltering(int areaMin, int areaMax);

    int contrastFiltering(int tMin, int tMax);
    int contrastFiltering(int tMin);
    void constructGraph();    /*!< Graph restitution (after filtering) */


    /** @brief Hierarchical queue
        **/
    typedef std::queue<int> *HierarchicalQueue;
    typedef std::vector<std::vector<ShapingNode *> > IndexType;

    HierarchicalQueue hq;
    // For now, container for accessing nodes by level and cc number
    IndexType index;

    int hMin;
    int hMax;
    // size of different containers
    int numberOfLevels;

    int totalNodes;

    static const int NOT_ACTIVE=-1;
    static const int ACTIVE=-2;
    static const int BORDER_STATUS=-3;
    Image <int> STATUS;
    vector <int> number_nodes;
    vector <bool> node_at_level;

    void link_node(ShapingNode *tree, ShapingNode *child);
    ShapingNode *new_node(int h, int n);
    void init();
    void computeTree();
    int flood(int h);
    void update_attributes(ShapingNode *n, int index);
    int hToIndex(int h)  {return h-hMin;}
    int indexToH(int h)  {return h+hMin;}
    int computeContrast();

    int writeDot(const char *filename);
    int computeArea();
    int computeAreaHelper(ShapingNode *tree);
    int contrastFilteringMax(int tMax);
    int elongFiltering(int tMin);
private :
    // heigh attribute
    int computeContrastHelper(ShapingNode *tree);

};

#include "shaping.hxx"

#endif // SHAPING_H

//Copyright (C) 2013, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.


#include "cgraph.h"

using namespace std;

void RAGraph::computeFlatZones()
{
    imFlatZones.setSize(imSource.getSize());

    FlatSE se;
    se.make2DN8();

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


Image<RGB> CGraph::syntheticImage()
{
    int a[]={0,0,0};
    int b[]={1,0,0};
    int c[]={0,1,0};
    int d[]={1,1,0};
    int e[]={1,1,1};
    int f[]={0,0,1};

    RGB A(a);
    RGB B(b);
    RGB C(c);
    RGB D(d);
    RGB E(e);
    RGB F(f);

    TSize size[]={7,1,1};
    TSpacing spacing[]={1.0,1.0,1.0};
//    const RGB data[]={  A,B,C,B,A,
//                        C,C,D,B,A,
//                        B,B,C,A,B,
//                        B,C,A,B,A,
//                        A,C,B,C,A};
    //const RGB data[]={  A,C,C,C,B,E,D,D,C,E,E,D,B,E,B,C,C,C,B,B,A};
    const RGB data[]={ A,E,C,F,B,E,A};

    //    TSize size[]={4,4,1};
    //    TSpacing spacing[]={1.0,1.0,1.0};
    //    const RGB data[]={A,A,B,B,
    //                 B,C,B,B,
    //                 A,A,E,D,
    //                 A,E,E,D};
    Image<RGB> imTmp(size,spacing,data);
    return imTmp;
}


Image<RGB> CGraph::syntheticImage2()
{
    int a[]={0,0,0};
    int b[]={1,0,0};
    int c[]={0,1,0};
    int d[]={1,1,0};
    int e[]={1,1,1};
    int f[]={0,0,1};

    RGB A(a);
    RGB B(b);
    RGB C(c);
    RGB D(d);
    RGB E(e);
    RGB F(f);

    TSize size[]={21,1,1};
    TSpacing spacing[]={1.0,1.0,1.0};
//    const RGB data[]={  A,B,C,B,A,
//                        C,C,D,B,A,
//                        B,B,C,A,B,
//                        B,C,A,B,A,
//                        A,C,B,C,A};
    const RGB data[]={  A,C,C,C,B,E,D,D,C,E,E,D,B,E,B,C,C,C,B,B,A};
    //const RGB data[]={ A,E,C,F,B,E,A};

    //    TSize size[]={4,4,1};
    //    TSpacing spacing[]={1.0,1.0,1.0};
    //    const RGB data[]={A,A,B,B,
    //                 B,C,B,B,
    //                 A,A,E,D,
    //                 A,E,E,D};
    Image<RGB> imTmp(size,spacing,data);
    return imTmp;
}



//// compute the components for G.. :
//// for each point p compute the node in which p belongs
//vector <Node *> CGraph::computeComponents2(Image<RGB> &im, FlatSE &connexity) {

//    int imSize=im.getBufSize();
//    int dx=im.getSizeX();
//    int dy=im.getSizeY();

//    vector <Node *> nodes;

//    Image<bool> active(im.getSize());
//    Image<bool> inQueue(im.getSize());
//    active.fill(true);
//    inQueue.fill(false);
//    std::queue<Point<TCoord > > fifo;

//    for(int y=0; y<dy; y++)
//        for(int x=0;x<dx; x++) {
//            if(active(x,y)) {
//                Point <TCoord> p(x,y);
//                inQueue.fill(false);
//                // new node
//                fifo.push(p);
//                active(p)=false;
//                inQueue(p)=true;
//                Node *n=new Node(im(p),0);

//                while(!fifo.empty()) {
//                    Point<TCoord> p=fifo.front();
//                    fifo.pop();

//                    n->area++;
//                    n->pixels.push_back(im.getOffset(p));
//                    for(int i=0; i<connexity.getNbPoints(); i++) {
//                        Point<TCoord> q=p+connexity.getPoint(i);
//                        // p and q belong to the same node: mark as not active to avoid computing two times the same node

//                        if(im.isPosValid(q)) {
//                            if(im(q)==n->value) active(q)=false;
//                            if(inQueue(q)==false && im(q)>=n->value) {
//                                fifo.push(q);
//                                inQueue(q)=true;
//                            }
//                        }
//                    }
//                }
//                nodes.push_back(n);
//            }

//        }
//    return nodes;
//}
//// check if set n is equal to m
//bool CGraph::isEqual(Node *n, Node *m) {

//    if(n->area!=m->area) return false;

//    for(int i=0; i<n->pixels.size(); i++) {
//        TOffset curPixel=n->pixels[i];
//        bool curPixelIncluded=false;
//        for(int j=0; j<m->pixels.size(); j++) {
//            if(curPixel==m->pixels[j])
//                {
//                curPixelIncluded=true;
//                }
//        }
//        if(curPixelIncluded==false) return false;
//    }

//    return true;
//}

//// check if set n is included in m
//// faster version
//bool CGraphSpace::Core::isIncludedFast(Node *n, Node *m, vector<bool> &tmp) {


//    tmp.assign(tmp.size(), false);
//    for(int i=0; i<m->pixels.size(); i++)
//        tmp[m->pixels[i]]=true;

//    for(int i=0; i<n->pixels.size(); i++)
//        if(tmp[n->pixels[i]]==false) return false;
//    return true;
//}

//void CGraphSpace::Core::computeLinksFast(vector <Node *> &nodes) {

//    vector<bool> tmp(imSource.getBufSize(),false);
//    for(int i=0; i<nodes.size(); i++) {
//        Node *n=nodes[i];
//        for(int j=0; j<nodes.size(); j++) {
//            if(j!=i && nodes[j]->area<nodes[i]->area) {
//                Node *m=nodes[j];
//                if(isIncludedFast(m,n,tmp)) {
//                    n->addChild(m);
//                }
//            }
//        }
//    }
//}


//vector <Node *> CGraph::Core::minimalElements(vector <Node *> &nodes, vector <bool> &tmp) {
//    vector <Node *> res;

//    vector <bool> active(nodes.size(), true);
//    for(int j=0; j<nodes.size(); j++) {
//        RGB i=nodes[j]->value;
//        for(int k=0; k<nodes.size(); k++) {
//            if(k!=j) {
//                RGB value2=nodes[k]->value;
//                if(isIncludedFast(nodes[k],nodes[j], tmp) ) {
//                    active[k]=false;
//                }
//            }
//        }
//    }

//    for(int j=0; j<nodes.size(); j++) {
//        if(active[j]) res.push_back(nodes[j]);
//    }

//    return res;
//}

//// compute transitive reduction of graph from its root
//// for each node:
////  - keep as childs only the minimal elements of the childs
//void CGraph::Core::computeTransitiveReduction(vector<Node *> &nodes)
//{
//    vector<bool> tmp(imSource.getBufSize(),false);
//    for(int i=0; i<nodes.size(); i++) {
//        Node *curNode=nodes[i];
//        curNode->childs=minimalElements(curNode->childs, tmp);
//    }
//}

//// G
//// first version:
//// -compute all components
//// -compute inclusion relations/ construct links
//// -compute the transitive reduction

//void CGraph::computeFullGraph(FlatSE &connexity)
//{
//    Image <RGB> im=this->imSource;
//    OrderedQueue <RGB> pq;

//    vector<Node *> nodes;

//    vector <bool>  processed(im.getBufSize());
//    vector <RGB> colorProcessed;

//    //tri des points par valeur décroissante
////    for(int i=0; i<im.getBufSize(); i++)
////    {
////        RGB value=im(i);

////        int R=value[0];
////        int G=value[1];
////        int B=value[2];

////        bool inQueue=false;

////        for(int k=0; k<colorProcessed.size(); k++) {
////            if(colorProcessed[k]==value)
////                inQueue=true;
////        }

////        if(!inQueue) {
////            pq.put(-(R+G+B),value);
////            colorProcessed.push_back(value);
////        }
////    }
//    int N=0;
//    int imSize=im.getBufSize();


//    std::cout << "Begin compute components\n";
//    clock_t c1=clock();

//    nodes=computeComponents2(im,connexity);

//    std::cout << "Size of vector nodes: " << nodes.size() << "\n";

////    while(!pq.empty()) {
////        RGB v=pq.get();

////        computeComponents(im,v,connexity,nodes);

////        //std::cout<< "Number of nodes at level >=(" <<(int)v[0] << "," << (int)v[1] << "," << (int)v[2] << ") :" << nodes.size() << "\n";

////        }
//     clock_t c2=clock();
//    std::cout << "End compute components Time : " << (double)(c2-c1)*1000/CLOCKS_PER_SEC << "ms\n\n";
//    std::cout << "Number of nodes: " << CGraphSpace::Node::nbNodes << "\n\n";


//    std::cout << "Begin compute links fast\n";
//    c1=clock();
//    computeLinksFast(nodes);
//    c2=clock();
//    std::cout << "End compute links fast Time : " << (double)(c2-c1)*1000/CLOCKS_PER_SEC << "ms\n\n";

//    /*std::cout << "Begin compute links\n";
//    c1=clock();
//    computeLinks(nodes);
//    c2=clock();
//    std::cout << "End compute links Time : " << (double)(c2-c1)*1000/CLOCKS_PER_SEC << "ms\n\n";
//    */

//    std::cout << "Begin compute transitive reduction\n";
//    c1=clock();
//    computeTransitiveReduction(nodes);
//    c2=clock();
//    std::cout << "End compute transitive reduction time : " << (double)(c2-c1)*1000/CLOCKS_PER_SEC << "ms\n\n";


//    Node *root=addRoot(nodes);

//    return root;

//}


/**
*   Algorithme fonctionnel
*   -on prend les zones plates selon leur couleur décroissante
*   -on lance une propagation à partir de la zone plate afin de rassembler exhaustivement toutes les zp du noeud
*   -on crée les liens
**/
int CGraph::computeGraph(CColorOrdering *order, CGraphWatcher *watcher) {

    OrderedQueue <int> pq;

    vector<bool> processed(rag->nodes.size());

    //std::cout << rag->nodes.size() << "\n";


    /** Warning : nodes[0] does not exist !!!! */
    for(int i=1; i<rag->nodes.size(); i++) {

        RGB value=rag->nodes[i]->color;

        int priority=order->getPriority(value);

        //qDebug() << "Priority : " << priority << "\n";
        // put in the prior. queue the flat zone number
        pq.put(priority,i);
        processed[i]=false;
    }

    // index from flat-zone number to corresponding node
    vector <Node *> regionToNode(rag->nodes.size());
    regionToNode.assign(regionToNode.size(),0);

    std::queue<int> fifo;
    std::vector<bool> infifo(regionToNode.size());

    std::vector<Node *> potentialChilds;

    while(!pq.empty() ) {
        int i=pq.get();

        if(watcher!=0)
            {
            watcher->progressUpdate();
            }
        //std::cout << "Région visitée : " << i << "\n";

        if(regionToNode[i]==0) {
            /** i est une région canonique (i==regionToNode[i]->index- */
            RAGraph::Vertex *curVertex=rag->nodes[i];
            Node *curNode=new Node(curVertex->index,curVertex->color,curVertex->pixels.size());

            curNode->regions.push_back(i);
            regionToNode[i]=curNode;

            int valueMax=curNode->color[0]+curNode->color[1]+curNode->color[2];

            potentialChilds.clear();

            /** on lance la propagation */
            infifo.assign(infifo.size(), false);
            fifo.push(i);
            infifo[i]=true;
            int M=0;
            while(!fifo.empty()) {
                int curpt=fifo.front();


                M++;
                fifo.pop();
                RAGraph::Vertex *tmpVertex=rag->nodes[curpt];
                for(int v=0; v<tmpVertex->allNb.size(); v++ ) {
                    int nb=tmpVertex->allNb[v];
                    if(!infifo[nb] && order->islessequal(rag->nodes[i]->color , rag->nodes[nb]->color)) {

                        if(order->isequal(rag->nodes[i]->color , rag->nodes[nb]->color)) {
                            /** ZP qui appartient au même noeud : on fusionne*/
                            curNode->regions.push_back(nb);
                            regionToNode[nb]=curNode;


                            //std::cout << "Fusion de " << i << " et " << nb << "\n";
                        }
                        else {
                            /** zp potentiellement descendant du noeud courant : on teste si un père du noeud est comparable avec curNode
                        *  Si c'est le cas, a n'est pas un fils direct de curNode
                        */
                            Node *tmp=regionToNode[nb];

                            valueMax=std::max(valueMax,tmp->color[0]+tmp->color[1]+tmp->color[2]);

                            assert(tmp!=0);
                            //if(tmp!=0) {
                                bool isChild=true;
                                for(int a=0; a<tmp->fathers.size(); a++) {
                                    if(order->islessequal( curNode->color , tmp->fathers[a]->color) ) {
                                        isChild=false;
                                        break;
                                    }
                                }
                                if(isChild) {
                                    curNode->childs.push_back(tmp);
                                    tmp->fathers.push_back(curNode);

                                }
                                //potentialChilds.push_back(tmp);
                                //std::cout << "\t Ajoute " << nb << "\n";
                            }
                        //}
                        curNode->area+=rag->nodes[nb]->pixels.size();
                        curNode->contrast=valueMax-(curNode->color[0]+curNode->color[1]+curNode->color[2]);
                        fifo.push(nb);
                        infifo[nb]=true;
                    }
                }


            }
            processed[i]=true;
        }
    }

    /** Assign a new index to each node */
    for(int i=0; i<regionToNode.size(); i++) {
        if(regionToNode[i]!=0 && regionToNode[i]->index==i) {
            graph.push_back(regionToNode[i]);
            graph[graph.size()-1]->index=graph.size()-1;
        }

    }

    /** Add fictitious root*/

    RGB value(0,0,0);

    root=new Node(-1,value,imSource.getBufSize());

        for (int i=0; i<graph.size(); i++) {

            if(graph[i]->fathers.size()==0) {
                root->addChild(graph[i]);
            }
        }

    return 1;
}


bool CGraph::isLTE(RGB &v, RGB &w)
{
    if(v[0]>=w[0] && v[1]>=w[1] && v[2]>=w[2] )
        return true;
    else return false;
}

/**
*   Algorithme fonctionnel
*   -on prend les zones plates selon leur couleur décroissante
*   -on lance une propagation à partir de la zone plate afin de rassembler exhaustivement toutes les zp du noeud
*   -on crée les liens
**/
int CGraph::computeGraphInverse(CGraphWatcher *watcher) {

    OrderedQueue <int> pq;

    vector<bool> processed(rag->nodes.size());

    //std::cout << rag->nodes.size() << "\n";


    /** Warning : nodes[0] does not exist !!!! */
    for(int i=1; i<rag->nodes.size(); i++) {

        RGB value=rag->nodes[i]->color;

        int R=value[0];
        int G=value[1];
        int B=value[2];
        // put in the prior. queue the flat zone number
        pq.put((R+G+B),i);
        processed[i]=false;
    }

    // index from flat-zone number to corresponding node
    vector <Node *> regionToNode(rag->nodes.size());
    regionToNode.assign(regionToNode.size(),0);

    std::queue<int> fifo;
    std::vector<bool> infifo(regionToNode.size());

    std::vector<Node *> potentialChilds;

    while(!pq.empty() ) {
        int i=pq.get();

        if(watcher!=0)
            {
            watcher->progressUpdate();
            }
        //std::cout << "Région visitée : " << i << "\n";

        if(regionToNode[i]==0) {
            /** i est une région canonique (i==regionToNode[i]->index- */
            RAGraph::Vertex *curVertex=rag->nodes[i];
            Node *curNode=new Node(curVertex->index,curVertex->color,curVertex->pixels.size());

            curNode->regions.push_back(i);
            regionToNode[i]=curNode;

            potentialChilds.clear();

            /** on lance la propagation */
            infifo.assign(infifo.size(), false);
            fifo.push(i);
            infifo[i]=true;
            int M=0;
            while(!fifo.empty()) {
                int curpt=fifo.front();
                M++;
                fifo.pop();
                RAGraph::Vertex *tmpVertex=rag->nodes[curpt];
                for(int v=0; v<tmpVertex->allNb.size(); v++ ) {
                    int nb=tmpVertex->allNb[v];
                    if(!infifo[nb] && isLTE(rag->nodes[i]->color, rag->nodes[nb]->color))  {
                        if(rag->nodes[i]->color == rag->nodes[nb]->color) {
                        /** ZP qui appartient au même noeud : on fusionne*/
                        curNode->regions.push_back(nb);
                        regionToNode[nb]=curNode;

                        /** Mise à jour de l'aire */

                        //std::cout << "Fusion de " << i << " et " << nb << "\n";
                        }
                        else {
                        /** zp potentiellement descendant du noeud courant : on teste si un père du noeud est comparable avec curNode
                        *  Si c'est le cas, a n'est pas un fils direct de curNode
                        */
                            Node *tmp=regionToNode[nb];
                            bool isChild=true;
                            for(int a=0; a<tmp->fathers.size(); a++) {
                                if(isLTE(curNode->color , tmp->fathers[a]->color) ) {
                                    isChild=false;
                                    break;
                                }
                            }
                            if(isChild) {
                                curNode->childs.push_back(tmp);
                                tmp->fathers.push_back(curNode);

                            }
                        //potentialChilds.push_back(tmp);
                        //std::cout << "\t Ajoute " << nb << "\n";
                        }
                        curNode->area+=rag->nodes[nb]->pixels.size();
                        fifo.push(nb);
                        infifo[nb]=true;
                    }
                }
                //if(M%100==0) std::cout << "Size of vector potchilds : " << potentialChilds.size() << "\n";
                /** On crée les liens */
//                for(int c=0; c<potentialChilds.size(); c++) {
//                    Node *n=potentialChilds[c];
//                    bool isChild=true;
//                    for(int a=0; a<n->fathers.size(); a++) {
//                        if(curNode->color <= n->fathers[a]->color )
//                            isChild=false;
//                    }
//                    if(isChild) {
//                        curNode->childs.push_back(n);
//                        n->fathers.push_back(curNode);
//                    }
//                }

            }
        processed[i]=true;
        }
    }

    /** Assign a new index to each node */
    for(int i=0; i<regionToNode.size(); i++) {
        if(regionToNode[i]!=0 && regionToNode[i]->index==i) {
            graph.push_back(regionToNode[i]);
            graph[graph.size()-1]->index=graph.size()-1;
        }

    }

    /** Add fictitious root*/

    RGB value(255,255,255);

    root=new Node(-1,value,imSource.getBufSize());

        for (int i=0; i<graph.size(); i++) {

            if(graph[i]->fathers.size()==0) {
                root->addChild(graph[i]);
            }
        }

    return 1;
}


//int CGraph::writeDot(const char *filename)
//{

//    std::ofstream file(filename,std::ios_base::trunc  | std::ios_base::binary);
//    if(!file)
//    {
//        std::cerr << "File I/O error\n";
//        return 0;
//    }



//    file << "digraph G {\n";

//    for(int i=0; i<graph.size(); i++) {
//        Node *tmp=graph[i];
//        //std::cout << "Node number : " << i << "  " << tmp->index << "\n";

//        std::stringstream nodeName;
//        nodeName.str("");
//        nodeName << "\"" << tmp->index << ":(" <<(int)tmp->color[0] << "," <<
//                    (int)tmp->color[1]<<","<<
//                    (int)tmp->color[2]<< " a=" << tmp->area << ")\"";
//        if(!tmp->active)
//            file << "\t" << nodeName.str() << "[style=filled, fillcolor=grey];\n";

//        if(tmp->childs.size()!=0)
//            for(int j=0; j<tmp->childs.size(); j++)
//            {
//                std::stringstream nodeNameChild;
//                nodeNameChild << "\"" << tmp->childs[j]->index << ":(" <<(int)tmp->childs[j]->color[0] << "," <<
//                                 (int)tmp->childs[j]->color[1]<<","<<
//                                 (int)tmp->childs[j]->color[2]<< " a=" << tmp->childs[j]->area  << ")\"";

//                file << "\t" <<
//                        nodeName.str() << "->" << nodeNameChild.str() << ";\n";

//            }
//        else file << "\t" << nodeName.str() << ";\n";

//    }

//    file << "}\n";

//    file.close();
//    return 1;
//}


void CGraph::areaFiltering(int areaMin)
{


    for(int i=0; i<graph.size(); i++) {
        Node *n=graph[i];
        if(n->area<areaMin) n->active=false;
    }

    if(root->area<areaMin) root->active=false;
}


void CGraph::areaFiltering(int areaMin, int areaMax)
{


    for(int i=0; i<graph.size(); i++) {
        Node *n=graph[i];
        if(n->area<areaMin || n->area>areaMax) n->active=false;
    }

    if(root->area<areaMin || root->area>areaMax) root->active=false;
}


void CGraph::contrastFiltering(int contrastMin)
{

    for(int i=0; i<graph.size(); i++) {
        Node *n=graph[i];
        if(n->contrast<contrastMin) n->active=false;
    }

    if(root->contrast<contrastMin) root->active=false;
}


void CGraph::resetFiltering()
{
/** Initialisation*/
    for(int i=0; i<graph.size(); i++) {graph[i]->active=true;}
}

/**
    Keep the p % nodes having the biggest area
*/
void CGraph::adaptiveAreaFiltering(int p)
{


    std::map<int, int> histo;
    for(int i=0; i<graph.size(); i++) {
        Node *n=graph[i];
        histo[-n->area]++;
    }

    histo[-root->area]++;

    int totalNodes=graph.size()+1;
    int bestNodes=(p*totalNodes)/100;

    int count=0;

    int threshold;

    std::map<int,int>::iterator it;
    for(it=histo.begin(); it!=histo.end(); ++it) {
        count+=it->second;
        if(count> bestNodes )
            {
            threshold=-it->first;
            break;
            }
    }
    //qDebug() << "Area = " << threshold <<"\n";
    areaFiltering(threshold);

}


/**
    Keep the p % nodes having the biggest area
*/
void CGraph::adaptiveContrastFiltering(int p)
{



    std::map<int, int> histo;
    for(int i=0; i<graph.size(); i++) {
        Node *n=graph[i];
        histo[-n->contrast]++;
    }

    histo[-root->contrast]++;

    int totalNodes=graph.size()+1;
    int bestNodes=(p*totalNodes)/100;

    int count=0;

    int threshold;

    std::map<int,int>::iterator it;
    for(it=histo.begin(); it!=histo.end(); ++it) {
        count+=it->second;
        if(count> bestNodes )
            {
            threshold=-it->first;
            break;
            }
    }
    //qDebug() << "Area = " << threshold <<"\n";
    contrastFiltering(threshold);

}


void CGraph::contrastFiltering(int contrastMin, int contrastMax)
{


    for(int i=0; i<graph.size(); i++) {
        Node *n=graph[i];
        if(n->contrast<contrastMin || n->contrast>contrastMax) n->active=false;
    }

    if(root->contrast<contrastMin || root->contrast>contrastMax) root->active=false;
}

//void CGraph::reconstructMin()
//{
//    std::queue<Node *> fifo;

//    vector <bool> active(graph.size());
//    active.assign(active.size(),true);

//    for(int i=0; i<graph.size(); i++) {
//        if(graph[i]->active==false && graph[i]->fathers.size()>1) {
//            fifo.push(graph[i]);
//            active[graph[i]->index]=false;

//            while(!fifo.empty()) {
//                Node *n=fifo.front();
//                fifo.pop();

//                int nactive=0;

//                for(int j=0; j<n->fathers.size(); j++) {
//                    if(n->fathers[j]->active==true) nactive++;
//                }
//                //qDebug() << "Node: " << n->label << " "<<  nactive << "\n";
//                int j=0;
//                while(nactive>1) {
//                    if(n->fathers[j]->active==true) {
//                        n->fathers[j]->active=false;
//                        if(active[n->fathers[j]->index])
//                        {
//                            fifo.push( n->fathers[j]);
//                            active[n->fathers[j]->index]=false;
//                        }
//                        nactive--;
//                    }
//                    j++;
//                }
//            }
//        }
//    }

//    for(int i=0; i<graph.size(); i++) {
//        if(graph[i]->active==false) {
//            Node *n=graph[i];
//            int nactive=0;
//            for(int j=0; j<n->fathers.size(); j++) {
//                if(n->fathers[j]->active==true) nactive++;
//            }
//            assert(nactive==0 || nactive==1);
//        }
//    }

//}


Image<RGB> CGraph::constructImageInf()
{
    std::queue<Node *> fifo;

    vector <bool> active(graph.size());
    active.assign(active.size(),true);

    OrderedQueue <Node *> pq;

    vector<bool> processed(graph.size());

    RGB rgbmin(0,0,0);

    /** Warning : nodes[0] does not exist !!!! */
    for(int i=0; i<graph.size(); i++) {

        graph[i]->dispColor=rgbmin;

        RGB value=graph[i]->color;

        int R=value[0];
        int G=value[1];
        int B=value[2];
        pq.put(-(R+G+B),graph[i]);
        processed[i]=false;
    }

    /** Special case for the root*/
    root->dispColor=rgbmin;

    while(!pq.empty()) {
        Node *n=pq.get();

        if(n->active==false && n->fathers.size()>1) {

            int nactive=0;
            RGB value(255,255,255);
            for(int j=0; j<n->fathers.size(); j++) {
                if(n->fathers[j]->active==true)
                {
                    nactive++;
                    value[0]=std::min(value[0],n->fathers[j]->color[0]);
                    value[1]=std::min(value[1],n->fathers[j]->color[1]);
                    value[2]=std::min(value[2],n->fathers[j]->color[2]);
                }
            }
            //qDebug() << "Node: " << n->label << " "<<  nactive << "\n";
            if(nactive>1) {
                for(int j=0; j<n->fathers.size(); j++) {

                    if(n->fathers[j]->active==true) {
                        n->fathers[j]->dispColor=value;
                    }
                }
            }
        }
    }

    Image<RGB> imRes=imSource;
    imRes.fill(0);

    for(int i=0; i<graph.size(); i++) {


        RGB value=graph[i]->color;

        int R=value[0];
        int G=value[1];
        int B=value[2];
        pq.put((R+G+B),graph[i]);
        processed[i]=false;
    }

    /** Special case for the root*/
    pq.put(-1,root);

    while(!pq.empty()) {
        Node *curNode=pq.get();
       //qDebug() << curNode->index << "(" << curNode->color[0] << "," << curNode->color[1] << ","  << curNode->color[2] << ") \n";
        if(curNode->active==true && curNode->dispColor!=rgbmin) {
            curNode->dispColor=curNode->color;
        }

        paintNode(imRes,curNode, curNode->dispColor);

        for(int c=0; c<curNode->childs.size(); c++) {
            Node *curChild=curNode->childs[c];
            if(curChild->active==false) {
//            if(curNode->dispColor>curChild->dispColor)
               curChild->dispColor=curNode->dispColor;
            }

        }
    }

    return imRes;

}

void CGraph::paintNode(Image <RGB> &imRes, Node *n, RGB &value)
{
    for(int r=0; r<n->regions.size(); r++) {
        vector<Point<TCoord> > points=rag->nodes[n->regions[r]]->pixels;
        for(int p=0; p<points.size(); p++) {
           // assert(value < n->color|| value > n->color || value == n->color  );
            //if(value> imRes(points[p]))
                imRes(points[p])=value;
        }
    }
}


void CGraph::paintNodeSup(Image <RGB> &imRes, Node *n, RGB &value)
{
    for(int r=0; r<n->regions.size(); r++) {
        assert(n->regions[r]!=0);
        vector<Point<TCoord> > points=rag->nodes[n->regions[r]]->pixels;
        for(int p=0; p<points.size(); p++) {
            RGB imValue=imRes(points[p]);
            imValue[0]=std::max(imValue[0],value[0]);
            imValue[1]=std::max(imValue[1],value[1]);
            imValue[2]=std::max(imValue[2],value[2]);

            imRes(points[p])=imValue;
        }
    }
}

//Image<RGB> CGraph::constructImage()
//{
//    Image<RGB> imRes=imSource;
//    imRes.fill(0);
//    if(!graph.empty()) {
//        std::vector<bool> infifo(graph.size());
//        for(int i=0; i<graph.size(); i++) {
//            Node *n=graph[i];
//            std::queue<Node *> fifo;
//            infifo.assign(infifo.size(),false);
//            if(n->active) {
//                fifo.push(n);
//                infifo[n->index]=true;
//                RGB value=n->color;
//                while(!fifo.empty()) {
//                    Node *curNode=fifo.front();
//                    fifo.pop();
//                    paintNodeSup(imRes,curNode, value);

//                    for(int c=0; c<curNode->childs.size(); c++) {

//                        if(!infifo[curNode->childs[c]->index]) {
//                            fifo.push(curNode->childs[c]);
//                            infifo[curNode->childs[c]->index]=true;
//                        }
//                    }
//                }
//            }

//        }
//    }
//    return imRes;
//}


//Image<RGB> CGraph::constructImage()
//{
//    Image<RGB> imRes=imSource;
//    imRes.fill(0);
//    std::queue<Node *> fifo;
//    if(!graph.empty()) {
//        std::vector<bool> infifo(graph.size());
//        for(int i=0; i<graph.size(); i++) {
//            fifo=std::queue<Node *>();
//            Node *n=graph[i];

//            if(n->active) {
//                paintNodeSup(imRes,n, n->color);
//            }
//            else  {
//                infifo.assign(infifo.size(),false);

//                infifo[n->index]=true;
//                fifo.push(n);

//                bool stop=false;

//                while(!fifo.empty() && !stop) {
//                    Node *curNode=fifo.front();
//                    fifo.pop();
//                    for(int c=0; c<curNode->fathers.size(); c++) {
//                        if(curNode->fathers[c]->active==true) {
//                            RGB value=curNode->fathers[c]->color;
//                            paintNodeSup(imRes,n,value);
//                            //stop=true;
//                        }

//                        else
//                        {
//                            if(!infifo[curNode->fathers[c]->index]) {
//                                fifo.push(curNode->fathers[c]);
//                                infifo[curNode->fathers[c]->index]=true;
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }

//    return imRes;
//}


//Image<RGB> CGraph::constructImage()
//{
//    Image<RGB> imRes=imSource;
//    imRes.fill(0);
//    std::queue<Node *> fifo;

//    RGB rgbmin(0,0,0);

//    if(!graph.empty()) {
//        for (int i=0; i<graph.size(); i++)
//            graph[i]->dispColor=rgbmin;
//        std::vector<bool> infifo(graph.size());
//        infifo.assign(infifo.size(),false);

//        fifo.push(root);
//        root->active=true;

//        while(!fifo.empty()) {
//            Node *curNode=fifo.front();
//            fifo.pop();

//            if(curNode->index!=-1) {
//                assert(curNode->dispColor!=rgbmin);
//                paintNode(imRes,curNode, curNode->dispColor);
//                }

//            for(int c=0; c<curNode->childs.size(); c++) {
//                Node *curChild=curNode->childs[c];
//                if(curNode->active==true) {
//                    if(curChild->active==false && !infifo[curChild->index]) {
//                        //                    curNode->childs[c]->dispColor[0]=std::max(curNode->childs[c]->dispColor[0],curNode->dispColor[0]);
//                        //                    curNode->childs[c]->dispColor[1]=std::max(curNode->childs[c]->dispColor[1],curNode->dispColor[1]);
//                        //                    curNode->childs[c]->dispColor[2]=std::max(curNode->childs[c]->dispColor[2],curNode->dispColor[2]);

//                    curChild->dispColor=curNode->dispColor;

//                    }
//                    else if(curChild->active==true) curChild->dispColor=curChild->color;

//                }
//                else curChild->dispColor=curNode->dispColor;


//                if(!infifo[curChild->index]) {
//                    fifo.push(curChild);
//                    infifo[curChild->index]=true;
//                }
//            }

//        }
//    }

////    for(int i=0; i<graph.size(); i++) {
////        Node *n=graph[i];
////        paintNodeSup(imRes,n, n->dispColor);
////    }

//    return imRes;
//}


Image<RGB> CGraph::constructImage(CColorOrdering *order)
{
    Image<RGB> imRes=imSource;
    imRes.fill(0);
    std::queue<Node *> fifo;

    OrderedQueue <Node *> pq;

    vector<bool> processed(graph.size());

    RGB rgbmin(0,0,0);

    for(int i=0; i<graph.size(); i++) {

        RGB value=graph[i]->color;

        pq.put(-order->getPriority(value),graph[i]);
        processed[i]=false;
    }

    /** Special case for the root*/
    pq.put(-1,root);


    while(!pq.empty()) {
        Node *curNode=pq.get();
       //qDebug() << curNode->index << "(" << curNode->color[0] << "," << curNode->color[1] << ","  << curNode->color[2] << ") \n";
        if(curNode->active==true ) {
            curNode->dispColor=curNode->color;
        }

        paintNode(imRes,curNode, curNode->dispColor);

        for(int c=0; c<curNode->childs.size(); c++) {
            Node *curChild=curNode->childs[c];
            if(curChild->active==false) {
//            if(curNode->dispColor>curChild->dispColor)
               curChild->dispColor=curNode->dispColor;
            }

        }
    }

    return imRes;
}



Image<RGB> CGraph::constructImageInverse()
{
    Image<RGB> imRes=imSource;
    imRes.fill(0);
    std::queue<Node *> fifo;

    OrderedQueue <Node *> pq;

    vector<bool> processed(graph.size());

    RGB rgbmin(0,0,0);

    /** Warning : nodes[0] does not exist !!!! */
    for(int i=0; i<graph.size(); i++) {

        graph[i]->dispColor=rgbmin;

        RGB value=graph[i]->color;

        int R=value[0];
        int G=value[1];
        int B=value[2];
        pq.put(-(R+G+B),graph[i]);
        processed[i]=false;
    }

    /** Special case for the root*/
    pq.put(-1,root);


    while(!pq.empty()) {
        Node *curNode=pq.get();

        if(curNode->active==true ) {
            curNode->dispColor=curNode->color;
        }

        paintNode(imRes,curNode, curNode->dispColor);

        for(int c=0; c<curNode->childs.size(); c++) {
            Node *curChild=curNode->childs[c];
            if(curChild->active==false) {
//            if(curNode->dispColor>curChild->dispColor)
               curChild->dispColor=curNode->dispColor;
            }

        }
    }

    return imRes;
}


//Image <RGB> CGraph::constructImage()
//{
//    Image<RGB> result=this->imSource;
//    std::queue<Node *> fifo;
//    vector<bool> active(graph.size());

//    std::cout << "Active size: " << active.size() << "\n";

//    for(int i=0; i<graph.size() ; i++)
//    {
//        //clear the fifo...
//        fifo=std::queue<Node *>();
//        if(graph[i]->active==false)
//        {
//            active.assign(active.size(),true);
//            Node *n=graph[i];
//            for(int j=0; j<n->fathers.size(); j++)
//            {
//                if(active[n->fathers[j]->index])
//                {
//                    fifo.push(n->fathers[j]);
//                    active[n->fathers[j]->index]=false;
//                }

//            }
//            bool stop=false;
//            while(!fifo.empty() && !stop)
//            {
//                Node *p=fifo.front();
//                fifo.pop();
//                if(p->active==true)
//                {
//                    value=p->color;
//                    stop=true;
//                }
//                else
//                    for(int j=0; j<p->fathers.size(); j++)
//                    {
//                        if(active[p->fathers[j]->label])
//                        {
//                            fifo.push(p->fathers[j]);
//                            active[p->fathers[j]->label]=false;
//                        }
//                    }
//            }
//        }
//    }
//    for(int i=0; i<result.getBufSize(); i++)
//    {
//        RGB value;
//        value[0]=0;
//        value[1]=0;
//        value[2]=0;
//        if(findNode2(i,this->par)!=-1)
//            value=imSource(findNode2(i,this->par));

//        result(i)=value;
//        assert(result(i)<=imSource(i));
//    }


//    return result;
//}



//void CGraph::computeArea()
//{
//    std::queue<Node *> fifo;
//    std::vector<int> active(imageNodes.size());
//    for(int i=0; i<imSource.getBufSize(); i++) {
//        int curNode=findNode(i,this->par);

//        for(int j=0; j<active.size(); j++) active[j]=true;

//        fifo.push(imageNodes[curNode]);
//        active[imageNodes[curNode]->label]=false;

//        while(!fifo.empty()) {
//            Node *p=fifo.front();
//            fifo.pop();
//            p->area++;

//            for(int k=0; k<p->fathers.size(); k++) {
//                if(active[p->fathers[k]->label] )   {
//                    fifo.push(p->fathers[k]);
//                    active[p->fathers[k]->label]=false;
//                }
//            }
//        }
//    }
//}


//int CGraph::computeContrast(Node *tree)
//{
//    if(tree!=0)
//    {
//        int current_level=tree->contrast;
//        int current_max=0;
//        int current_contrast=0;

//        if(tree->contrast==0) {


//            for(int i=0; i<tree->childs.size(); i++)
//            {
//                Node *c=tree->childs[i];
//                int current_contrast=c->value[0]-tree->value[0]+c->value[1]-tree->value[1]+c->value[2]-tree->value[2]+computeContrast(c);
//                if(current_contrast>current_max)
//                    current_max=current_contrast;
//            }
//            tree->contrast=current_max;
//        }
//        return tree->contrast;
//    }
//    // error
//    else return -1;
//}



bool CGraph::notComparable(Image<RGB> &im, Point<TCoord> &p, Point<TCoord> &q)
{
    if(!(im(p)<im(q) ) && !(im(p)>im(q)) && !(im(p)==im(q)))
        return true;
    else return false;

}


int CGraph::writeDot(const char *filename)
{
    if(root!=0)
    {
        std::ofstream file(filename,std::ios_base::trunc  | std::ios_base::binary);
        if(!file)
        {
            std::cerr << "File I/O error\n";
            return 0;
        }

        /** Maximum label of the graph **/
        int labelMax=graph.size()+1;
        bool isActive[labelMax];
        for(int i=0; i<labelMax; i++) isActive[i]=true;

        file << "digraph G {\n";

        std::queue <Node *> fifo;
        fifo.push(root);
        while(!fifo.empty() )
        {
            Node *tmp=fifo.front();
            fifo.pop();

            std::stringstream nodeName;
            nodeName.str("");
            nodeName << "\"" << tmp->index << ":(" <<(int)tmp->color[0] << "," <<
                        (int)tmp->color[1]<<","<<
                        (int)tmp->color[2]<< " a=" << tmp->area << ")\"";
            if(!tmp->active)
                file << "\t" << nodeName.str() << "[style=filled, fillcolor=grey];\n";

            for(int i=0; i<tmp->childs.size(); i++)
            {
                std::stringstream nodeNameChild;
                nodeNameChild << "\"" << tmp->childs[i]->index << ":(" <<(int)tmp->childs[i]->color[0] << "," <<
                                 (int)tmp->childs[i]->color[1]<<","<<
                                 (int)tmp->childs[i]->color[2]<< " a=" << tmp->childs[i]->area  << ")\"";

                file << "\t" <<
                        nodeName.str() << "->" << nodeNameChild.str() << ";\n";

                if(isActive[tmp->childs[i]->index]==true)
                {
                    fifo.push(tmp->childs[i]);
                    isActive[tmp->childs[i]->index]=false;
                }
            }

        }

        file << "}\n";

        file.close();
        return 1;
    }
    else
        return 0;
}



///** check if set n is equal to m */
//bool CGraph::isEqual(Node *n, Node *m) {

//    if(n->area!=m->area) return false;

//    for(int i=0; i<n->pixels.size(); i++) {
//        TOffset curPixel=n->pixels[i];
//        bool curPixelIncluded=false;
//        for(int j=0; j<m->pixels.size(); j++) {
//            if(curPixel==m->pixels[j])
//                {
//                curPixelIncluded=true;
//                }
//        }
//        if(curPixelIncluded==false) return false;
//    }

//    return true;
//}



/** Test if set n is included in m
*/
bool CGraph::isIncluded(Node *n, Node *m, vector<bool> &tmp)
{
    tmp.assign(tmp.size(), false);
    for(int i=0; i<m->pixels.size(); i++)
        tmp[m->pixels[i]]=true;

    for(int i=0; i<n->pixels.size(); i++)
        if(tmp[n->pixels[i]]==false) return false;
    return true;
}

void CGraph::computeLinks(CColorOrdering *order, vector <Node *> &nodes)
{
    vector<bool> tmp(imSource.getBufSize(),false);
    for(int i=0; i<nodes.size(); i++) {
        Node *n=nodes[i];
        for(int j=0; j<nodes.size(); j++) {
            if(j!=i && nodes[j]->area<=nodes[i]->area && order->islessequal(nodes[i]->color,nodes[j]->color)) {
                Node *m=nodes[j];
                if(!order->isequal(m->color,n->color) && isIncluded(m,n,tmp)) {
                    n->addChild(m);
                }
            }
        }
    }
}



CGraph::Node * CGraph::addRoot(vector <Node *> &nodes)
{
    Node *root=new Node(-1,0 ,0);

    for (int i=0; i<nodes.size(); i++) {
        if(nodes[i]->fathers.size()==0) {
            root->addChild(nodes[i]);
        }
    }
    return root;
}


vector <CGraph::Node *> CGraph::minimalElements(CColorOrdering *order, vector <Node *> &nodes, vector <bool> &tmp) {
    vector <Node *> res;

    vector <bool> active(nodes.size(), true);
    for(int j=0; j<nodes.size(); j++) {
        RGB i=nodes[j]->color;
        for(int k=0; k<nodes.size(); k++) {
            if(k!=j) {
                RGB value2=nodes[k]->color;
                if(order->islessequal(i,value2) && isIncluded(nodes[k],nodes[j], tmp) ) {
                    active[k]=false;
                }
            }
        }
    }

    for(int j=0; j<nodes.size(); j++) {
        if(active[j]) res.push_back(nodes[j]);
    }

    return res;
}

/** Compute transitive reduction of graph from its root
* For each node:
*  - keep as childs only the minimal elements of the childs
*/
void CGraph::computeTransitiveReduction(CColorOrdering *order, vector<Node *> &nodes)
{
    vector<bool> tmp(imSource.getBufSize(),false);
    for(int i=0; i<nodes.size(); i++) {
        Node *curNode=nodes[i];
        curNode->childs=minimalElements(order,curNode->childs, tmp);
    }
}

/** Compute the nodes for G and \dot G component-graph
**/
vector<CGraph::Node *> CGraph::computeComponents(CColorOrdering *order, OrderedQueue <RGB> &pq)
{
    int dx=imSource.getSizeX();
    int dy=imSource.getSizeY();

    vector <Node *> nodes;

    Image<bool> active(imSource.getSize());
    Image<bool> inQueue(imSource.getSize());

    inQueue.fill(false);
    std::queue<Point<TCoord > > fifo;

    int index=1;

    while(!pq.empty())
    {
        RGB value=pq.get();

        active.fill(true);

        int ncomposantes=0;

        for(int y=0; y<dy; y++)
            for(int x=0;x<dx; x++) {

                if(active(x,y) && order->islessequal(value,imSource(x,y))) {
                /** Construct a new node and begin a propagation
                **/
                    Point <TCoord> p(x,y);
                    inQueue.fill(false);

                    fifo.push(p);
                    ncomposantes++;
                    inQueue(p)=true;

                    Node *n=new Node(index,value,0);
                    index++;

                     if(index%100000==0)
                    {
                        std::cout << "Node : " << n->index << " Value: "<< (int)value[0] << "," << (int)value[1] << "," << (int)value[2] << "\n";
                    }

                    while(!fifo.empty()) {
                        Point<TCoord> curPt=fifo.front();
                        fifo.pop();

                        active(curPt)=false;

                        n->area++;
                        n->pixels.push_back(imSource.getOffset(curPt));
                        for(int i=0; i<connexity.getNbPoints(); i++) {
                            Point<TCoord> q=curPt+connexity.getPoint(i);

                            if(imSource.isPosValid(q)) {

                                if(inQueue(q)==false && order->islessequal(n->color,imSource(q)) ) {
                                    fifo.push(q);
                                    inQueue(q)=true;
                                }
                            }
                        }
                    }
                    nodes.push_back(n);
                }

            }
        if(ncomposantes!=0) {
        std::cout << "Level: " << (int)value[0] << " " << (int)value[1] << " " << (int)value[2] << "\n";
        std::cout << "N Composantes : " << ncomposantes << "\n";
        }
    }

    return nodes;
}



/**
* Compute component-graph G
* -compute all components
* -compute inclusion relations/ construct links
* -compute the transitive reduction
**/
int CGraph::computeGraphFull(CColorOrdering *order, CGraphWatcher *watcher)
{
    Image <RGB> im=this->imSource;
    OrderedQueue <RGB> pq;

    vector<Node *> nodes;

    vector <RGB> colorProcessed;

    /** Put all colors in priority queue
    **/
    for(int r=0; r<256; r++)
        for(int g=0; g<256; g++)
            for(int b=0; b<256; b++)
            {
                RGB value(r,g,b);
                pq.put(order->getPriority(value),value);

            }

    std::cout << "Begin compute components\n";
    clock_t c1=clock();

    nodes=computeComponents(order,pq);

    std::cout << "Number of nodes: " << nodes.size() << "\n";

     clock_t c2=clock();
    std::cout << "End compute components Time : " << (double)(c2-c1)*1000/CLOCKS_PER_SEC << "ms\n\n";


    std::cout << "Begin compute links\n";
    c1=clock();
    computeLinks(order,nodes);
    c2=clock();
    std::cout << "End compute links Time : " << (double)(c2-c1)*1000/CLOCKS_PER_SEC << "ms\n\n";

    for(int i=0; i<nodes.size(); i++)
        graph.push_back(nodes[i]);
    root=addRoot(nodes);

    writeDot("fulllinks.dot");
//    std::cout << "Begin compute transitive reduction\n";
//    c1=clock();
   computeTransitiveReduction(order, nodes);
//    c2=clock();
//    std::cout << "End compute transitive reduction time : " << (double)(c2-c1)*1000/CLOCKS_PER_SEC << "ms\n\n";



    writeDot("fullgraph.dot");
    return 0;
}

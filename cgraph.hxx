//Copyright (C) 2013, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.


#include "ragraph.h"
#include "cgraph.h"
#include "utils.h"

using namespace std;

template <>
Image<RGB> CGraph<RGB>::syntheticImage()
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

template <>
Image<RGB> CGraph<RGB>::syntheticImage2()
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


/**
*   Computation of component-graph \ddot G
 *  In this implementation, each node has a pointer to all rag regions belonging to the node. Therefore, all information of a node is contained in the node.
 *  Attributes can be computed for each node (contrary to a more memory efficient implementation in which each node contains only the proper regions of the node
 *  (of same value 'value')).
**/
template <class T>
int CGraph<T>::computeGraph(CGraphWatcher *watcher) {

    OrderedQueue <int> pq;

    vector<bool> processed(rag.vertices.size());

    //std::cout << rag->nodes.size() << "\n";

    for(int i=0; i<rag.vertices.size(); i++) {

        T value=rag.vertices[i].value;
		
        int priority=order.getPriority(value);

        // put in the prior. queue the flat zone number
        pq.put(priority,i);
        processed[i]=false;
    }

    // index from flat-zone number to corresponding node
    vector <Node *> regionToNode(rag.vertices.size());
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

        if(regionToNode[i]==0) {
            /** i est une région canonique (i==regionToNode[i]->index- */
            typename Rag<T>::Vertex curVertex;
            curVertex=rag.vertices[i];
            Node *curNode=new Node(curVertex.n,curVertex.value);
//			update_attributes(curNode,curVertex->pixels);
			
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
                typename Rag<T>::Vertex tmpVertex=rag.vertices[curpt];
                for(int v=0; v < tmpVertex.edges.size(); v++ ) {
                    int nb=tmpVertex.edges[v];
                    if(!infifo[nb] && order.islessequal(rag.vertices[i].value , rag.vertices[nb].value)) {
                        curNode->regions.push_back(nb);
                        if(order.isequal(rag.vertices[i].value , rag.vertices[nb].value)) {
                            /** ZP qui appartient au même noeud : on fusionne*/

                            regionToNode[nb]=curNode;

                            //std::cout << "Fusion de " << i << " et " << nb << "\n";
                        }
                        else{
                            /** zp potentiellement descendant du noeud courant : on teste si un père du noeud est comparable avec curNode
                        *  Si c'est le cas, a n'est pas un fils direct de curNode
                        */
                            Node *tmp=regionToNode[nb];

                            assert(tmp!=0);
                            //if(tmp!=0) {
                                bool isChild=true;
                                for(int a=0; a<tmp->fathers.size(); a++) {
                                    if(order.islessequal( curNode->value , tmp->fathers[a]->value) ) {
                                        isChild=false;
                                        break;
                                    }
                                }
                                if(isChild) {
                                    curNode->childs.push_back(tmp);
                                    tmp->fathers.push_back(curNode);
                                }
                            }

//                        curNode->area+=rag.vertices[nb]->pixels.size();

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

    T value(0);

    root=new Node(-1,value);

        for (int i=0; i<graph.size(); i++) {

            if(graph[i]->fathers.size()==0) {
                root->addChild(graph[i]);
            }
        }

    return 1;
}

//
//template <class T>
//void CGraph<T>::elongFiltering(int elongMin)
//{
//	for(int i=0; i<graph.size(); i++)
//	{
//        Node *n=graph[i];
//        if(n->elong<elongMin) n->active=false;
//    }
//
//    if(root->elong<elongMin) root->active=false;
//}

template <class T>
void CGraph<T>::areaFiltering(int areaMin)
{


    for(int i=0; i<graph.size(); i++) {
        Node *n=graph[i];
        if(n->area<areaMin) {
            n->active=false;
            n->active2=false;
            }
    }

    if(root->area<areaMin) {root->active=false; root->active2=false;}
}

template <class T>
void CGraph<T>::areaFiltering(int areaMin, int areaMax)
{

    for(int i=0; i<graph.size(); i++) {
        Node *n=graph[i];
        if(n->area<areaMin || n->area>areaMax) n->active=false;
    }

    if(root->area<areaMin || root->area>areaMax) root->active=false;
}

template <class T>
void CGraph<T>::contrastFiltering(int contrastMin)
{

    for(int i=0; i<graph.size(); i++) {
        Node *n=graph[i];
        if(n->contrast<contrastMin) { n->active=false; n->active2=false;}
    }

    if(root->contrast<contrastMin) {root->active=false; root->active2=false;}
}

template <class T>
void CGraph<T>::resetFiltering()
{
/** Initialisation*/
    for(int i=0; i<graph.size(); i++) {graph[i]->active=true;}
}

/**
    Keep the p % nodes having the biggest area
*/
template <class T>
void CGraph<T>::adaptiveAreaFiltering(int p)
{

    std::map<int, int> histo;
    for(int i=0; i<graph.size(); i++) {
        Node *n=graph[i];
        histo[-n->area]++;
    }

    histo[-root->area]++;

    unsigned long totalNodes=graph.size()+1;
    unsigned long bestNodes=(p*totalNodes)/100;

    int count=0;

    int threshold=-1;

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
template <class T>
void CGraph<T>::adaptiveContrastFiltering(int p)
{
    std::map<int, int> histo;
    for(int i=0; i<graph.size(); i++) {
        Node *n=graph[i];
        histo[-n->contrast]++;
    }

    histo[-root->contrast]++;

    unsigned long totalNodes=graph.size()+1;
    unsigned long bestNodes=(p*totalNodes)/100;

    int count=0;

    int threshold=-1;

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

template <class T>
void CGraph<T>::contrastFiltering(int contrastMin, int contrastMax)
{
    for(int i=0; i<graph.size(); i++) {
        Node *n=graph[i];
        if(n->contrast<contrastMin || n->contrast>contrastMax) n->active=false;
    }

    if(root->contrast<contrastMin || root->contrast>contrastMax) root->active=false;
}
//
//template <>
//Image<RGB> CGraph<RGB>::constructImageInf()
//{
//    std::queue<Node *> fifo;
//
//    vector <bool> active(graph.size());
//    active.assign(active.size(),true);
//
//    OrderedQueue <Node *> pq;
//
//    vector<bool> processed(graph.size());
//
//    RGB rgbmin(0,0,0);
//
//    /** Warning : nodes[0] does not exist !!!! */
//    for(int i=0; i<graph.size(); i++) {
//
//        graph[i]->dispColor=rgbmin;
//
//        RGB value=graph[i]->value;
//
//        int R=value[0];
//        int G=value[1];
//        int B=value[2];
//        pq.put(-(R+G+B),graph[i]);
//        processed[i]=false;
//    }
//
//    /** Special case for the root*/
//    root->dispColor=rgbmin;
//
//    while(!pq.empty()) {
//        Node *n=pq.get();
//
//        if(!n->active && n->fathers.size() > 1) {
//
//            int nactive=0;
//            RGB value(255,255,255);
//            for(int j=0; j<n->fathers.size(); j++) {
//                if(n->fathers[j]->active==true)
//                {
//                    nactive++;
//                    value[0]=std::min(value[0],n->fathers[j]->value[0]);
//                    value[1]=std::min(value[1],n->fathers[j]->value[1]);
//                    value[2]=std::min(value[2],n->fathers[j]->value[2]);
//                }
//            }
//            //qDebug() << "Node: " << n->label << " "<<  nactive << "\n";
//            if(nactive>1) {
//                for(int j=0; j<n->fathers.size(); j++) {
//
//                    if(n->fathers[j]->active==true) {
//                        n->fathers[j]->dispColor=value;
//                    }
//                }
//            }
//        }
//    }
//
//    Image<RGB> imRes=imSource;
//    imRes.fill(0);
//
//    for(int i=0; i<graph.size(); i++) {
//
//
//        RGB value=graph[i]->value;
//
//        int R=value[0];
//        int G=value[1];
//        int B=value[2];
//        pq.put((R+G+B),graph[i]);
//        processed[i]=false;
//    }
//
//    /** Special case for the root*/
//    pq.put(-1,root);
//
//    while(!pq.empty()) {
//        Node *curNode=pq.get();
//       //qDebug() << curNode->index << "(" << curNode->color[0] << "," << curNode->color[1] << ","  << curNode->color[2] << ") \n";
//        if(curNode->active && curNode->dispColor!=rgbmin) {
//            curNode->dispColor=curNode->value;
//        }
//
//        paintNode(imRes,curNode, curNode->dispColor);
//
//        for(int c=0; c<curNode->childs.size(); c++) {
//            Node *curChild=curNode->childs[c];
//            if(!curChild->active) {
////            if(curNode->dispColor>curChild->dispColor)
//               curChild->dispColor=curNode->dispColor;
//            }
//
//        }
//    }
//
//    return imRes;
//
//}
//
template <class T>
void CGraph<T>::constructNode(Image<T> &imRes, Node *n, T &value)
{
    for(int r=0; r<n->regions.size(); r++) {
        vector<int> points=rag.vertices[n->regions[r]].pixels;
        for(int p=0; p<points.size(); p++) {
            if(!order.islessequal(value, imRes(points[p])))
                imRes(points[p])=value;
        }
    }
}
//
//template <>
//void CGraph<RGB>::paintNodeSup(Image <RGB> &imRes, Node *n, RGB &value)
//{
//    for(int r=0; r<n->regions.size(); r++) {
//        assert(n->regions[r]!=0);
//        vector<Point<TCoord> > points=rag.vertices[n->regions[r]]->pixels;
//        for(int p=0; p<points.size(); p++) {
//            RGB imValue=imRes(points[p]);
//            imValue[0]=std::max(imValue[0],value[0]);
//            imValue[1]=std::max(imValue[1],value[1]);
//            imValue[2]=std::max(imValue[2],value[2]);
//
//            imRes(points[p])=imValue;
//        }
//    }
//}
//
//template <class T>
//Image<T> CGraph<T>::constructImage(Ordering<T> &order)
//{
//    Image<T> imRes=imSource;
//    imRes.fill(0);
//    std::queue<Node *> fifo;
//
//    OrderedQueue <Node *> pq;
//
//    vector<bool> processed(graph.size());
//
//    for(int i=0; i<graph.size(); i++) {
//
//        T value=graph[i]->value;
//
//        pq.put(-order.getPriority(value),graph[i]);
//        processed[i]=false;
//    }
//
//    /** Special case for the root*/
//    pq.put(-1,root);
//
//    while(!pq.empty()) {
//        Node *curNode=pq.get();
//        if(curNode->active ) {
//            curNode->dispColor=curNode->value;
//        }
//
//        constructNode(imRes,curNode, curNode->dispColor);
//
//        for(int c=0; c<curNode->childs.size(); c++) {
//            Node *curChild=curNode->childs[c];
//            if(!curChild->active) {
//               curChild->dispColor=curNode->dispColor;
//            }
//
//        }
//    }
//
//    return imRes;
//}


template <class T>
Image<T> CGraph<T>::constructImage(Ordering<T> &order)
{
    Image<T> imRes=imSource;
    imRes.fill(0);
    std::queue<Node *> fifo;

    OrderedQueue <Node *> pq;

    vector<bool> processed(graph.size());

    for(int i=0; i<graph.size(); i++) {

        T value=graph[i]->value;

        pq.put(-order.getPriority(value),graph[i]);
        processed[i]=false;
    }

    /** Special case for the root*/
    pq.put(-1,root);

    while(!pq.empty()) {
        Node *curNode=pq.get();
        if(curNode->active ) {
            constructNode(imRes,curNode, curNode->value);
        }
    }

    return imRes;
}

//
//template <>
//Image<RGB> CGraph<RGB>::constructImageInverse()
//{
//    Image<RGB> imRes=imSource;
//    imRes.fill(0);
//    std::queue<Node *> fifo;
//
//    OrderedQueue <Node *> pq;
//
//    vector<bool> processed(graph.size());
//
//    RGB rgbmin(0,0,0);
//
//    /** Warning : nodes[0] does not exist !!!! */
//    for(int i=0; i<graph.size(); i++) {
//
//        graph[i]->dispColor=rgbmin;
//
//        RGB value=graph[i]->value;
//
//        int R=value[0];
//        int G=value[1];
//        int B=value[2];
//        pq.put(-(R+G+B),graph[i]);
//        processed[i]=false;
//    }
//
//    /** Special case for the root*/
//    pq.put(-1,root);
//
//
//    while(!pq.empty()) {
//        Node *curNode=pq.get();
//
//        if(curNode->active==true ) {
//            curNode->dispColor=curNode->value;
//        }
//
//        paintNode(imRes,curNode, curNode->dispColor);
//
//        for(int c=0; c<curNode->childs.size(); c++) {
//            Node *curChild=curNode->childs[c];
//            if(curChild->active==false) {
////            if(curNode->dispColor>curChild->dispColor)
//               curChild->dispColor=curNode->dispColor;
//            }
//
//        }
//    }
//
//    return imRes;
//}



template <class T>
template <int N>
int CGraph<T>::writeDot(const char *filename)
{
    if(root!=0)
    {
        std::ofstream file(filename,std::ios_base::trunc);
        if(!file)
        {
            std::cerr << "File I/O error\n";
            return 0;
        }

        /** Maximum label of the graph **/
        unsigned long labelMax=graph.size()+1;
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
            nodeName << "\"" << tmp->index << ":(";
            for(int i=0; i<N; ++i) {
                nodeName << (int)tmp->value[i];
                if(i==N-1)
                    nodeName << " ";
                else nodeName << ",";
            }
            nodeName << " a=" << tmp->area << " c=" << tmp->compacity << ")\"";
            if(!tmp->active)
                file << "\t" << nodeName.str() << "[style=filled, fillcolor=grey];\n";

            for(int i=0; i<tmp->childs.size(); i++)
            {
                std::stringstream nodeNameChild;
                nodeNameChild << "\"" << tmp->childs[i]->index << ":(";
                for(int k=0; k<N; ++k) {
                    nodeNameChild << (int) tmp->childs[i]->value[k];
                    if (k == N - 1)
                        nodeNameChild << " ";
                    else nodeNameChild << ",";

                }
                nodeNameChild <<" a=" << tmp->childs[i]->area  << " c=" << tmp->childs[i]->compacity << ")\"";

                file << "\t" <<
                        nodeName.str() << "->" << nodeNameChild.str() << ";\n";

                if(isActive[tmp->childs[i]->index])
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


//template <class T>
//void CGraph<T>::computeAreaHelper(){
//    std::queue<Node *> fifo;
//    std::vector<bool> infifo(graph.size());
//
//    for(Node *n : graph) {
//        // compute area of node n
//        fifo.push(n);
//        n->area=0;
//        infifo.assign(infifo.size(),false);
//        while(!fifo.empty()){
//            // scan all the descendants
//            Node *tmp=fifo.front();
//            fifo.pop();
//
//            for(int r : tmp->regions) {
//                n->area+=rag.vertices[r].pixels.size();
//            }
//
//            for(Node *child : tmp->childs) {
//                // insert each node once
//                if(!infifo[child->index]) {
//                    fifo.push(child);
//                    infifo[child->index]=true;
//                }
//            }
//        }
//    }
//
//    // special processing the root which is not contained in graph[]
//    fifo.push(root);
//    root->area=0;
//    infifo.assign(infifo.size(),false);
//    while(!fifo.empty()){
//        // scan all the descendants
//        Node *tmp=fifo.front();
//        fifo.pop();
//
//        for(int r : tmp->regions) {
//            root->area+=rag.vertices[r].pixels.size();
//        }
//
//        for(Node *child : tmp->childs) {
//            // insert each node once
//            if(!infifo[child->index]) {
//                fifo.push(child);
//                infifo[child->index]=true;
//            }
//        }
//    }
//}


template <class T>
void CGraph<T>::computeAreaHelper(){

    for(Node *n : graph) {
        // compute area of node n
        n->area=0;
        for(int r : n->regions) {
            n->area+=rag.vertices[r].pixels.size();
        }
    }

    // special processing the root which is not contained in graph[]
    root->area=this->imSource.getBufSize();
}

// 2D only
template <class T>
void CGraph<T>::computeCompacityHelper(){

    int dx=imSource.getSizeX();
    int dy=imSource.getSizeY();

    for(Node *n : graph) {
        // compute compacity of node n
        n->compacity=0;
        int perimeter=0;
        // compute boundary of the node
        // a point is considered on the boundary if it has at least one neighboring point of inferior value or not comparable

        FlatSE connexity=rag.connexity;

        for(int r : n->regions) {
            for(int i=0; i< rag.vertices[r].pixels.size(); ++i) {
                int curPixel=rag.vertices[r].pixels[i];
                bool isBorder=false;
                Point<TCoord> p(curPixel%dx,curPixel/dx);
                for(int j=0; j<connexity.getNbPoints(); ++j) {
                    Point<TCoord> q=p+connexity.getPoint(j);
                    if(q.x>=0 && q.x<dx && q.y>=0 && q.y<dy ) {

                        // check for border : compare n->value (value of the node) and not imSource(p) (value of the current point)
                        if(!order.islessequal(n->value,imSource(q)))
                            isBorder=true;
                    }
                    else
                        isBorder=true;
                }

                if(isBorder) perimeter++;
            }
        }
        // 4pi unnecessary (discretization effects)

        n->compacity=100*(n->area)/(perimeter*perimeter);
    }
}

template<class T>
template <int N>
int CGraph<T>::diff_L1(T v1, T v2){
    int dist=0;
    for(int i=0; i<N; ++i) {
        dist+=std::abs(v1[i]-v2[i]);
    }
    return dist;
}

template<class T>
template <int N>
int CGraph<T>::diff_L2(T v1, T v2){
    int dist=0;
    for(int i=0; i<N; ++i) {
        dist+=(v1[i]-v2[i])*(v1[i]-v2[i]);
    }
    return dist;
}

template<class T>
template<int N>
void CGraph<T>::computeFelsContrastHelper() {

    int dx=imSource.getSizeX();
    int dy=imSource.getSizeY();

    FlatSE connexity=rag.connexity;

    for(Node *n : graph) {
        n->fels_contrast=0;
        int dist_min=256*3;
        int dist_max=0;

        for(int r : n->regions) {
            for (int i = 0; i < rag.vertices[r].pixels.size(); ++i) {
                int curPixel = rag.vertices[r].pixels[i];
                Point<TCoord> p(curPixel % dx, curPixel / dx);
                for (int j = 0; j < connexity.getNbPoints(); ++j) {
                    Point<TCoord> q = p + connexity.getPoint(j);
                    if (q.x >= 0 && q.x < dx && q.y >= 0 && q.y < dy) {

                        // point on a border : we check the min wrt the neighborhood between imSource(p) and imSource(q)
                        // note : n->value could be != than imSource(p)
                        if (!order.islessequal(n->value, imSource(q))) {
                            int dist = diff_L1<3>(imSource(p), imSource(q));
                            dist_min = std::min(dist, dist_min);
                        } else
                            // check for dist_max btw nghb points of the node
                        {
                            int dist = diff_L1<3>(imSource(p), imSource(q));
                            dist_max = std::max(dist, dist_max);
                        }
                    }
                }
            }
        }
        n->fels_contrast=dist_min-dist_max;
        n->compacity=n->fels_contrast;
    }

    root->fels_contrast=0;
}

template<class T>
int CGraph<T>::computeAttributes() {

    computeAreaHelper();
    // area must be computed before compacity
    computeCompacityHelper();
    // int N
    computeFelsContrastHelper<3>();

    return 0;
}

template <class T>
void CGraph<T>::setShapingAttribute(const std::string &attrname)
{
    if(attrname=="area") {
        for (Node *n : graph ){
            n->attr=n->area;
        }
    }

    if(attrname=="compacity") {
        for (Node *n : graph ){
            n->attr=n->compacity;
        }
    }

    if(attrname=="inverse compacity") {
        for (Node *n : graph ){
            n->attr=-n->compacity;
        }
    }

    if(attrname=="fels contrast") {
        for (Node *n : graph ){
            n->attr=-n->fels_contrast;
        }
    }

}






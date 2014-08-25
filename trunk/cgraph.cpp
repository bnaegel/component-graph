//Copyright (C) 2013, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.


#include "ragraph.h"
#include "cgraph.h"

using namespace std;


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


/**
*   Computation of component-graph \ddot G
**/
int CGraph::computeGraph(ColorOrdering *order, CGraphWatcher *watcher) {

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
*   Computation of component-graph \ddot G using inverse order
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

Image<RGB> CGraph::constructImage(ColorOrdering *order)
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



/** check if set n is equal to m */
bool CGraph::isEqual(Node *n, Node *m) {

    if(n->area!=m->area) return false;

    for(int i=0; i<n->pixels.size(); i++) {
        TOffset curPixel=n->pixels[i];
        bool curPixelIncluded=false;
        for(int j=0; j<m->pixels.size(); j++) {
            if(curPixel==m->pixels[j])
                {
                curPixelIncluded=true;
                }
        }
        if(curPixelIncluded==false) return false;
    }

    return true;
}



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

void CGraph::computeLinks(ColorOrdering *order, vector <Node *> &nodes)
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


vector <CGraph::Node *> CGraph::minimalElements(ColorOrdering *order, vector <Node *> &nodes, vector <bool> &tmp) {
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
void CGraph::computeTransitiveReduction(ColorOrdering *order, vector<Node *> &nodes)
{
    vector<bool> tmp(imSource.getBufSize(),false);
    for(int i=0; i<nodes.size(); i++) {
        Node *curNode=nodes[i];
        curNode->childs=minimalElements(order,curNode->childs, tmp);
    }
}

/** Compute the nodes for G and \dot G component-graph
**/
vector<CGraph::Node *> CGraph::computeComponents(ColorOrdering *order, OrderedQueue <RGB> &pq)
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
int CGraph::computeGraphFull(ColorOrdering *order, CGraphWatcher *watcher)
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

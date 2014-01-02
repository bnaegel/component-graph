#include "core.h"

int CGraph::Node::nbLinks=0;
int CGraph::Node::nbNodes=0;
int CGraph::Node::count=0;

namespace CGraph {




Core::Core(Image<RGB> &imSource) : imSource(imSource)
{


}

int Core::colorToIndex(RGB &value)
{
    return value[2]+value[1]*256+value[0]*256*256;
}


RGB Core::indexToColor(int &colorKey)
{
    RGB value;
    value[0]=colorKey/65536;
    colorKey=colorKey-value[0]*65536;
    value[1]=colorKey/256;
    colorKey=colorKey-value[1]*256;
    value[2]=colorKey;

    return value;
}


void Core::printColor(RGB value)
{
    std::cout << "(" << (int)value[0] << "," << (int)value[1] << "," << (int)value[2] << ")" << endl;
}


void Core::printVectorNodes(vector <Node *> v)
{
    for(int i=0; i<v.size(); i++)
        {
        std::cout << "Node: " << v[i]->label <<"\n";
        }
    std::cout << "\n";
}


// return an index on canonical node
int Core::findNode(int n, vector<int> &par)
{

    if(par[n]!=n)
        {
        par[n]=findNode(par[n],par);
        }

    return par[n];
}

// return an index on canonical node
int Core::findNodeExp(int n, vector<int> &par)
{
    assert(n!=-1);
    if(par[n]!=n && par[n]!=-1)
        {
        par[n]=findNodeExp(par[n],par);
        }

    return par[n];
}

// return an index on canonical nodes
vector <int> Core::findNode(int n, vector<vector <int> > &par)
{

//    if(par[n]!=n)
//        {
//        par[n]=findNode(par[n],par);
//        }

    return par[n];
}

int Core::linkNode(int n, int m, vector<int> &par, vector <int> &rank)
{
    if(rank[n]>rank[m]) {int tmp=n; n=m; m=tmp;}
    else if(rank[n]==rank[m]) rank[m]++;
    par[n]=m;
    return m;
}

void Core::link(int n,int m,vector<int> &par, vector <int> &rank)
{
    par[n]=m;
}

void Core::makeSet(int n, vector<int> &par, vector<int> &rank)
{
    par[n]=n;
    rank[n]=0;
}

void Core::makeSet(int n, vector<vector <int> > &par)
{
    par[n].push_back(n);
}


Node *Core::makeNode(RGB value, int rep)
{
    Node *n;
    n=new Node(value, rep);
    return n;
}

void Core::mergeNodes(Node *finalNode, Node *inputNode)
{
    for(int i=0; i<inputNode->childs.size(); i++)
        {
        Node *tmp=inputNode->childs[i];
        finalNode->addChild(tmp);

        //finalNode->rep=inputNode->rep;
        }
    inputNode->isPresent=false;
    inputNode->childs.clear();
    finalNode->area+=inputNode->area;
    //std::cout << "Noeud " << finalNode->label << " nbr de fils: " << finalNode->childs.size() << "\n";
}

void Core::printPar(vector<int> par)
{
    for(int i=0; i<par.size(); i++)
        {
            std::cout << "Pixel: " << i << " Parent: " << par[i] << std::endl;;
        }
}

int Core::writeDot(Node *root, const char *filename)
{
    if(root!=0)
        {
        std::ofstream file(filename,std::ios_base::trunc  | std::ios_base::binary);
        if(!file)
            {
            std::cerr << "File I/O error\n";
            return 0;
            }

        // Node::count représente le label maximum du réseau
        int labelMax=Node::count+1;
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
            nodeName << "\"" << tmp->label << ":(" <<(int)tmp->value[0] << "," <<
            (int)tmp->value[1]<<","<<
            (int)tmp->value[2]<< " c=" << tmp->contrast << ")\"";
            for(int i=0; i<tmp->childs.size(); i++)
                    {
                    std::stringstream nodeNameChild;
                    nodeNameChild << "\"" << tmp->childs[i]->label << ":(" <<(int)tmp->childs[i]->value[0] << "," <<
                    (int)tmp->childs[i]->value[1]<<","<<
                    (int)tmp->childs[i]->value[2]<< " c=" << tmp->childs[i]->contrast <<  ")\"";

                    file << "\t" <<
                    nodeName.str() << "->" << nodeNameChild.str() << ";\n";

                    if(isActive[tmp->childs[i]->label]==true)
                        {
                        fifo.push(tmp->childs[i]);
                        isActive[tmp->childs[i]->label]=false;
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

int Core::writeDot(const char *filename)
{
   std::ofstream file(filename,std::ios_base::trunc  | std::ios_base::binary);
   if(!file)
       {
       std::cerr << "File I/O error\n";
       return 0;
       }

        file << "digraph G {\n";

        set<int>::iterator it;
        for(int i=0; i<imSource.getBufSize(); i++) {
            if(par[i]==i) {

            std::stringstream nodeName;
            nodeName.str("");
            nodeName << "\"" << i << ":(" << (int)imSource(i)[0] <<  "," <<    (int)imSource(i)[1]<<","<<(int)imSource(i)[2] <<")\"";
            for(it=graph[i].begin(); it!=graph[i].end(); it++) {
                 std::stringstream nodeNameFather;
                 nodeNameFather.str("");
                 int f=findNode(*it,par);
                 nodeNameFather << "\"" << f << ":(" << (int)imSource(f)[0] <<  "," <<    (int)imSource(f)[1]<<","<<(int)imSource(f)[2] <<")\"";
                 file << "\t" <<
                 nodeNameFather.str() << "->" << nodeName.str() << ";\n";
            }

        }
        }

        file << "}\n";

        file.close();
        return 1;
}

// find the set (a_1,...,a_n) of all minimal ancestors of n
// with \forall i,j  a_i and a_j are not comparable (i!=j) and a_i >= v
void Core::findMinAncestors(Node *n, RGB v, vector<Node *> &ancestors)
{
    std::queue<Node *> fifo;

    //for(int i=0; i<active.size(); i++) {active[i]=true; isAncestor[i]=true;}
    fifo.push(n);

    while(!fifo.empty())
        {
        Node *curNode=fifo.front();
        fifo.pop();

        for(int i=0; i<curNode->fathers.size(); i++)
            {
            Node *tmp=curNode->fathers[i];
            //std::cout << "Nbr of fathers: " << curNode->fathers.size() << "\n";
            //si le noeud existe on le traite, sinon on passe au suivant
            //(fathers contient tous les noeuds père, on ne les efface pas au fur et à mesure
            // pour gagner du temps)
            if(tmp->isPresent==true)
                {
                if(v <=tmp->value  && tmp->active==true)
                    {
                    fifo.push(tmp);
                    tmp->active=false; //tmp est dans la file, il n'est plus actif
                    }
            // tmp est dans la file: le noeud courant n'est pas dans la liste des ancetres
                if(tmp->active==false)
                    {
                    curNode->isAncestor=false;
                    }
                }
            }
            //aucun parent n'a été mis dans la file
            if(curNode->isAncestor==true)
                {
                ancestors.push_back(curNode);
                curNode->isAncestor=false;
                }
        // le noeud courant sort de la file: on peut réinitialiser son flag isAncestor pour le prochain parcours
        //il redevient actif (pour un prochain parcours)
        curNode->active=true;
        curNode->isAncestor=true;
        }
}


// find the set (a_1,...,a_n) of all minimal ancestors of n
// with \forall i,j  a_i and a_j are not comparable (i!=j) and a_i >= v
void Core::findMinAncestors2(Node *n, RGB v, vector <bool> &active, vector <bool> &isAncestor, vector<Node *> &ancestors)
{
    std::queue<Node *> fifo;

    for(int i=0; i<active.size(); i++) {active[i]=true; isAncestor[i]=true;}
    fifo.push(n);

    while(!fifo.empty())
        {
        Node *curNode=fifo.front();
        fifo.pop();

        for(int i=0; i<curNode->fathers.size(); i++)
            {
            Node *tmp=curNode->fathers[i];
            //std::cout << "Nbr of fathers: " << curNode->fathers.size() << "\n";
            //si le noeud existe on le traite, sinon on passe au suivant
            //(fathers contient tous les noeuds père, on ne les efface pas au fur et à mesure
            // pour gagner du temps)
            if(tmp->isPresent==true)
                {
                if(v <=tmp->value  && active[tmp->rep]==true)
                    {
                    fifo.push(tmp);
                    active[tmp->rep]=false; //tmp est dans la file, il n'est plus actif
                    }
            // tmp est dans la file: le noeud courant n'est pas dans la liste des ancetres
                if(active[tmp->rep]==false)
                    {
                    isAncestor[curNode->rep]=false;
                    }
                }
            }
            //aucun parent n'a été mis dans la file
            if(isAncestor[curNode->rep]==true)
                {
                ancestors.push_back(curNode);
                isAncestor[curNode->rep]=false;
                }
        }
}


void Core::findMinAncestors3(Node *n, vector<Node *> &ancestors)
{
   //ancestors=n->ancestors;
}


// find the set (a_1,...,a_n) of all minimal ancestors of adjNodes
// with \forall i,j  a_i and a_j are not comparable (i!=j) and a_i >= v
void Core::findMinAncestors4( vector <Node *> &adjNodes, RGB v, vector <bool> &active, vector <bool> &isAncestor, vector<Node *> &ancestors)
{
    std::queue<Node *> fifo;

    for(int i=0; i<active.size(); i++) {active[i]=true; isAncestor[i]=true;}

    for(int i=0; i<adjNodes.size(); i++)
        fifo.push(adjNodes[i]);

    while(!fifo.empty())
        {
        Node *curNode=fifo.front();
        fifo.pop();

        for(int i=0; i<curNode->fathers.size(); i++)
            {
            Node *tmp=curNode->fathers[i];
            //std::cout << "Nbr of fathers: " << curNode->fathers.size() << "\n";
            //si le noeud existe on le traite, sinon on passe au suivant
            //(fathers contient tous les noeuds père, on ne les efface pas au fur et à mesure
            // pour gagner du temps)
            if(tmp->isPresent==true)
                {
                if(v <=tmp->value  && active[tmp->rep]==true)
                    {
                    fifo.push(tmp);
                    active[tmp->rep]=false; //tmp est dans la file, il n'est plus actif
                    }
            // tmp est dans la file: le noeud courant n'est pas dans la liste des ancetres
                if(active[tmp->rep]==false)
                    {
                    isAncestor[curNode->rep]=false;
                    }
                }
            }
            //aucun parent n'a été mis dans la file
            if(isAncestor[curNode->rep]==true)
                {
                ancestors.push_back(curNode);
                isAncestor[curNode->rep]=false;
                }
        }
}



Node *Core::componentGraph(Image <RGB> &im, FlatSE &connexity)
{

    OrderedQueue <int> pq;
    std::queue <Node *> fifo;

    vector <int> rank(im.getBufSize());
    vector <bool> processed(im.getBufSize());
    vector <bool> active(im.getBufSize());
    vector <bool> isAncestor(im.getBufSize());
    this->par.resize(im.getBufSize());
    this->nodes.resize(im.getBufSize());
    int imSize=im.getBufSize();

    for(int i=0; i<im.getBufSize(); i++)
        {
        RGB value=im(i);
        makeSet(i,par,rank);
        nodes[i]=makeNode(value,i);

        int R=value[0];
        int G=value[1];
        int B=value[2];
        pq.put(-(R+G+B),i);
        processed[i]=false;
        }
    int N=0;
    while(!pq.empty())
        {
        int i=pq.get();
        N++;
         //std::cout << "Taille de la file: " << pq.size() << "\n";
        int curNode=i;
        //le pt i n'a pas encore traité donc findNode=par(i)=i
        assert(curNode==findNode(curNode,par));
        assert(i==findNode(curNode,par));

        Point <TCoord> p=im.getCoord(i);
       if (N%1000==0)
            qDebug() << "Point courant: " << N << "/" << imSize <<  "\n";
       //std::cout << "Point courant: " << i <<  "\n";
       //  std::cout << "Nb links: " << Node::nb_links << "\n";

        for(int j=0; j<connexity.getNbPoints(); j++)
            {
            Point<TCoord>  q=p+connexity.getPoint(j);
            if(im.isPosValid(q))
                {

                int qoff=im.getOffset(q);
                if (processed[qoff]==true)
                {
                //std::cout << "Voisin explore: " << qoff << "\n";

                // on cherche l'élt canonique de q (représentant du noeud)
                int adjNode=findNode(qoff,par);
               // std::cout << "Elt canonique: " << adjNode << "\n";
                assert(im(adjNode)==im(qoff));
                // f(q)=f(p): q appartient au noeud de p

                if(im(curNode)==im(adjNode) && curNode!=adjNode)
                    {
                    //curNode devient élt canonique
                    par[adjNode]=curNode;
                    //les fils de adjNode sont ajoutés aux fils de curNode
                    assert(nodes[curNode]->value==nodes[adjNode]->value);
                    mergeNodes(nodes[curNode],nodes[adjNode]);
                    //qDebug() << curNode << "-->" << adjNode << " - " <<  nodes[curNode]->area << "\n";
//                    curNode=linkNode(curNode,adjNode,par,rank);
                    }


                // p est un ancêtre de q
                // mais il faut:
                // -vérifier quels ancêtres a_i de q deviendront les fils de p
                // -vérifier si un ancêtre a_i de q n'a pas la même valeur de p, auquel cas p et a_i appartiennent au même noeud
                // si p et q sont non comparables il n'y a rien à faire: on sait que aucun ancêtre de q ne peut exister tel que f(p)<f(a_i)
                else if(im(curNode)<im(adjNode))
                    {
                    assert(nodes[adjNode]->isPresent);
                    // on cherche les ancêtres minimaux de adjNode tels que f(a_i) >= f(p)
                    // grâce à l'ordre de traitement des pts, on sait que tous les ancêtres de q sont >, = ou non comparables avec f(p)
                    vector <Node *> ancNodes;
                    //ancNodes.push_back(nodes[adjNode]);
                    //findMinAncestors(nodes[adjNode], im(curNode), ancNodes);
                    findMinAncestors2(nodes[adjNode],  im(curNode), active, isAncestor, ancNodes);
                    //findMinAncestors3(nodes[adjNode],ancNodes);
                    //std::cout << "Ancetres: ";
                    //printVectorNodes(ancNodes);
                    for(int k=0; k<ancNodes.size(); k++)
                        {
                        Node *tmp=ancNodes[k];
                        // on regarde quel est l'élt canonique du noeud
                        tmp->rep=findNode(tmp->rep,par);
                        //assert(tmp==nodes[tmp->rep]);
                        //nodes[curNode] et tmp appartiennent au même noeud
                        if(im(curNode)==tmp->value && curNode!=tmp->rep)
                            {
//                          curNode=linkNode(curNode,tmp->rep,par,rank);
                            par[tmp->rep]=curNode;
                            mergeNodes(nodes[curNode],tmp);
                           // qDebug() << i << "--->" << tmp->rep << " - " <<  nodes[curNode]->area << "\n";
                            //std::cout <<  i << "=" << tmp->rep << "\n";
                            }
                        //nodes[curNode] est un ancêtre de tmp
                        else if(im(curNode)<tmp->value )
                            {
                            nodes[curNode]->addChild(tmp);
                           // qDebug() << i << "->" << tmp->rep << " - " <<  nodes[curNode]->area << "\n";
                            }
                        }

                    }
                }
            }
        }

        processed[i]=true;
    }



//    printVectorNodes(findMinAncestors(nodes[0]));
//    printVectorNodes(findMinAncestors(nodes[50]));
    RGB inf;
    inf[0]=0;
    inf[1]=0;
    inf[2]=0;
    int aNode=findNode(0,par);
    vector <Node *> roots;
    //findMinAncestors(nodes[aNode],inf,roots);
    findMinAncestors2(nodes[aNode],inf,active,isAncestor,roots);
    //findMinAncestors3(nodes[aNode],roots);
    // Noeud racine fictif (au cas où il y a plusieurs "racines")
    // au niveau inf
    Node *root=new Node(inf,-1);
    root->area =0;
    for(int i=0; i<roots.size(); i++)
        {
        root->addChild(roots[i]);

        }
    assert(root->fathers.size()==0);
//    std::cout << root->rep << "\n";
//    printColor(root->value);
//    printVectorNodes(root->childs);

//    printPar(par);
    int nb_nodes=1;
    for(int i=0; i<nodes.size(); i++)
        {
        if(nodes[i]->isPresent) Node::nbNodes++;
        }

    return root;
}

std::vector <int> Core::lowestNodes(Image<RGB> &im, int p,  vector <int> &par, vector <vector <int> > &graph , vector <vector <int> > &minanc, RGB &v)
{
    //p: representant du noeud
    std::vector <int> active(im.getBufSize());
    for(int i=0; i<active.size(); i++)
        active[i]=true;
    std::vector<int> nodes;
    nodes.push_back(p);

    std::queue<int> fifo;
    fifo.push(p);

    while(!fifo.empty()) {
        int a=findNode(fifo.front(),par);
        fifo.pop();

        if(!graph[a].empty()) {
            for(int i=0; i<graph[a].size(); i++) {
                int q=findNode(graph[a][i],par);
                if(active[q] && v<=im(q)) {
                    fifo.push(q);

                    active[q]=false;
                    }
            }
        }

        else nodes.push_back(a);
    }

    return nodes;
}

vector <int> Core::lowestNodesExp(Image <RGB> &im, RGB v, int q, vector <int > &par)
{
//    static int count=0;
//    count++;
//    qDebug() << "Niveau: " << (int)v[0]  << " " << (int)v[1] << " " << (int)v[2] << " Lowest Nodes " <<  count << "\n" ;
    std::queue <int> fifo;
    fifo.push(q);

    set<int>::iterator it;
    //vector<int> active(im.getBufSize(),true);
    //vector<int> isAncestor(im.getBufSize(),true);
    active.assign(active.size(), true);
    isAncestor.assign(isAncestor.size(), true);
    vector<int> lowestNodes;

    active[q]=false;

    while(!fifo.empty()) {
        int p=fifo.front();
        fifo.pop();

        for ( it=graph[p].begin() ; it != graph[p].end(); it++ ) {
            int q=findNode(*it,par);
            if(im(q) >= v && active[q]) {
                fifo.push(q);
                active[q]=false;
            }
            if(active[q]==false) {
                isAncestor[p]=false;
            }
        }
        if(isAncestor[p]==true) {
            lowestNodes.push_back(p);
        }
        //active[p]=true;
        //isAncestor[p]=true;
    }

    return lowestNodes;
}


vector <int> Core::lowestNodesExpOpti(Image <RGB> &im, RGB v, int q, vector <int > &par)
{
    std::queue <int> fifo;

    set<int>::iterator it;
    vector<int> active(im.getBufSize(),true);
    vector<int> isAncestor(im.getBufSize(),true);
    active.assign(active.size(), true);
    isAncestor.assign(isAncestor.size(), true);
    vector<int> lowestNodes;

    if(this->lowestNodesTab[q].find(colorToIndex(v))==lowestNodesTab[q].end())
        fifo.push(q);
    else {
         return this->lowestNodesTab[q][colorToIndex(v)];

    }




    while(!fifo.empty()) {
        int p=fifo.front();
        fifo.pop();

        for ( it=graph[p].begin() ; it != graph[p].end(); it++ ) {
            int q=findNode(*it,par);
            if(im(q) >= v && active[q]) {
                fifo.push(q);
                active[q]=false;
            }
            if(active[q]==false) {
                isAncestor[p]=false;
            }
        }
        if(isAncestor[p]==true) {
            lowestNodes.push_back(p);
        }
        //active[p]=true;
        //isAncestor[p]=true;
    }


    this->lowestNodesTab[q][colorToIndex(v)]=lowestNodes;
    return lowestNodes;

}

void Core::componentGraphExp(Image <RGB> &im, FlatSE &connexity)
{
    OrderedQueue <int> pq;
    std::queue <Node *> fifo;


    vector <int> rank(im.getBufSize());
    //vector <vector <int> > graph(im.getBufSize());
    //vector <vector <int> > minanc(im.getBufSize());
    graph.clear();
    par.clear();
    active.clear();
    isAncestor.clear();
    graph.resize(im.getBufSize());
    par.resize(im.getBufSize());
    active.resize(im.getBufSize());
    isAncestor.resize(im.getBufSize());

    lowestNodesTab.clear();
    lowestNodesTab.resize(im.getBufSize());

    vector <bool>   processed(im.getBufSize());

    active.assign(active.size(), true);
    isAncestor.assign(isAncestor.size(),true);

    //tri des points par valeur décroissante
    for(int i=0; i<im.getBufSize(); i++)
        {
        RGB value=im(i);
        makeSet(i,par,rank);
        //makeSet(i,minanc);

        int R=value[0];
        int G=value[1];
        int B=value[2];
        pq.put(-(R+G+B),i);
        processed[i]=false;
        }
    int N=0;
    int imSize=im.getBufSize();

    while(!pq.empty()) {
        int i=pq.get();
        N++;
        if (N%1000==0)
            qDebug() << "Point courant: " << N << "/" << imSize <<  "\n";
        Point <TCoord> p=im.getCoord(i);
        for(int j=0; j<connexity.getNbPoints(); j++) {
            Point <TCoord> q=p+connexity.getPoint(j);
            if(im.isPosValid(q)) {
                int qoff=im.getOffset(q);
                if (processed[qoff]==true) {
                    // on cherche l'élt canonique de q (représentant du noeud)
                    int adjNode=findNode(qoff,par);
                    //i et adjNode appartiennent à la même zone plate
                    if(i!=adjNode && im(i)==im(adjNode) ) {
                        //i devient élt canonique pour la zp courante
                        par[adjNode]=i;
                    }
                    else

                        if(im(i)<im(adjNode)) {
                        // i est un ancêtre de adjNode
                        // mais il faut:
                        // -vérifier quels ancêtres a de adjNode deviendront les fils de i
                        // -vérifier si un ancêtre a de adjNode n'a pas la même valeur que i, auquel cas i et a appartiennent au même noeud
                        // si i et adjNode sont non comparables il n'y a rien à faire: on sait que aucun ancêtre de adjNode ne peut exister tel que f(i)<f(a)
                        //vector <int> nodes=this->lowestNodes(im,adjNode,par,graph,minanc,im(adjNode));
                        vector <int> nodes=this->lowestNodesExp(im, im(i),adjNode,par);
                        for(int k=0; k<nodes.size(); k++) {
                            if(im(i)<im(nodes[k])) {
                                graph[nodes[k]].insert(i);
                            }
                            else if (im(i)==im(nodes[k])) {

                                if(nodes[k]!=i) par[nodes[k]]=i;
                            }
                        }

                    }

                }

            }

        }
    processed[i]=true;
    }

}



Node *Core::componentGraph2(Image <RGB> &im, FlatSE &connexity)
{
    OrderedQueue <int> pq;
    std::queue <Node *> fifo;

    vector <int>    par(im.getBufSize());
    vector <int>    rank(im.getBufSize());
    vector <bool>   processed(im.getBufSize());
    vector <bool>   active(im.getBufSize());
    vector <bool>   isAncestor(im.getBufSize());
    vector <Node *> nodes(im.getBufSize());
    int imSize=im.getBufSize();
    //
    for(int i=0; i<im.getBufSize(); i++)
        {
        RGB value=im(i);
        makeSet(i,par,rank);
        nodes[i]=makeNode(value,i);

        int R=value[0];
        int G=value[1];
        int B=value[2];
        pq.put(-(R+G+B),i);
        processed[i]=false;
        }
    int N=0;
    while(!pq.empty())
        {
        int i=pq.get();
        N++;
        //std::cout << "Taille de la file: " << pq.size() << "\n";
        int curNode=i;
        //le pt i n'a pas encore traité donc findNode=par(i)=i
        assert(curNode==findNode(curNode,par));
        assert(i==findNode(curNode,par));

        Point <TCoord> p=im.getCoord(i);
        if (N%1000==0)
        std::cout << "Point courant: " << N << "/" << imSize <<  "\n";

        //  std::cout << "Nb links: " << Node::nb_links << "\n";
        vector <Node *> adjNodes;
        for(int j=0; j<connexity.getNbPoints(); j++)
            {
            Point<TCoord>  q=p+connexity.getPoint(j);
            if(im.isPosValid(q))
                {

                int qoff=im.getOffset(q);
                if (processed[qoff]==true)
                    {
                    //std::cout << "Voisin explore: " << qoff << "\n";

                    // on cherche l'élt canonique de q (représentant du noeud)
                    int adjNode=findNode(qoff,par);
                   // std::cout << "Elt canonique: " << adjNode << "\n";
                    assert(im(adjNode)==im(qoff));
                    // f(q)=f(p): q appartient au noeud de p

                    if(im(curNode)==im(adjNode) && curNode!=adjNode)
                        {
                        //curNode devient élt canonique
                        par[adjNode]=curNode;
                        //les fils de adjNode sont ajoutés aux fils de curNode
                        assert(nodes[curNode]->value==nodes[adjNode]->value);
                        mergeNodes(nodes[curNode],nodes[adjNode]);
    //                    curNode=linkNode(curNode,adjNode,par,rank);
                        }


                    // p est un ancêtre de q
                    // mais il faut:
                    // -vérifier quels ancêtres a_i de q deviendront les fils de p
                    // -vérifier si un ancêtre a_i de q n'a pas la même valeur de p, auquel cas p et a_i appartiennent au même noeud
                    // si p et q sont non comparables il n'y a rien à faire: on sait que aucun ancêtre de q ne peut exister tel que f(p)<f(a_i)
                    else if(im(curNode)<im(adjNode))
                        {
                        assert(nodes[adjNode]->isPresent);
                        adjNodes.push_back(nodes[adjNode]);
                        }
                    }

                }
            }
        for(int k=0; k<adjNodes.size(); k++)
            {
            // on cherche les ancêtres minimaux de adjNode tels que f(a_i) >= f(p)
            // grâce à l'ordre de traitement des pts, on sait que tous les ancêtres de q sont >, = ou non comparables avec f(p)
            vector <Node *> ancNodes;
            //ancNodes.push_back(nodes[adjNode]);
            //findMinAncestors(nodes[adjNode], im(curNode), ancNodes);
            //findMinAncestors2(nodes[adjNode],  im(curNode), active, isAncestor, ancNodes);
            findMinAncestors4(adjNodes,im(p), active, isAncestor, ancNodes);
            //std::cout << "Ancetres: ";
            //printVectorNodes(ancNodes);
            for(int k=0; k<ancNodes.size(); k++)
                {
                Node *tmp=ancNodes[k];
                // on regarde quel est l'élt canonique du noeud
                tmp->rep=findNode(tmp->rep,par);
                //assert(tmp==nodes[tmp->rep]);
                //nodes[curNode] et tmp appartiennent au même noeud
                if(im(curNode)==tmp->value && curNode!=tmp->rep)
                    {
                    //curNode=linkNode(curNode,tmp->rep,par,rank);
                    par[tmp->rep]=curNode;
                    mergeNodes(nodes[curNode],tmp);
                    //std::cout <<  i << "=" << tmp->rep << "\n";
                    }
                    //nodes[curNode] est un ancêtre de tmp
                else if(im(curNode)<tmp->value )
                    {
                    //std::cout << i << "->" << tmp->rep << "\n";
                    nodes[curNode]->addChild(tmp);
                    }
                }

            }

        processed[i]=true;
        }

    Image<U16> imRes(im.getSize());
    for(int i=0; i<imRes.getBufSize(); i++)
        {
        imRes(i)=findNode(i,par);

        }

//    printVectorNodes(findMinAncestors(nodes[0]));
//    printVectorNodes(findMinAncestors(nodes[50]));
    RGB inf;
    inf[0]=0;
    inf[1]=0;
    inf[2]=0;
    int aNode=findNode(0,par);
    vector <Node *> roots;
    //findMinAncestors(nodes[aNode],inf,roots);
    //findMinAncestors2(nodes[aNode],inf,active,isAncestor,roots);
    findMinAncestors3(nodes[aNode],roots);
    // Noeud racine fictif (au cas où il y a plusieurs "racines")
    // au niveau inf
    Node *root=new Node(inf,-1);
    for(int i=0; i<roots.size(); i++)
        {
        root->addChild(roots[i]);
        }
//    std::cout << root->rep << "\n";
//    printColor(root->value);
//    printVectorNodes(root->childs);

//    printPar(par);
    int nb_nodes=1;
    for(int i=0; i<nodes.size(); i++)
        {
        if(nodes[i]->isPresent) nb_nodes++;
        }


    return root;
}


//on stocke tous les liens puis on fait une réduction transitive...
Node *Core::componentGraph3(Image <RGB> &im, FlatSE &connexity)
{
    OrderedQueue <int> pq;
    std::queue <Node *> fifo;

    vector <int> par(im.getBufSize());
    vector <int> rank(im.getBufSize());
    vector <bool> processed(im.getBufSize());
    vector <Node *> nodes(im.getBufSize());
    int imSize=im.getBufSize();

    for(int i=0; i<im.getBufSize(); i++)
        {
        RGB value=im(i);
        makeSet(i,par,rank);
        nodes[i]=makeNode(value,i);

        int R=value[0];
        int G=value[1];
        int B=value[2];
        pq.put(-(R+G+B),i);
        processed[i]=false;
        }
    int N=0;
    while(!pq.empty())
        {
        int i=pq.get();
        N++;
         //std::cout << "Taille de la file: " << pq.size() << "\n";
        int curNode=i;
        //le pt i n'a pas encore traité donc findNode=par(i)=i
        assert(curNode==findNode(curNode,par));
        assert(i==findNode(curNode,par));

        Point <TCoord> p=im.getCoord(i);
        if (N%1000==0)
         std::cout << "Point courant: " << N << "/" << imSize <<  "\n";
         //std::cout << "Point courant: " << i <<  "\n";
        //  std::cout << "Nb links: " << Node::nb_links << "\n";

            for(int j=0; j<connexity.getNbPoints(); j++)
                {
                Point<TCoord>  q=p+connexity.getPoint(j);
                if(im.isPosValid(q))
                    {
                    int qoff=im.getOffset(q);
                    if (processed[qoff]==true)
                        {
                        // on cherche l'élt canonique de q (représentant du noeud)
                        int adjNode=findNode(qoff,par);
                       // std::cout << "Elt canonique: " << adjNode << "\n";
                        assert(im(adjNode)==im(qoff));
                        // f(q)=f(p): q appartient au noeud de p
                        if(im(curNode)==im(adjNode) && curNode!=adjNode)
                            {
                            //curNode devient élt canonique
                            par[adjNode]=curNode;
                            //les fils de adjNode sont ajoutés aux fils de curNode
                            assert(nodes[curNode]->value==nodes[adjNode]->value);
                            mergeNodes(nodes[curNode],nodes[adjNode]);
                            }

                        // p est un ancêtre de q
                        // mais il faut:
                        // -vérifier quels ancêtres a_i de q deviendront les fils de p
                        // -vérifier si un ancêtre a_i de q n'a pas la même valeur de p, auquel cas p et a_i appartiennent au même noeud
                        // si p et q sont non comparables il n'y a rien à faire: on sait que aucun ancêtre de q ne peut exister tel que f(p)<f(a_i)
                        else if(im(curNode)<im(adjNode))
                            {
                            nodes[curNode]->addChild(nodes[adjNode]);
                            }

                        }
                    }
                }
        processed[i]=true;
        }

    RGB inf;
    inf[0]=0;
    inf[1]=0;
    inf[2]=0;
    int aNode=findNode(0,par);
    vector <Node *> roots;
    //findMinAncestors(nodes[aNode],inf,roots);
    //findMinAncestors2(nodes[aNode],inf,active,isAncestor,roots);
    findMinAncestors3(nodes[aNode],roots);
    // Noeud racine fictif (au cas où il y a plusieurs "racines")
    // au niveau inf
    Node *root=new Node(inf,-1);
    for(int i=0; i<roots.size(); i++)
        {
        root->addChild(roots[i]);
        }

    for(int i=0; i<nodes.size(); i++)
        {
        if(nodes[i]->isPresent) Node::nbNodes++;
        }


    return root;
}


//Image <RGB> CGraph::constructImageCoherenceRecovery2(Node *graph)
//{
//    // component graph contains an image with border
//    Image <RGB> imRes(imSource.getSize());
//    RGB value;
//    value[0]=0;
//    value[1]=0;
//    value[2]=0;
//    imRes.fill(value);

//    Image<bool> imTest(imRes.getSize());
//    imTest.fill(false);

//    int dx=graph.indexOffsets.getSizeX();
//    int dy=graph.indexOffsets.getSizeY();

//    bool *isActive=new bool[graph.root->label+1];

//     std::queue<Node *> fifo;

//    for(int y=1; y<dy-1; y++)
//        for(int x=1; x<dx-1; x++)
//            {
//            Point <TCoord> p(x,y);
//            p.x-=1;
//            p.y-=1;

//            Node *n=graph.indexOffsets(x,y);
//            if(n->active && !n->isVoid)
//                {
//                imRes(p)=n->value;
//                imTest(p)=true;
//                //std::cout << "point: " << p.x << " " << p.y << " label: " << n->label << "\n";
//                }
//            else
//                {
//                for(int i=0; i<graph.root->label+1; i++)
//                    {
//                    isActive[i]=true;
//                    }

//                fifo.push(n);

//                while(!fifo.empty())
//                    {
//                    Node *f=fifo.front();
//                    fifo.pop();

//                    if(f->active && !f->isVoid)
//                        {
//                        RGB v=f->value;
//                        imRes(p)[0]=std::max(imRes(p)[0],v[0]);
//                        imRes(p)[1]=std::max(imRes(p)[1],v[1]);
//                        imRes(p)[2]=std::max(imRes(p)[2],v[2]);
//                        imTest(p)=true;
//                        }
//                    else
//                        {
//                        for(int i=0; i<f->father.size(); i++)
//                            {
//                            // si le père n'a pas encore été visité
//                            if(isActive[f->father[i]->label]==true)
//                                {
//                                fifo.push(f->father[i]);
//                                isActive[f->father[i]->label]=false;
//                                }
//                            }
//                        }
//                    }

//                }
//            }

//    delete[] isActive;

//    for(int i=0; i<imTest.getBufSize(); i++)
//        assert(imTest(i)==true);
//    return imRes;
//}

void Core::areaFiltering(Node *graph, int value, int value2)
{

    std::queue<Node *> fifo;

    if(graph!=0)
        {
        vector <bool> isActive(CGraph::Node::count+1);
        for(int i=0; i<isActive.size(); i++) isActive[i]=true;

        fifo.push(graph);

        while(!fifo.empty())
            {
            Node *n=fifo.front();
            fifo.pop();
            if(n->rep!=-1)
            if(n->area <value || n->area > value2)
                n->active=false;

            for(int i=0; i<n->childs.size(); i++)
                {
                if(isActive[n->childs[i]->label])
                    {
                    isActive[n->childs[i]->label]=false;
                    fifo.push(n->childs[i]);
                    }
                }

            }
        }

}


void Core::contrastFiltering(Node *graph, int value, int value2)
{

    std::queue<Node *> fifo;

    if(graph!=0)
        {
        vector <bool> isActive(CGraph::Node::count+1);
        for(int i=0; i<isActive.size(); i++) isActive[i]=true;

        fifo.push(graph);

        while(!fifo.empty())
            {
            Node *n=fifo.front();
            fifo.pop();
            if(n->rep!=-1)
            if(n->contrast <value || n->contrast>value2)
                n->active=false;

            for(int i=0; i<n->childs.size(); i++)
                {
                if(isActive[n->childs[i]->label])
                    {
                    isActive[n->childs[i]->label]=false;
                    fifo.push(n->childs[i]);
                    }
                }

            }
        }

}

void Core::reconstructMin()
{
    std::queue<Node *> fifo;

        vector <bool> active(CGraph::Node::count+1);
        active.assign(active.size(),true);

        //version bourrin
//        for(int i=0; i<active.size(); i++) {
//            fifo.push(imageNodes[i]);

//            while(!fifo.empty())
//                {
//                Node *n=fifo.front();
//                fifo.pop();

//                for(int i=0; i<n->fathers.size(); i++)
//                    {
//                    if(active[n->fathers[i]->label] && n->fathers[i]->rep!=-1)
//                        {
//                        active[n->fathers[i]->label]=false;
//                        fifo.push(n->fathers[i]);
//                        }
//                    }

//                }
//        }

     for(int i=0; i<nodes.size(); i++) {
        if(nodes[i]->active==false && nodes[i]->fathers.size()>1) {
            fifo.push(nodes[i]);
            active[nodes[i]->label]=false;

            while(!fifo.empty()) {
                Node *n=fifo.front();
                fifo.pop();

                int nactive=0;

                for(int j=0; j<n->fathers.size(); j++) {
                    if(n->fathers[j]->active==true) nactive++;
                }
                //qDebug() << "Node: " << n->label << " "<<  nactive << "\n";
                int j=0;
                while(nactive>1) {
                    if(n->fathers[j]->active==true) {
                        n->fathers[j]->active=false;
                        if(active[n->fathers[j]->label])
                            {
                            fifo.push( n->fathers[j]);
                            active[n->fathers[j]->label]=false;
                            }
                        nactive--;
                    }
                    j++;
                }
           }
        }
    }

    for(int i=0; i<nodes.size(); i++) {
        if(nodes[i]->active==false) {
            Node *n=nodes[i];
            int nactive=0;
            for(int j=0; j<n->fathers.size(); j++) {
                if(n->fathers[j]->active==true) nactive++;
            }
            assert(nactive==0 || nactive==1);
        }
    }

}

void Core::setSyntheticImage()
{
    int a[]={0,0,0};
    int b[]={1,0,0};
    int c[]={0,1,0};
    int d[]={1,1,0};
    int e[]={1,1,1};

    RGB A(a);
    RGB B(b);
    RGB C(c);
    RGB D(d);
    RGB E(e);

    TSize size[]={3,1,1};
    TSpacing spacing[]={1.0,1.0,1.0};
    //const RGB data[]={  A,A,B,
     //                   E,C,D,
      //                  B,A,A};
    const RGB data[]={  A,B,C};
//    TSize size[]={4,4,1};
//    TSpacing spacing[]={1.0,1.0,1.0};
//    const RGB data[]={A,A,B,B,
//                 B,C,B,B,
//                 A,A,E,D,
//                 A,E,E,D};
    Image<RGB> imTmp(size,spacing,data);
    this->imSource=imTmp;
}


//void Core::computeArea()
//{
//    std::queue<Node *> fifo;
//    std::vector<int> active(nodes.size());


//    for(int i=0; i<nodes.size(); i++)
//        {
//        if(this->nodes[i]->isPresent)
//            {
//            for(int i=0; i<active.size(); i++) active[i]=true;

//            fifo.push(this->nodes[i]);
//            active[this->nodes[i]->label]=false;

//            vector<int> tmp;

//            while(!fifo.empty()) {
//                Node *p=fifo.front();
//                fifo.pop();

//                for(int k=0; k<p->childs.size(); k++) {
//                    if(active)
//                }
//            }
//            this->nodes[i]->area+=tmp.size();
//            }
//        }
//}

void Core::computeArea()
{
     std::queue<Node *> fifo;
     std::vector<int> active(imageNodes.size());
    for(int i=0; i<imSource.getBufSize(); i++) {
        int curNode=findNode(i,this->par);

        for(int j=0; j<active.size(); j++) active[j]=true;

        fifo.push(imageNodes[curNode]);
        active[imageNodes[curNode]->label]=false;

        while(!fifo.empty()) {
            Node *p=fifo.front();
            fifo.pop();
            p->area++;

            for(int k=0; k<p->fathers.size(); k++) {
                if(active[p->fathers[k]->label] )   {
                    fifo.push(p->fathers[k]);
                    active[p->fathers[k]->label]=false;
                }
            }
        }
    }
}


//distance par rapport à la feuille la plus éloignée  (équivalent au h-contraste)
//norme L1
void Core::computeContrast(Node *graph)
{
    std::queue<Node *> fifo;
    std::vector<int> active(CGraph::Node::count + 1 );
    active.assign(active.size(), true);
    for(int i=0; i<nodes.size(); i++) {
        if(nodes[i]->childs.size()==0)
            {
            fifo.push(nodes[i]);
            active[nodes[i]->label]=false;
            }
    }

    while(!fifo.empty()) {
        Node *n=fifo.front();
        fifo.pop();
        int curMax=0;
        for(int i=0; i<n->childs.size(); i++)
            {
            Node *c=n->childs[i];
            int d=c->value[0]-n->value[0]+c->value[1]-n->value[1]+c->value[2]-n->value[2];
            if(d>curMax)
                curMax=d;
            }
        n->contrast=curMax;
        for(int k=0; k<n->fathers.size(); k++) {
                if(active[n->fathers[k]->label] )   {
                    fifo.push(n->fathers[k]);
                    active[n->fathers[k]->label]=false;
                }
        }

    }

}

int Core::computeContrastExp(Node *tree)
{
    if(tree!=0)
        {
        int current_level=tree->contrast;
        int current_max=0;
        int current_contrast=0;

        if(tree->contrast==0) {


        for(int i=0; i<tree->childs.size(); i++)
            {
            Node *c=tree->childs[i];
            int current_contrast=c->value[0]-tree->value[0]+c->value[1]-tree->value[1]+c->value[2]-tree->value[2]+computeContrastExp(c);
            if(current_contrast>current_max)
                current_max=current_contrast;
            }
        tree->contrast=current_max;
        }
        return tree->contrast;
        }
    // error
    else return -1;
}

void Core::printNodes()
{
    for(int i=0; i<nodes.size(); i++)
        {

            {
            qDebug() << "Node: " << nodes[i]->label << " - " << "Area: " <<  nodes[i]->area << "Contrast: " << nodes[i]->contrast << " Active: " << nodes[i]->active << " \n";
            }
        }
}

void Core::printGraph()
{


}

Node *Core::graphToNode()
{
    for(int i=0; i<imageNodes.size(); i++)
        if(imageNodes[i]!=0) delete imageNodes[i];
    imageNodes.clear();
    nodes.clear();
    imageNodes.resize(imSource.getBufSize());
    imageNodes.assign(imageNodes.size(),0);

    set<int>::iterator it;

    // création des noeuds
    for(int i=0; i<imSource.getBufSize(); i++) {
        if(par[i]==i) {
            //new node
            Node *n=new Node(imSource(i),i);
            n->rep=i;
            nodes.push_back(n);
            imageNodes[i]=n;
        }
    }

    //création des liens
    for(int i=0; i<imSource.getBufSize(); i++) {
        if(par[i]==i) {
            for(it=graph[i].begin(); it!= graph[i].end(); it++)
                {
                int q=findNode(*it,par);
                imageNodes[i]->fathers.push_back(imageNodes[q]);
                imageNodes[q]->childs.push_back(imageNodes[i]);
                }
        }
    }

    vector<Node *> rootNodes;
    // création d'une racine fictive
    for(int i=0; i<imageNodes.size(); i++) {
        if(imageNodes[i]!=0 && imageNodes[i]->fathers.size()==0)
            rootNodes.push_back(imageNodes[i]);
    }

    //RGB rgbmin=imSource.getMin();
    //RGB rgbmin=std::numeric_limits<RGB>::max();
    //qDebug() << "Image min: " << rgbmin[0] << " " << rgbmin[1] << " "  << rgbmin[2] << "\n";
    Node *root=new Node(0,-1);
    for(int i=0; i<rootNodes.size(); i++) {
        root->childs.push_back(rootNodes[i]);
        rootNodes[i]->fathers.push_back(root);
    }
    nodes.push_back(root);
    return root;
}

void Core::constructImage()
{
    Image<RGB> result=this->imSource;

    for(int i=0; i<imageNodes.size() ; i++)
        {
        if(imageNodes[i]!=0 && imageNodes[i]->active==false)
            {
            Node *n=imageNodes[i];
            vector <Node *> tmp;
            for(int j=0; j<n->fathers.size(); j++)
                if(n->fathers[j]->active && n->fathers[j]->rep != -1)
                    tmp.push_back(n->fathers[j]);
           // assert(tmp.size()==0 || tmp.size() ==1);
            if(tmp.size()>=1)
                par[i]=tmp[0]->rep;
            else if(n->fathers[0]->rep!=-1)// problem to solve here
                par[i]=n->fathers[0]->rep;
            }
        }
    for(int i=0; i<result.getBufSize(); i++)
        {
        RGB value=imSource(findNode(i,this->par));
        result(i)=value;

        }

    result.save("result.ppm");

}


void Core::constructImageExp()
{
    Image<RGB> result=this->imSource;
    std::queue<Node *> fifo;
    vector<bool> active(CGraph::Node::count +1);

    for(int i=0; i<imageNodes.size() ; i++)
        {
        //clear the fifo...
        fifo=std::queue<Node *>();
        if(imageNodes[i]!=0 && imageNodes[i]->active==false)
            {
            active.assign(active.size(),true);
            Node *n=imageNodes[i];
            for(int j=0; j<n->fathers.size(); j++)
                {
                if(active[n->fathers[j]->label])
                    {
                    fifo.push(n->fathers[j]);
                    active[n->fathers[j]->label]=false;
                    }

                }
            bool stop=false;
            while(!fifo.empty() && !stop)
                {
                Node *p=fifo.front();
                fifo.pop();
                if(p->active==true)
                    {
                    //if(p->rep!=-1)
                        par[i]=p->rep;
                    stop=true;
                    }
                else
                    for(int j=0; j<p->fathers.size(); j++)
                        {
                        if(active[p->fathers[j]->label])
                            {
                            fifo.push(p->fathers[j]);
                            active[p->fathers[j]->label]=false;
                            }
                        }
                }
            }
        }
    for(int i=0; i<result.getBufSize(); i++)
        {
        RGB value;
        value[0]=0;
        value[1]=0;
        value[2]=0;
        if(findNodeExp(i,this->par)!=-1)
            value=imSource(findNodeExp(i,this->par));

        result(i)=value;
        assert(result(i)<=imSource(i));
        }


    result.save("result.ppm");

}

void Core::compute(int value, int value2,int value3,int value4)
{
//    CGraph::Node *f1,*f2,*f3,*f4,*f5,*f6,*f7;





//    f1=new Node(A,0);
//    f2=new Node(C,0);
//    f3=new Node(D,0);
//    f4=new Node(E,0);
//    f5=new Node(B,0);
//    f6=new Node(A,0);
//    f7=new Node(D,0);
//
//
//    f6->addChild(f5);
//    f1->addChild(f5);
//    f1->addChild(f2);
//    f1->addChild(f3);
//    f2->addChild(f4);
//    f3->addChild(f4);
//    f1->addChild(f7);
//    f7->addChild(f4);
//
//    vector <Node *> a1=findMinAncestors(f4,A);
//    vector <Node *> a2=findMinAncestors(f5,A);
//
//    printVectorNodes(a1);
//    printVectorNodes(a2);
//
//    writeDot(f1,"test.dot");

//    if(argc<=1)
//        {
//        cout<<"Usage: " << argv[0] << " <imgRGB> \n";
//        exit(1);
//        }

//    TSize size[]={4,4,1};
//    TSpacing spacing[]={1.0,1.0,1.0};
// const RGB data[]={A,A,B,B,
//                   B,C,B,B,
//                   A,A,E,D,
//                   A,E,E,D};



    CGraph::Node::nbLinks=0;
    CGraph::Node::nbNodes=0;
    CGraph::Node::count=0;

    FlatSE connexity;
    connexity.make2DN8();

    CGraph::Core::componentGraphExp(this->imSource, connexity);
    qDebug() << "Compute graph\n";
    Node *root=graphToNode();
    qDebug() << "End compute graph\n";

//    set<int>::iterator it;
//    for(int i=0; i< this->imSource.getBufSize(); i++) {
//        if(this->par[i]==i) {
//            qDebug() << "Pixel: " << i << " Fathers:";
//            for(it=graph[i].begin(); it!=graph[i].end(); it++) qDebug() <<  findNode(*it,par) << " ";
//            qDebug() << "\n";
//        }

//    }

    writeDot("exp.dot");
    writeDot(root,"graph.dot");
    //this->printGraph();
    //CGraph::Node *graph=CGraph::Core::componentGraph(this->imSource, connexity);
    qDebug() << "Compute contrast\n";
    //computeContrastExp(root);
    //printNodes();
    this->writeDot(root,"test.dot");
    qDebug() << "End compute contrast\n";
    computeArea();

   //
   //

//    Image<U8> imRes(imSource.getSize());
//    for(int i=0; i<imRes.getBufSize(); i++)
//        {
//        imRes(i)=findNode(i,par);
//        }
//    imRes.save("res1.pgm");

//    qDebug() << "Area: " << value << "\n";
      qDebug() << "Area filtering\n";
      //contrastFiltering(root,value,value2);
      qDebug() << value3 << " " << value4 << "\n";
      areaFiltering(root,value3,value4);
      //printNodes();
      qDebug() << "End Area filtering\n";
      qDebug() << "Reconstruct min\n";
      //reconstructMin();
      qDebug() << "Fin reconstruct min\n";
      qDebug() << "Construct image\n";
      constructImageExp();
      qDebug() << "Fin construct image min\n";

//    Image<RGB> result=this->imSource;

//    for(int i=0; i<imRes.getBufSize(); i++)
//        {
//        if(nodes[findNode(i,this->par)]->active==false)
//            {
//            if((nodes[findNode(i,this->par)]->fathers.size()!=0) && nodes[findNode(i,this->par)]->fathers[0]->rep!=-1)
//                par[findNode(i,this->par)]=nodes[findNode(i,this->par)]->fathers[0]->rep;
//            }
//    for(int i=0; i<imRes.getBufSize(); i++)
//        {
//        RGB value=imSource(findNode(i,this->par));
//        result(i)=value;
//        imRes(i)=0;
//        }

//        }
//    imRes.save("res2.pgm");
//    result.save("result.ppm");


//    writeDot(graph,"graphFiltered.dot");

//    qDebug() << "Nb noeuds initial: " << Node::count-1 << "\n";
//    qDebug() << "Nb noeuds final: " << Node::nbNodes <<"\n";


}

}

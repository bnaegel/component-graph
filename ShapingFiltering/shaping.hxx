//Copyright (C) 2014, Benoît Naegel <b.naegel@unistra.fr>
//This program is free software: you can use, modify and/or
//redistribute it under the terms of the GNU General Public
//License as published by the Free Software Foundation, either
//version 3 of the License, or (at your option) any later
//version. You should have received a copy of this license along
//this program. If not, see <http://www.gnu.org/licenses/>.

/** Compute the max-tree of the underlying weighted graph */
template<class T>
void Shaping<T>::computeShaping()
{
    totalNodes=0;
    init();
    computeTree();
    delete[] hq;
}

template<class T>
void Shaping<T>::link_node(ShapingNode *tree, ShapingNode *child)
{
    child->father=tree;
    tree->childs.push_back(child);
}

template<class T>
typename Shaping<T>::ShapingNode *Shaping<T>::new_node(int h, int n)
{

    ShapingNode *res=new ShapingNode(totalNodes,h,0);
    totalNodes++;

    return res;
}

// index=index of the node in the graph
template<class T>
void Shaping<T>::update_attributes(ShapingNode *n, int index)
{
   n->nodes.push_back(index);
   n->area++;
}

template<class T>
int Shaping<T>::flood(int h)
{
    int m;

    while(!hq[h].empty())
    {

        int p=hq[h].front();
        typename CGraph<T>::Node *curNode=cgraph.graph[p];
        hq[h].pop();

        STATUS(p)=number_nodes[h];

        if(index[h][STATUS(p)]==0)
        {
            index[h][STATUS(p)]=this->new_node(indexToH(h),STATUS(p));;
        }

        update_attributes(index[h][STATUS(p)],p);

        // visit all neighbors i.e. fathers...

        for(int i=0; i<curNode->fathers.size(); i++) {
            {
                typename CGraph<T>::Node *n=curNode->fathers[i];

                int q=n->index;

                if(STATUS(q)==ACTIVE)
                {

                    hq[hToIndex(n->attr)].push(q);
                    STATUS(q)=NOT_ACTIVE;

                    node_at_level[hToIndex(n->attr)]=true;

                    if(n->attr>curNode->attr)
                    {

                        m=hToIndex(n->attr);

                        do 	{

                            m=this->flood(m);

                        } while(m!=h);
                    }
                }
            }
        }

        // ... and childs
        for(int i=0; i<curNode->childs.size(); i++) {
            typename CGraph<T>::Node *n=curNode->childs[i];

            int q=n->index;

            if(STATUS(q)==ACTIVE)
            {

                hq[hToIndex(n->attr)].push(q);
                STATUS(q)=NOT_ACTIVE;

                node_at_level[hToIndex(n->attr)]=true;

                if(n->attr>curNode->attr)
                {

                    m=hToIndex(n->attr);

                    do 	{

                        m=this->flood(m);

                    } while(m!=h);
                }
            }
        }
    }
    //End of recursion: we have reached a regional maximum
    number_nodes[h]=number_nodes[h]+1;

    m=h-1;
    while(m>=hToIndex(hMin) && node_at_level[m]==false) m--;

    if(m>=hToIndex(hMin) )
    {
        int i=number_nodes[h]-1;
        int j=number_nodes[m];
        if(index[m][j]==0)
        {
            index[m][j]=new_node(indexToH(m),j);
        }

        this->link_node(index[m][j],index[h][i]);
    }
    else
    {
        //The father of root is itself
        index[hToIndex(hMin)][0]->father=index[hToIndex(hMin)][0];
    }
    node_at_level[h]=false;
    return m;
}


template<class T>
void Shaping<T>::computeTree()
{
    //Put the first pixel with value hMin in the queue
    for(int i=0; i<cgraph.graph.size() ; i++) {
        typename CGraph<T>::Node *n=cgraph.graph[i];
        if(n->attr==hMin && STATUS(i)==ACTIVE)
            {
            hq[hToIndex(hMin)].push(i);
            break;
            }
    }

    node_at_level[hToIndex(hMin)]=true;

    this->flood(hToIndex(hMin));

    root=index[hToIndex(hMin)][0];
}


template<class T>
void Shaping<T>::init()
{
    // abstract attribute ->attr should be computed in cgraph
    //
    STATUS.setSize(cgraph.graph.size(),1,1);
    STATUS.fill(ACTIVE);

    int min,max;
    min=std::numeric_limits<int>::max();
    max=std::numeric_limits<int>::min();
    // Compute min and max weight
    for(int i=0; i<cgraph.graph.size() ; i++) {
        typename CGraph<T>::Node *n=cgraph.graph[i];
        if(n->attr > max) max=n->attr;
        if(n->attr < min) min=n->attr;
    }

    this->hMin=min;
    this->hMax=max;
    this->numberOfLevels=hMax-hMin+1;

    std::cout << "hmin : " << hMin << " hmax : " << hMax << " nb of levels : "<<numberOfLevels << "\n";

    index.resize(numberOfLevels);

    hq=new std::queue<int> [numberOfLevels];

    //we take a (max-min+1) * (number of grey-levels at level h)
    // so we compute histogram

    int *histo;
    histo =new int[numberOfLevels];
    for(int i=0; i<numberOfLevels; i++)
        histo[i]=0;


    for(int i=0; i<cgraph.graph.size() ; i++)
        {
        typename CGraph<T>::Node *n=cgraph.graph[i];
        int v=n->attr-hMin;
        histo[v]++;
        }

    for(int i=0; i<numberOfLevels; i++)
        {
        int sizeOfLevelH=histo[i];

        index[i].resize(sizeOfLevelH);

        for(int j=0; j<sizeOfLevelH; j++)
            index[i][j]=0;
        }

    this->number_nodes.resize(numberOfLevels);
    this->node_at_level.resize(numberOfLevels);

    for(int i=0; i<numberOfLevels; i++)
        {
        this->number_nodes[i]=0;
        this->node_at_level[i]=false;
        }

    delete[] histo;
}


template<class T>
int Shaping<T>::computeAreaHelper(ShapingNode *tree)
{
    if(tree!=0)
        {

        for(int i=0; i<tree->childs.size(); i++)
            {
            tree->area+=computeAreaHelper(tree->childs[i]);
            }
        return tree->area;
        }
    // error
    else return -1;
}

template<class T>
int Shaping<T>::computeArea()
{
    root->area=computeAreaHelper(root);
    return 0;
}

template<class T>
int Shaping<T>::computeContrastHelper(ShapingNode *node)
{
    if(node!=0)
        {
        int current_level=node->contrast;
        int current_max=0;
        int current_contrast=0;

        for(int i=0; i< node->childs.size(); i++)
            {
            current_contrast=(node->childs[i]->contrast-current_level)+computeContrastHelper(node->childs[i]);
            if(current_contrast>current_max)
                current_max=current_contrast;
            }
        node->contrast=current_max;
        return node->contrast;
        }
    // error
    else return -1;
}


template<class T>
int Shaping<T>::computeContrast()
{
    root->contrast=computeContrastHelper(root);
    return 0;
}


template<class T>
void Shaping<T>::areaFiltering(int areaMin, int areaMax)
{
    if(root!=0)
    {
        std::queue <ShapingNode *> fifo;
        fifo.push(root);

        while(!fifo.empty() )
        {
            ShapingNode *curNode=fifo.front();
            fifo.pop();
            if(curNode->area < areaMin || curNode->area > areaMax)
            {
                curNode->active=false;
            }

            for(int i=0; i<curNode->childs.size(); i++)
            {
                fifo.push(curNode->childs[i]);
            }
        }
    }
}


template<class T>
void Shaping<T>::areaFiltering(int areaMin)
{
    if(root!=0)
    {
        std::queue <ShapingNode *> fifo;
        fifo.push(root);

        while(!fifo.empty() )
        {
            ShapingNode *curNode=fifo.front();
            fifo.pop();
            if(curNode->area < areaMin )
            {
                curNode->active=false;
            }

            for(int i=0; i<curNode->childs.size(); i++)
            {
                fifo.push(curNode->childs[i]);
            }
        }
    }
}


// height filtering
template<class T>
int Shaping<T>::contrastFiltering(int tMin, int tMax)
{
    if(root!=0)
    {
        std::queue <ShapingNode *> fifo;
        fifo.push(root);

        while(!fifo.empty() )
        {
            ShapingNode *curNode=fifo.front();
            fifo.pop();
            if(curNode->contrast < tMin || curNode->contrast > tMax)
            {
                curNode->active=false;
            }

            for(int i=0; i<curNode->childs.size(); i++)
            {
                fifo.push(curNode->childs[i]);
            }
        }
    }
    return 0;
}


// height filtering
template<class T>
int Shaping<T>::contrastFiltering(int tMin)
{
    if(root!=0)
    {
        std::queue <ShapingNode *> fifo;
        fifo.push(root);

        while(!fifo.empty() )
        {
            ShapingNode *curNode=fifo.front();
            fifo.pop();
            if(curNode->contrast < tMin )
            {
                curNode->active=false;
            }

            for(int i=0; i<curNode->childs.size(); i++)
            {
                fifo.push(curNode->childs[i]);
            }
        }
    }
    return 0;
}

template<class T>
int Shaping<T>::contrastFilteringMax(int tMax)
{
    if(root!=0)
    {
        std::queue <ShapingNode *> fifo;
        fifo.push(root);

        while(!fifo.empty() )
        {
            ShapingNode *curNode=fifo.front();
            fifo.pop();
            if(curNode->contrast > tMax )
            {
                curNode->active=false;
            }

            for(int i=0; i<curNode->childs.size(); i++)
            {
                fifo.push(curNode->childs[i]);
            }
        }
    }
}

template<class T>
int Shaping<T>::elongFiltering(int tMin)
{
    if(root!=0)
    {
        std::queue <ShapingNode *> fifo;
        fifo.push(root);

        while(!fifo.empty() )
        {
            ShapingNode *curNode=fifo.front();
            fifo.pop();
            if(curNode->weight < tMin )
            {
                curNode->active=false;
            }

            for(int i=0; i<curNode->childs.size(); i++)
            {
                fifo.push(curNode->childs[i]);
            }
        }
    }
    return 0;
}


// shaping to graph
// modify cgraph in place (activate set of nodes activated in the shaping)
template<class T>
void Shaping<T>::constructGraph()
{

    // deactivate all nodes
    for(int i=0; i< cgraph.graph.size(); i++) {
        cgraph.graph[i]->active=false;
    }

    // tree breadth first
    if(root!=0)
    {
        std::queue <ShapingNode *> fifo;
        fifo.push(root);

        while(!fifo.empty() )
        {
            ShapingNode *curNode=fifo.front();
            fifo.pop();
            if (curNode->active) {
                for(int i=0; i<curNode->nodes.size(); i++) {
                    if(cgraph.graph[curNode->nodes[i]]->active2==true)
                        cgraph.graph[curNode->nodes[i]]->active=true;
                }
            }

            for(int i=0; i<curNode->childs.size(); i++)
            {
                fifo.push(curNode->childs[i]);
            }
        }
    }
}

template<class T>
int Shaping<T>::writeDot(const char *filename)
{
    if(root!=0)
        {
        std::ofstream file(filename,std::ios_base::trunc  | std::ios_base::binary);
        if(!file)
            {
            std::cerr << "File I/O error\n";
            return 0;
            }

        file << "digraph G {\n";

        std::queue <ShapingNode *> fifo;
        fifo.push(root);
        while(!fifo.empty() )
            {
            ShapingNode *tmp=fifo.front();
            fifo.pop();
            if(tmp->active)
                {
                // write father->son relation if the node is not the root
                if(tmp->father!=tmp)
                {
                file << "\t" << " \"" << tmp->father->index << ",h=" << tmp->father->weight << "," <<  " a= " << tmp->father->area << "," << "c= " << tmp->father->contrast << "\" "
                   << "->" << " \"" << tmp->index << ",h=" << tmp->weight << ","   << " a= " << tmp->area << "," << "c= " << tmp->contrast <<
                    "\" " <<
                    ";\n";
                }
                }
                // push the childs
                for(int i=0; i<tmp->childs.size(); i++)
                    {
                    fifo.push(tmp->childs[i]);
                    }


            }

        file << "}\n";

        file.close();
        return 1;
        }
    else
        return 0;
}

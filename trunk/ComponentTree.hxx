#include "OrderedQueue.h"
#include <iostream>
#include <set>
#include <stack>
#include <map>

namespace CTree {

using std::vector;
using std::map;

template <class T>
ComponentTree<T>::ComponentTree( Image< T > & img )
:m_root(0),m_img(img)
{
	FlatSE connexity;
	connexity.make2DN8();
	SalembierRecursiveImplementation<T> strategy(this,connexity);

	m_root=strategy.computeTree();
	strategy.computeAttributes(m_root);

    std::cout << "Number of nodes: " << strategy.totalNodes << "\n";
}

template <class T>
ComponentTree<T>::ComponentTree( Image< T > & img , FlatSE &connexity)
:m_root(0),m_img(img)
{

	SalembierRecursiveImplementation<T> strategy(this,connexity);

	m_root=strategy.computeTree();
	strategy.computeAttributes(m_root);

	//std::cout << "Root Node: " << m_root << "\n";
}

template <class T>
ComponentTree<T>::~ComponentTree()
{
	//std::cout << "DESTRUCTEUR!\n";
	erase_tree();
}

template <class T>
void ComponentTree<T>::erase_tree()
{
	//std::cout << "Erase tree\n";
	int tot=0;
	if(m_root!=0)
		{
		std::queue <Node *> fifo;
		fifo.push(m_root);
		while(!fifo.empty() )
			{
			Node *curNode=fifo.front();
			fifo.pop();

			for(int i=0; i<curNode->childs.size(); i++)
				{
				fifo.push(curNode->childs[i]);
				}
			delete curNode;
			tot++;
			}
		}
	//std::cout << "Total nodes of tree: " << tot << "\n";
}

template <class T>
Image <T> &ComponentTree<T>::constructImageOptimized()
{
	int numberNonActives=0;
	if(m_root!=0)
		if(m_root->active==true)
		{

		std::queue <Node *> fifo;
		std::queue <Node *> fifoChilds;
		fifo.push(m_root);
		while(!fifo.empty() )
			{
			Node *tmp=fifo.front();
			fifo.pop();

			for(std::vector<Node *>::iterator it=tmp->childs.begin(); it!=tmp->childs.end(); ++it)
				{
				if((*it)->active==false)
					{

					fifoChilds.push(*it);

					while(!fifoChilds.empty())
						{
						Node *child=fifoChilds.front();
						fifoChilds.pop();

						for(std::vector<TOffset >::iterator it3=child->pixels.begin(); it3!=child->pixels.end(); ++it3)
 							m_img(*it3)=(T)tmp->h;

						for(std::vector<Node *>::iterator it2=child->childs.begin(); it2!=child->childs.end(); ++it2)
						{
						fifoChilds.push(*it2);
						}
					}
 					//merge_pixels(*it,subPixels);
// 					for(std::vector<Point<TCoord> >::iterator it=subPixels.begin(); it!=subPixels.end(); ++it)
// 						m_img(*it)=(T)tmp->h;
					}
				else
					fifo.push(*it);
				}

			}
		}
		else
			m_img.fill(T(0));
	else m_img.fill(T(0));

	//std::cout << "Number non active nodes: " << numberNonActives << "\n";
	return m_img;
}

template <class T>
void ComponentTree<T>::constructImageMin(Image<T> &res)
{
	if(m_root->active==true)
		{
		std::queue <Node *> fifo;
		fifo.push(m_root);
		while(!fifo.empty() )
			{
			Node *tmp=fifo.front();
			fifo.pop();

			//update pixels contained in the node
			for(std::vector<TOffset >::iterator it=tmp->pixels.begin(); it!=tmp->pixels.end(); ++it)
				res(*it)=(T)tmp->h;

			for(std::vector<Node *>::iterator it=tmp->childs.begin(); it!=tmp->childs.end(); ++it)
				{
				//if child->active is false, "cut" the subtree and hence search
				//all pixels of all subnodes
				if((*it)->active==false)
					{
					std::vector<TOffset > subPixels=merge_pixels(*it);
					for(std::vector<TOffset >::iterator it=subPixels.begin(); it!=subPixels.end(); ++it)
						res(*it)=(T)tmp->h;
					}
				else
					fifo.push(*it);
				}
			}
		}
	else res.fill(T(0));
}


//Does not work!!!!!
//TODO: implement construct image Max
template <class T>
void ComponentTree<T>::constructImageMax(Image<T> &res)
{
	res.fill(0);

	std::queue<Node *> fifoLeafs;
	std::queue<Node *> fifo;
	fifo.push(m_root);

	while(!fifo.empty())
		{
		Node *currentNode=fifo.front();
		fifo.pop();

		currentNode->status=true;
		if(currentNode->childs.size()!=0)
			{
			for(Node::ContainerChilds::iterator it=currentNode->childs.begin();
				it!=currentNode->childs.end(); ++it)
					fifo.push(*it);
			}
		else
			fifoLeafs.push(currentNode);
		}
	while(!fifoLeafs.empty())
		{
		Node *tmp=fifoLeafs.front();
		fifoLeafs.pop();

		if(tmp->active==false && tmp->father->status==true)
			{
			//we put father in the queue
			fifoLeafs.push(tmp->father);
			tmp->father->status=false;
			}
		else if(tmp->active==true)
				{
			//we construct image node at this level
				std::vector<TOffset > subPixels=merge_pixels(tmp);
				for(std::vector<TOffset >::iterator it=subPixels.begin(); it!=subPixels.end(); ++it)
					res(*it)=(T)tmp->h;

				}
		}
}

template <class T>
void ComponentTree<T>::constructImageDirect(Image<T> &res)
{
	res.fill(T(0));

	std::queue <Node *> fifo;
	fifo.push(m_root);
	while(!fifo.empty() )
		{
		Node *tmp=fifo.front();
		fifo.pop();

		if(tmp->active==true)
			{
			for(std::vector<TOffset >::iterator it=tmp->pixels.begin(); it!=tmp->pixels.end(); ++it)
				res(*it)=(T)tmp->h;

			for(std::vector<Node *>::iterator it=tmp->childs.begin(); it!=tmp->childs.end(); ++it)
				{
				//return all pixels of all consecutive false subnodes
				//stop when an active node is found
				if((*it)->active==false)
					{
					std::vector<TOffset> subPixels=merge_pixelsFalseNodes(*it);
					for(std::vector<TOffset >::iterator it2=subPixels.begin(); it2!=subPixels.end(); ++it2)
						res(*it2)=(T)tmp->h;
					}

				fifo.push(*it);
				}
			}
		else
			{
			for(std::vector<Node *>::iterator it=tmp->childs.begin(); it!=tmp->childs.end(); ++it)
				{
				fifo.push(*it);
				}
			}
		}
}

template <class T>
void ComponentTree<T>::constructImageDirectExpe(Image<T> &res)
{
	res.fill(0);

 	std::queue <Node *> fifo;
 	std::queue <Node *> fifo2;

	fifo.push(m_root);

	while(!fifo.empty() )
		{
		Node *tmp=fifo.front();
		fifo.pop();

		if(tmp->active==true)
			{
			fifo2.push(tmp);
			}

		else
			for(std::vector<Node *>::iterator it=tmp->childs.begin(); it!=tmp->childs.end(); ++it)
				{
				fifo.push(*it);
				}
		}

	while(!fifo2.empty() )
		{
		Node *tmp=fifo2.front();
		fifo2.pop();

		for(std::vector<TOffset >::iterator it=tmp->pixels.begin(); it!=tmp->pixels.end(); ++it)
			res(*it)=(T)tmp->h;

		for(std::vector<Node *>::iterator it=tmp->childs.begin(); it!=tmp->childs.end(); ++it)
			{
			if((*it)->active==false)
				{
				(*it)->h=tmp->h;
				}

			fifo2.push(*it);
			}
		}
}

template <class T>
Image <T> ComponentTree<T>::constructImage(ConstructionDecision decision)
{
	Image <T> res(m_img.getSize());

	if(m_root!=0)
		{
		switch(decision)
			{
			case MIN:
				constructImageMin(res);
				break;
			case MAX:
				constructImageMax(res);
				break;
			case DIRECT:
				constructImageDirectExpe(res);
				break;
			}
		}

		else
			res.fill(T(0));


	return res;
}



template <class T>
void ComponentTree<T>::constructNode(Image <T> &res, Node *node)
{
	std::queue<Node *> fifo;
	fifo.push(node);

	while(!fifo.empty() )
		{
		Node *tmp=fifo.front();
		fifo.pop();

		for(std::vector<TOffset >::iterator it=tmp->pixels.begin(); it!=tmp->pixels.end(); ++it)
			res(*it)=(T)tmp->h;
		for(std::vector<Node *>::iterator it=tmp->childs.begin(); it!=tmp->childs.end(); ++it)
			fifo.push(*it);
		}
}

template <class T>
void ComponentTree<T>::constructNodeDirect(Image <T> &res, Node *node)
{
	std::queue<Node *> fifo;
	fifo.push(node);

	int h=node->h;

	while(!fifo.empty() )
		{
		Node *tmp=fifo.front();
		fifo.pop();

		for(std::vector<TOffset >::iterator it=tmp->pixels.begin(); it!=tmp->pixels.end(); ++it)
			res(*it)=(T)h;
		for(std::vector<Node *>::iterator it=tmp->childs.begin(); it!=tmp->childs.end(); ++it)
			fifo.push(*it);
		}
}


//Experimental 16/07/07
//Construct entire branch of leaf
template <class T>
void ComponentTree<T>::constructBranch(Image <T> &res, Node *leaf)
{
	std::queue<Node *> fifo;
	fifo.push(leaf);


	while(!fifo.empty() )
		{
		Node *tmp=fifo.front();
		fifo.pop();

		for(std::vector<TOffset >::iterator it=tmp->pixels.begin(); it!=tmp->pixels.end(); ++it)
			res(*it)=(T)(tmp->h);

		//Test for root
		if(tmp->father!=tmp)
			{
			fifo.push(tmp->father);
			}
		}
}

template <class T>
void ComponentTree<T>::setFalse()
{
	if(m_root!=0)
		{
		std::queue<Node *> fifo;
		fifo.push(m_root);

		while(!fifo.empty())
			{
			Node *tmp=fifo.front();
			fifo.pop();

			tmp->active=false;

			for(std::vector<Node *>::iterator it=tmp->childs.begin(); it!=tmp->childs.end(); ++it)
				fifo.push(*it);
			}
		}
}

//Experimental 16/07/07
//Active entire branch from leaf
template <class T>
void ComponentTree<T>::activeBranch(Node *leaf)
{
	std::queue<Node *> fifo;
	fifo.push(leaf);

	while(!fifo.empty() )
		{
		Node *tmp=fifo.front();
		fifo.pop();

		tmp->active=true;

		//Test for root
		if(tmp->father!=tmp)
			{
			fifo.push(tmp->father);
			}
		}
}

//Experimental 16/07/07
//Active entire branch from leaf
template <class T>
void ComponentTree<T>::activeBranch(SignatureType &signature)
{

	std::queue<Node *> fifo;

	//Put leaf of signature on fifo
	fifo.push(signature.rbegin()->second);

	while(!fifo.empty() )
		{
		Node *tmp=fifo.front();
		fifo.pop();

		tmp->active=true;

		//Test for root
		if(tmp->father!=tmp)
			{
			fifo.push(tmp->father);
			}
		}
}

template <class T>
void ComponentTree<T>::print()
{
	if(m_root!=0)
		{
		std::queue <Node *> fifo;
		fifo.push(m_root);
		while(!fifo.empty() )
			{
			Node *tmp=fifo.front();
			fifo.pop();
			std::cout << "Node: C(" << tmp->h << "," << tmp->label << ")\n";
			std::cout << "Area= " << tmp->area << "\n";
			std::cout << "Contrast= " << tmp->h << "\n";
			if(tmp->father==0)
				std::cout << "Father= " << 0 << "\n";
			else std::cout << "Father= " << "C(" << tmp->father->h << "," << tmp->father->label <<   ")\n";
			std::cout << "Childs: \n";
			for(int i=0; i<tmp->childs.size(); i++)
				{
				std::cout << "\t\t C(" << tmp->childs[i]->h << "," << tmp->childs[i]->label << ")\n" ;
				fifo.push(tmp->childs[i]);
				}
			std::cout << "\n\n";
			}
		}

}

template <class T>
void ComponentTree<T>::printSize()
{
	int totalSize=0;
	if(m_root!=0)
		{
		std::queue <Node *> fifo;
		fifo.push(m_root);
		totalSize+=sizeof(*m_root);
		while(!fifo.empty() )
			{
			Node *tmp=fifo.front();
			fifo.pop();

			totalSize+=sizeof(*tmp);

			for(int i=0; i<tmp->childs.size(); i++)
				{

				fifo.push(tmp->childs[i]);
				}
			}
		}
	 std::cout << "Total size of tree is " << totalSize/1024 << "kO  (" << totalSize << " bytes)\n";
}

template <class T>
int ComponentTree<T>::writeDot(const char *filename)
{
	if(m_root!=0)
		{
		std::ofstream file(filename,std::ios_base::trunc  | std::ios_base::binary);
		if(!file)
			{
			std::cerr << "File I/O error\n";
			return 0;
			}

		file << "digraph G {\n";

		std::queue <Node *> fifo;
		fifo.push(m_root);
		while(!fifo.empty() )
			{
			Node *tmp=fifo.front();
			fifo.pop();
			if(tmp->active==true)
				{
				// write father->son relation if the node is not the root
				if(tmp->father!=tmp)
				{
				file << "\t" << " \"" << tmp->father->h << "," << tmp->father->label << "\\n " <<
					"a= " << tmp->father->area << "," << "c= " << tmp->father->contrast <<
// 					"\\n v= " << tmp->father->volume <<"," << "l= " << tmp->father->contourLength <<
// 					"\\n cpl= " << tmp->father->complexity << " bbx= " << tmp->father->xmax-tmp->father->xmin <<
// 					"\\n bby= " << tmp->father->ymax-tmp->father->ymin <<
 					"\" "
					"->" << " \"" << tmp->h << "," << tmp->label <<  "\\n " <<
					"a= " << tmp->area << "," << "c= " << tmp->contrast <<
// 					"\\n v= " << tmp->volume <<"," << "l= " << tmp->contourLength <<
// 					"\\n cpl= " << tmp->complexity << " bbx= " << tmp->xmax-tmp->xmin <<
// 					"\\n bby= " << tmp->ymax-tmp->ymin <<
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


//Test whether the se is include in the component (pixels)

template <class T>
bool ComponentTree<T>::isInclude(FlatSE &se, Node::ContainerPixels &pixels)
{
	//Case where the se is larger than the component:
	//obviously se does not fit in
	if(se.getNbPoints() > pixels.size())
		{
		//std::cout << "Size diff\n";
		return false;
		}
	else
	 	{
	 	FlatSE::iterator itSe;
	 	Node::ContainerPixels::iterator itPixels;
	 	Node::ContainerPixels::iterator itPixels2;

	 	for(itPixels=pixels.begin(); itPixels!=pixels.end(); ++itPixels)
	 		{
	 		bool isInclude=true;
	 		for(itSe=se.begin(); itSe!=se.end(); ++itSe)
	 			{
	 			TOffset q=*itSe+*itPixels;
	 			//Test if q belongs to pixels set
	 			for(itPixels2=pixels.begin(); itPixels2!=pixels.end(); ++itPixels2)
	 				if(*itPixels2==q)
	 					break;
	 			if(itPixels2==pixels.end() )
	 				{
	 				isInclude=false;
	 				break;
	 				}
	 			}
	 		if(isInclude==true)
	 			return true;
	 		}

	 	return false;
	 	}
}

//aggregate and return all pixels belonging to subtree
template <class T>
std::vector <TOffset > ComponentTree<T>::merge_pixels(Node *tree)
{

	vector <TOffset > res;

	std::queue <Node *> fifo;
	fifo.push(tree);
	while(!fifo.empty() )
		{
		Node *tmp=fifo.front();
		fifo.pop();
		for(int i=0; i<tmp->pixels.size(); i++)
			res.push_back(tmp->pixels[i]);
		for(int i=0; i<tmp->childs.size(); i++)
			{
			fifo.push(tmp->childs[i]);
			}
		}

	return res;
}

//return all the pixels of the subtree
//stop when it reaches an active node
template <class T>
std::vector <TOffset > ComponentTree<T>::merge_pixelsFalseNodes(Node *tree)
{
	vector <TOffset > res;

	std::queue <Node *> fifo;
	fifo.push(tree);
	while(!fifo.empty() )
		{
		Node *tmp=fifo.front();
		fifo.pop();
		if(tmp->active==false)
			{
			for(int i=0; i<tmp->pixels.size(); i++)
				res.push_back(tmp->pixels[i]);
			for(int i=0; i<tmp->childs.size(); i++)
				{
				fifo.push(tmp->childs[i]);
				}
			}
		}

	return res;
}

//aggregate and return all pixels belonging to subtree
template <class T>
void ComponentTree<T>::merge_pixels(Node *tree, std::vector <TOffset > &res)
{

	std::queue <Node *> fifo;
	fifo.push(tree);
	while(!fifo.empty() )
		{
		Node *tmp=fifo.front();
		fifo.pop();
		for(int i=0; i<tmp->pixels.size(); i++)
			res.push_back(tmp->pixels[i]);
		for(int i=0; i<tmp->childs.size(); i++)
			{
			fifo.push(tmp->childs[i]);
			}
		}
}


//////////////////////////////////////////////////////////////
//
//
//
// Component Tree Filters
//
//
//
//////////////////////////////////////////////////////////////


//reinitialization of tree

template <class T>
int ComponentTree<T>::restore()
{
	if(m_root!=0)
		{
		std::queue<Node *> fifo;
		fifo.push(m_root);

		while(!fifo.empty())
			{
			Node *curNode=fifo.front();
			fifo.pop();
			curNode->active=true;
			curNode->h=curNode->ori_h;
			Node::ContainerChilds::iterator it;
			for(it=curNode->childs.begin(); it!=curNode->childs.end(); ++it)
				fifo.push(*it);
			}
		}
}

//Increasing criteria

template <class T>
int ComponentTree<T>::contrastFiltering(int tMin, int tMax)
{
	if(m_root!=0)
		{
        std::queue <Node *> fifo;
		fifo.push(m_root);

		while(!fifo.empty() )
			{
			Node *curNode=fifo.front();
			fifo.pop();
			if(curNode->contrast < tMin || curNode->contrast > tMax)
				{
				curNode->active=false;
				}

			for(int i=0; i<curNode->childs.size(); i++)
				{
				//curNode->active=true;
				fifo.push(curNode->childs[i]);
				}
			}
		}
}

template <class T>
int ComponentTree<T>::areaFiltering(int tMin, int tMax)
{
	if(m_root!=0)
		{
        std::queue <Node *> fifo;
		fifo.push(m_root);
		while(!fifo.empty() )
			{
			Node *curNode=fifo.front();
			fifo.pop();


			if(curNode->area < tMin  || curNode->area > tMax)
				{
				curNode->active=false;
				}


			for(int i=0; i<curNode->childs.size(); i++)
				{
				//curNode->active=true;
				fifo.push(curNode->childs[i]);
				}
			}
		}
}

template <class T>
int ComponentTree<T>::volumicFiltering(int tMin, int tMax)
{
	if(m_root!=0)
		{
        std::queue <Node *> fifo;
		fifo.push(m_root);
		while(!fifo.empty() )
			{
			Node *curNode=fifo.front();
			fifo.pop();

			if(curNode->volume < tMin || curNode->volume > tMax)
				{
				curNode->active=false;
				}

			for(int i=0; i<curNode->childs.size(); i++)
				{
				//curNode->active=true;
				fifo.push(curNode->childs[i]);
				}
			}
		}
}

//Experimental 16/07/07
//Return N most brighest leafs
template <class T>
vector<Node *> ComponentTree<T>::intensityPruning(int N)
{
	vector<Node *> res;
	map<int, Node *> mapRes;

	if(m_root!=0)
		{
		std::queue<Node *> fifo;
		std::queue<Node *> fifoLeafs;
		fifo.push(m_root);

		while(!fifo.empty())
			{
			Node *currentNode=fifo.front();
			fifo.pop();

			if(currentNode->childs.size()!=0)
				{
				for(Node::ContainerChilds::iterator it=currentNode->childs.begin();
					it!=currentNode->childs.end(); ++it)
						fifo.push(*it);
				}
			else
				mapRes[currentNode->h]=currentNode;
			}

		map<int,Node *>::reverse_iterator it=mapRes.rbegin();
		for(int i=0; i<N && it!=mapRes.rend(); i++)
			{
			Node *tmp=it->second;
			res.push_back(tmp);
			++it;
			}
		}
	return res;
}

//Non-increasing criteria: we must choose a strategy to prune the tree
template <class T>
int ComponentTree<T>::complexityFiltering(int tMin, int tMax)
{
		if(m_root!=0)
			{
			std::queue <Node *> fifo;
			fifo.push(m_root);
			while(!fifo.empty() )
				{
				Node *curNode=fifo.front();
				fifo.pop();

				if(curNode->complexity < tMin || curNode->complexity > tMax)

					{
					curNode->active=false;
					}

				for(int i=0; i<curNode->childs.size(); i++)
					{
					//curNode->active=true;
					fifo.push(curNode->childs[i]);
					}
				}
			}
}

template <class T>
int ComponentTree<T>::compacityFiltering(int tMin, int tMax)
{
		if(m_root!=0)
			{
			std::queue <Node *> fifo;
			fifo.push(m_root);
			while(!fifo.empty() )
				{
				Node *curNode=fifo.front();
				fifo.pop();

				if(curNode->compacity < tMin || curNode->compacity > tMax)

					{
					curNode->active=false;
					}

				for(int i=0; i<curNode->childs.size(); i++)
					{
					//curNode->active=true;
					fifo.push(curNode->childs[i]);
					}
				}
			}
}

template <class T>
int ComponentTree<T>::intensityFiltering(int tMin, int tMax)
{
		if(m_root!=0)
			{
			std::queue <Node *> fifo;
			fifo.push(m_root);
			while(!fifo.empty() )
				{
				Node *curNode=fifo.front();
				fifo.pop();

				if(curNode->h < tMin || curNode->h > tMax)

					{
					curNode->active=false;
					}

				for(int i=0; i<curNode->childs.size(); i++)
					{
					//curNode->active=true;
					fifo.push(curNode->childs[i]);
					}
				}
			}
}

template <class T>
int ComponentTree<T>::boundingBoxFiltering(int min, int max)
{
	if(m_root!=0)
		{
		std::queue <Node *> fifo;
		fifo.push(m_root);

		while(!fifo.empty())
			{
			Node *tmp=fifo.front();
			fifo.pop();


			if(tmp->father!=tmp)

			if( ((tmp->xmax-tmp->xmin) <min && (tmp->ymax-tmp->ymin)<min )
				|| ( (tmp->xmax-tmp->xmin) >max && (tmp->ymax-tmp->ymin)>max) )
				tmp->active=false;

			std::vector<Node *>::iterator it;
			for(it=tmp->childs.begin(); it!=tmp->childs.end(); ++it)
				{
				fifo.push(*it);
				}
			}
		}
}

template <class T>
int ComponentTree<T>::connectedOpening2(FlatSE &se)
{
	se.setContext(this->m_img.getSize());
	if(m_root!=0)
		{
		std::queue<Node *> fifo;

		fifo.push(m_root);

		while(!fifo.empty() )
			{
			Node *curNode=fifo.front();
			fifo.pop();

			std::vector<TOffset> pixels=this->merge_pixels(curNode);
			if(isInclude(se,pixels))
				{
				for(int i=0; i<curNode->childs.size(); i++)
					{
					fifo.push(curNode->childs[i]);
					}
				}
			else
				curNode->active=false;

			}

		return 1;
		}
	else return -1;
}

template <class T>
int ComponentTree<T>::connectedOpening(FlatSE &se)
{
	//we must begin with the leafs and then climb towards the root
	if(m_root!=0)
		{
        std::queue <Node *> fifo;
      	OrderedQueue <Node *> fifo_leafs;
		fifo.push(m_root);
		while(!fifo.empty() )
			{
			Node *curNode=fifo.front();
			fifo.pop();

			curNode->status=true;
			if(curNode->childs.size()==0)
				{
				//we have a leaf!
				fifo_leafs.put(-curNode->h,curNode);
				}
			else
				{
				for(Node::ContainerChilds::iterator it=curNode->childs.begin();
					it!=curNode->childs.end(); ++it)
						fifo.push(*it);
				}
			}
		std::map<Node *, bool> inFifo;

// 		//now we process the leafs
		while(!fifo_leafs.empty() )
			{
			Node *curNode=fifo_leafs.get();

			//std::cout << "We process current node (" <<curNode->h << ","<< curNode->label
			//<< ") PixelSize: " << curNode->pixels.size() <<  " Area: " << curNode->area << "\n";

				std::vector <TOffset > pixelList=merge_pixels(curNode);
				//test if the se fits in the component
				if(isInclude(se,pixelList)==false  )
					{
					//std::cout << "se not include in node (" <<curNode->h << ","<< curNode->label << ")\n";
// 					// we assign the pixels to father's node
  					for(int i=0; i< curNode->pixels.size(); i++)
  						curNode->father->pixels.push_back(curNode->pixels[i]);

					//curNode->childs.clear();
					curNode->active=false;
					//put the father in the queue if it is not the root

					if(curNode->father->status==true)
						{
						//std::cout << "We put in the queue: (" <<curNode->father->h << ","<< curNode->father-> label << ")\n";
						fifo_leafs.put(-curNode->father->h,curNode->father);
						curNode->father->status=false;
						}
					}
				else
					{
					//std::cout << "se IS include in node (" <<curNode->h << ","<< curNode->label << ")\n";

					}



			}
		//std::cout << "File vides \n";
		return 1;
	}
}

template <class T>
Node * ComponentTree<T>::coordToNode(TCoord x, TCoord y)
{
	TOffset offset=m_img.getOffset(x,y);
	return offsetToNode(offset);
}

template <class T>
Node * ComponentTree<T>::offsetToNode(TOffset offset)
{
	Node *res=0;
	std::queue<Node *> fifo;
	fifo.push(m_root);

	while(!fifo.empty() )
		{
		Node *n=fifo.front();
		fifo.pop();

		if(n!=0)
			{
			Node::ContainerPixels::iterator it;
			for(it=n->pixels.begin(); it!=n->pixels.end(); ++it)
				{
				if(*it==offset)
					return n;
				}
			Node::ContainerChilds::iterator jt;
			for(jt=n->childs.begin(); jt!=n->childs.end(); ++jt)
				fifo.push(*jt);
			}
		}
	return res;
}

//Return a list of all ancestors of a node described
template <class T>
std::map<int, Node *> ComponentTree<T>::branchSignature(TCoord x, TCoord y)
{
	SignatureType signature;

	Node *selectedNode=coordToNode(x,y);

	std::queue<Node *> fifo;
	fifo.push(selectedNode);

	while(!fifo.empty() )
		{
		Node *n=fifo.front();
		fifo.pop();
		if(n!=0)
			{
			signature[n->h]=n;
			if(n->father!=n)
				fifo.push(n->father);
			}
		}

	return signature;
}

//Return a list of all ancestors of a node described
template <class T>
std::map<int, Node *> ComponentTree<T>::branchSignature(Node *node)
{
	SignatureType signature;

	Node *selectedNode=node;

	std::queue<Node *> fifo;
	fifo.push(selectedNode);

	while(!fifo.empty() )
		{
		Node *n=fifo.front();
		fifo.pop();
		if(n!=0)
			{
			signature[n->h]=n;
			if(n->father!=n)
				fifo.push(n->father);
			}
		}

	return signature;
}

template <class T>
int ComponentTree<T>::writeSignature(SignatureType &signature, const char *file)
{
	std::ofstream outputFile(file);
	if(!outputFile)
 		{
 		return 0;
 		}
	SignatureType::iterator it;

	//write all attributes of Node
	 for( it=signature.begin(); it!=signature.end(); ++it)
 		outputFile << (int)it->first << " "
 		<< it->second->area << " "
  		<< it->second->contrast << " "
  		<< it->second->xmax-it->second->xmin << " "
  		<< it->second->ymax-it->second->ymin << " "
  		<< it->second->complexity << " "
 		<< it->second->compacity << "\n";
 	outputFile.close();
}

//////////////////////////////////////////////////////////////
//
//
// Attributes computation
//
//
//
//////////////////////////////////////////////////////////////

template <class T>
int SalembierRecursiveImplementation<T>::computeContrast(Node *tree)
{
	if(tree!=0)
		{
		int current_level=tree->h;
		int current_max=0;
		int current_contrast=0;
		Node::ContainerChilds::iterator it;
		for(it=tree->childs.begin(); it!=tree->childs.end(); ++it)
			{
			current_contrast=((*it)->h-current_level)+computeContrast(*it);
			if(current_contrast>current_max)
				current_max=current_contrast;
			}
		tree->contrast=current_max;
		return tree->contrast;
		}
	// error
	else return -1;
}

template <class T>
int SalembierRecursiveImplementation<T>::computeArea(Node *tree)
{
	if(tree!=0)
		{
		Node::ContainerChilds::iterator it;
		for(it=tree->childs.begin(); it!=tree->childs.end(); ++it)
			{
			tree->area+=computeArea(*it);
			}
		return tree->area;
		}
	// error
	else return -1;
}

template <class T>
int SalembierRecursiveImplementation<T>::computeSubNodes(Node *tree)
{
	if(tree!=0)
		{
		Node::ContainerChilds::iterator it;
		for(it=tree->childs.begin(); it!=tree->childs.end(); ++it)
			{
			tree->subNodes=tree->childs.size()+computeSubNodes(*it);
			}
		return tree->subNodes;
		}
	// error
	else return -1;
}

template <class T>
long double SalembierRecursiveImplementation<T>::computeM01(Node *tree)
{
	if(tree!=0)
		{
		Node::ContainerChilds::iterator it;
		for(it=tree->childs.begin(); it!=tree->childs.end(); ++it)
			{
			tree->m01+=computeM01(*it);
			}
		return tree->m01;
		}
	// error
	else return -1;
}

template <class T>
long double SalembierRecursiveImplementation<T>::computeM10(Node *tree)
{
	if(tree!=0)
		{
		Node::ContainerChilds::iterator it;
		for(it=tree->childs.begin(); it!=tree->childs.end(); ++it)
			{
			tree->m10+=computeM10(*it);
			}
		return tree->m10;
		}
	// error
	else return -1;
}

template <class T>
long double SalembierRecursiveImplementation<T>::computeM20(Node *tree)
{
	if(tree!=0)
		{
		Node::ContainerChilds::iterator it;
		for(it=tree->childs.begin(); it!=tree->childs.end(); ++it)
			{
			tree->m20+=computeM20(*it);
			}
		return tree->m20;
		}
	// error
	else return -1;
}

template <class T>
long double SalembierRecursiveImplementation<T>::computeM02(Node *tree)
{
	if(tree!=0)
		{
		Node::ContainerChilds::iterator it;
		for(it=tree->childs.begin(); it!=tree->childs.end(); ++it)
			{
			tree->m02+=computeM02(*it);
			}
		return tree->m02;
		}
	// error
	else return -1;
}


template <class T>
void SalembierRecursiveImplementation<T>::computeInertiaMoment(Node *tree)
{
	std::queue<Node *> fifo;

	fifo.push(tree);

	while(!fifo.empty())
		{
		Node *tmp=fifo.front();
		fifo.pop();

		long double xmoy=tmp->m10/tmp->area;
		long double ymoy=tmp->m01/tmp->area;
		long double eta20=(tmp->m20-xmoy*tmp->m10)/(tmp->area*tmp->area);
		long double eta02=(tmp->m02-ymoy*tmp->m01)/(tmp->area*tmp->area);
		tmp->I=100*(eta20+eta02);

		std::vector<Node *>::iterator it;
		for(it=tmp->childs.begin(); it!=tmp->childs.end(); ++it)
			fifo.push(*it);
		}
}

/** @brief Compute Volume for each node
  * There is no clear definition of what is volume on discrete image.
  * I choose the following: V(node)=node.area*(node.h-node.father.h)+ sum (V(n.fils))
  * A special case for the root: the volume is equal to the sum of all grey levels
  * This way the notion is more "intuitive"
**/

//Warning: this function depends on the attribute "area" of each node
//So the "area" attribute must be computed before any call to this function!!!!
template <class T>
int SalembierRecursiveImplementation<T>::computeVolume(Node *tree)
{
	if(tree!=0)
		{
		int local_contrast=0;
		//special case for root node
		if(tree->father==tree)
			{
			local_contrast=tree->h;
			}
		else local_contrast=tree->h-tree->father->h;

		tree->volume=tree->area* local_contrast;

		Node::ContainerChilds::iterator it;
		for(it=tree->childs.begin(); it!=tree->childs.end(); ++it)
			{
			tree->volume+= computeVolume(*it);
			}
		return (tree->volume);
		}
	// error
	else return -1;
}


/** @brief Compute compacity
  *
**/

//Here we use shape-based attribute

template <class T>
int SalembierRecursiveImplementation<T>::computeContourLength()
{
	// we compute the contour length with STATUS image and m_img
	typename Image<T>::iterator it;
	typename Image<T>::iterator end=imBorder.end();


	this->se.setContext(imBorder.getSize());

	FlatSE::iterator itSE;
	FlatSE::iterator seEnd=se.end();;

	TOffset offset=0;
	for(it=imBorder.begin(); it!=end; ++it,offset++)
		{
		bool contour=false;
		bool hitsBorder=false;
		T minValue=std::numeric_limits<T>::max();
		//std::cout << "Offset= " << offset << " Status= " << STATUS(offset) << "\n";
		if(STATUS(offset)!=BORDER_STATUS)
			{
			for(itSE=se.begin(); itSE!=seEnd; ++itSE)
				{
				TOffset q=offset+*itSE;
				if(STATUS(q)!=BORDER_STATUS)
					{
					if(*it>imBorder(q))
						{
						contour=true;
						if(imBorder(q)<minValue)
							minValue=imBorder(q);
						}
					}
				else if(STATUS(q)==BORDER_STATUS)
					{
					//a neighbor of border is a contour point
					//we must propagate this until level Min
					contour=true;
					hitsBorder=true;
					minValue=this->hMin;

					}
				}
			}
		if(contour==true)
			//for all the nodes of levels *it->minValue+1, update contour length
			{
// 			std::cout << "Un pt de contour, offset= " << offset <<
// 			" MinValue= " << (int)minValue << "\n" ;
			Node *tmp=index[hToIndex(*it)][STATUS(offset)];
			assert(tmp!=0);
			if(hitsBorder==false)
				while(tmp->h > minValue)
					{
					//std::cout << "tmp->h= " << tmp->h << "\n";
					tmp->contourLength++;
					tmp=tmp->father;
					}
			else
				{
				bool stop=false;
				while(!stop)
					{
					tmp->contourLength++;
					if(tmp!=tmp->father)
						tmp=tmp->father;
					else stop=true;
					}
				}
			}
		}

}

template <class T>
int SalembierRecursiveImplementation<T>::computeComplexityAndCompacity(Node *tree)
{
	computeContourLength();

	if(tree!=0)
		{
		std::queue<Node *> fifo;
		fifo.push(tree);

		while(!fifo.empty() )
			{
			Node *n=fifo.front();
			fifo.pop();

			if(n->area!=0)
				n->complexity=(int)(100.0*n->contourLength/n->area);
			if(n->contourLength!=0)
				{
				n->compacity=(int)(((double)(4*M_PI*n->area)/((double)n->contourLength*n->contourLength))*100);
				//WARNING!!!!!!!!!!!!!!
// 				if(n->compacity>100)
// 					n->compacity=100;
				}
			else n->compacity=0;
			std::vector<Node *>::iterator it;
			for(it=n->childs.begin(); it!=n->childs.end(); ++it)
				fifo.push(*it);
			}
		return 1;
		}
	// error
	else return -1;
}



template <class T>
int SalembierRecursiveImplementation<T>::computeBoundingBox(Node *tree)
{
	int xmin,xmax,ymin,ymax;
	std::queue<Node *> fifo;
	std::stack<Node *> stackNodes;

	fifo.push(tree);

	while(!fifo.empty())
		{
		Node *tmp=fifo.front();
		fifo.pop();

		stackNodes.push(tmp);

		std::vector<Node *>::iterator it;
		for(it=tmp->childs.begin(); it!=tmp->childs.end(); ++it)
			fifo.push(*it);
		}

	while(!stackNodes.empty())
		{
		Node *tmp=stackNodes.top();
		stackNodes.pop();

		if(tmp->father!=tmp)
			{
			tmp->father->xmin=std::min(tmp->father->xmin, tmp->xmin);
			tmp->father->xmax=std::max(tmp->father->xmax, tmp->xmax);
			tmp->father->ymin=std::min(tmp->father->ymin, tmp->ymin);
			tmp->father->ymax=std::max(tmp->father->ymax, tmp->ymax);
			}
		}
}

template <class T>
int SalembierRecursiveImplementation<T>::computeAttributes(Node *tree)
{
	if(tree!=0)
		{
		tree->area=computeArea(tree);
		tree->contrast=computeContrast(tree);
		tree->volume=computeVolume(tree);

		computeComplexityAndCompacity(tree);
 		computeBoundingBox(tree);
		tree->subNodes=computeSubNodes(tree);
		tree->m01=computeM01(tree);
		tree->m10=computeM10(tree);
		tree->m20=computeM20(tree);
		tree->m02=computeM02(tree);
		computeInertiaMoment(tree);
		}
}

//////////////////////////////////////////////////////////////

template <class T>
inline int SalembierRecursiveImplementation<T>::update_attributes(Node *n, TOffset &imBorderOffset)
{
	//conversion offset imBorder->im
	//ugly
	Point <TCoord> imCoord=imBorder.getCoord(imBorderOffset);
	imCoord.x-=back[0];
	imCoord.y-=back[1];
	imCoord.z-=back[2];
	//coord.print();
	TOffset imOffset=imCoord.x+imCoord.y*oriSize[0]+imCoord.z*oriSize[0]*oriSize[1];

	n->pixels.push_back(imOffset);
	n->area++;

	n->m10+=imCoord.x;
	n->m01+=imCoord.y;
	n->m20+=imCoord.x*imCoord.x;
	n->m02+=imCoord.y*imCoord.y;

	if(imCoord.x < n->xmin) n->xmin=imCoord.x;
	if(imCoord.x > n->xmax) n->xmax=imCoord.x;

	if(imCoord.y < n->ymin) n->ymin=imCoord.y;
	if(imCoord.y > n->ymax) n->ymax=imCoord.y;

}

template <class T>
inline int SalembierRecursiveImplementation<T>::flood(int h)
{
	//assert(h>=0 && h <numberOfLevels);
	//std::cout << "h= " << (int)h << "\n";
	int m;

	while(!hq[h].empty())
		{

		TOffset p=hq[h].front();
		hq[h].pop();
		//std::cout << "Point trait�: " << p << " " << (int)imBorder(p) << "\n";

		//ne peut-on pas indexer un noeud selon un label unique pour tout l'arbre?
		STATUS(p)=number_nodes[h];

		if(index[h][STATUS(p)]==0)
			{
			index[h][STATUS(p)]=this->new_node(indexToH(h),STATUS(p));;
			//std::cout << "Cr�ation du noeud C("<< h << "," << STATUS[p] << ")\n";
			}

		update_attributes(index[h][STATUS(p)],p);

		FlatSE::iterator it;
		FlatSE::iterator end=se.end();

		for(it=se.begin(); it!=end; ++it)
			{
			TOffset q=p+*it;

			if(STATUS(q)==ACTIVE)
				{

				hq[hToIndex(imBorder(q))].push(q);
				STATUS(q)=NOT_ACTIVE;

				node_at_level[hToIndex(imBorder(q))]=true;

				if(imBorder(q)>imBorder(p))
					{

					m=hToIndex(imBorder(q));

					do 	{

						m=this->flood(m);
						//std::cout << "m= " << m << "h= " << h << "\n";

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
			//std::cout << "Redescente: Cr�ation du noeud C(" << m <<"," << j << ")\n";
			index[m][j]=new_node(indexToH(m),j);
			}
// 		std:: cout << "Pere (C(" << index[hToIndex(h)][i]->h << "," << i << ")=" << "C(" << index[hToIndex(m)][j]->h << "," << j << ")\n";

 		this->link_node(index[m][j],index[h][i]);
		}
	else
	 	{
	 	//The father of root is itself
	 	index[hToIndex(hMin)][0]->father=index[hToIndex(hMin)][0];
	 	}
	//assert(h>=hMin && h < node_at_level.size());
	node_at_level[h]=false;
	return m;
}


template <class T>
Node * SalembierRecursiveImplementation<T>::computeTree()
{
	//Put the first pixel with value hMin in the queue
	typename Image<T>::iterator it;
	typename Image<T>::iterator end=imBorder.end();
	TOffset offset=0;
	for(it=imBorder.begin(); it!=end; ++it,offset++)
			if(*it==hMin && STATUS(offset)==ACTIVE)
				{
				hq[hToIndex(hMin)].push(offset);
				break;
				}

	node_at_level[hToIndex(hMin)]=true;

	//std::cout << "Initialisation termin�e Hmin= " << (int)hMin << "\n";

	this->flood(hToIndex(hMin));

	//std::cout << "Calcul termin� Hmin= " << (int)hMin << "\n";

	Node *root=index[hToIndex(hMin)][0];

	//WARNING: this is for now evil, for research purposes only!
// 	std::cout << back[0] << " " << front[0] << "\n";
// 	std::cout << back[1] << " " << front[1] << "\n";
// 	std::cout << back[2] << " " << front[2] << "\n";

	this->m_parent->STATUS=this->STATUS.crop(back[0],this->STATUS.getSizeX()-front[0],
	back[1],this->STATUS.getSizeY()-front[1],
	back[2],this->STATUS.getSizeZ()-front[2]);

	this->m_parent->index=this->index;

	return root;
}

//initialize global index for nodes
template <class T>
int SalembierRecursiveImplementation<T>::init(Image <T> &img, FlatSE &connexity)
{

// 	FlatSE se;
// 	se.make2DN8();
//
	FlatSE se=connexity;

	const TSize *tmpSize=img.getSize();
	const TCoord *tmpBack=se.getNegativeOffsets();
	const TCoord *tmpFront=se.getPositiveOffsets();

	for(int i=0; i<=2; i++)
		{
		oriSize[i]=tmpSize[i];
		back[i]=tmpBack[i];
		front[i]=tmpFront[i];
		}

	imBorder=img;
	STATUS.setSize(img.getSize());
	STATUS.fill(ACTIVE);

	addBorders(imBorder,back,front,BORDER);
	addBorders(STATUS,back,front,BORDER_STATUS);
	se.setContext(imBorder.getSize());


	this->hMin=img.getMin();
	this->hMax=img.getMax();
	this->numberOfLevels=hMax-hMin+1;

	index.resize(numberOfLevels);

	hq=new std::queue<TOffset> [numberOfLevels];

	//we take a (max-min+1) * (number of grey-levels at level h)
	// so we compute histogram

	int *histo;
	histo =new int[numberOfLevels];
	for(int i=0; i<numberOfLevels; i++)
		histo[i]=0;

	typename Image<T>::iterator it;
	typename Image<T>::iterator end=img.end();

	//std:: cout << "Number of levels: " << (int)numberOfLevels << "\n";

	for(it=img.begin(); it!=end; ++it)
		{
		int n=*it-hMin;
		//std::cout << "n= " << n << "\n";
		assert(n>=0 && n<numberOfLevels);
		histo[n]++;
		}

	for(int i=0; i<numberOfLevels; i++)
		{
		int sizeOfLevelH=histo[i];

		//index[i]=new Node * [sizeLevelH ];
		index[i].resize(sizeOfLevelH);

		for(int j=0; j<sizeOfLevelH; j++)
			index[i][j]=0;
		}


	this->se=se;

	this->number_nodes.resize(numberOfLevels);
	this->node_at_level.resize(numberOfLevels);

	for(int i=0; i<numberOfLevels; i++)
		{
		this->number_nodes[i]=0;
		this->node_at_level[i]=false;
		}

    totalNodes=0;

	delete[] histo;
}

template <class T>
void SalembierRecursiveImplementation<T>::link_node(Node *tree, Node *child)
{
	child->father=tree;
	tree->childs.push_back(child);
}

template <class T>
Node *SalembierRecursiveImplementation<T>::new_node(int h, int n)
{
	Node *res=new Node;
	res->ori_h=h;
	res->h=h;
	res->label=n;
    totalNodes++;
	return res;
}


///Aggregate all subpixels from the node tree
// vector <long int> merge_pixels(tNode *tree)
// {
//
// 	vector <long int> res;
//
// 	std::queue <struct Node *> fifo;
// 	fifo.push(tree);
// 	while(!fifo.empty() )
// 		{
// 		tNode *tmp=fifo.front();
// 		fifo.pop();
// 		for(int i=0; i<tmp->pixels.size(); i++)
// 			res.push_back(tmp->pixels[i]);
// 		for(int i=0; i<tmp->childs.size(); i++)
// 			{
// 			fifo.push(tmp->childs[i]);
// 			}
// 		}
//
// 	return res;
// }
//
//
//
// void filterArea(tNode *root, int area)
// {
// 	if(root!=NULL)
// 		{
//             std::queue <struct Node *> fifo;
// 		fifo.push(root);
// 		while(!fifo.empty() )
// 			{
// 			tNode *curNode=fifo.front();
// 			fifo.pop();
//
// 			if(curNode->area <= area)
// 				//we "cut" the subtree
// 				{
// 				vector <long int> subPixels=merge_pixels(curNode);
//
// 				for(int i=0; i< subPixels.size(); i++)
// 					curNode->father->pixels.push_back(subPixels[i]);
//
// 				assert(curNode->father!=0);
//
//
// 				curNode->childs.clear();
// 				curNode->active=false;
//
// 				}
//
// 			else
// 			for(int i=0; i<curNode->childs.size(); i++)
// 				{
// 				fifo.push(curNode->childs[i]);
// 				}
// 			}
// 		}
// }
//


// void make_father(tNode ***index, int label1, int label2, int h1, int h2)
// {
// 	tNode *node=index[h1][label1];
// 	if(node==NULL) {
// 		std::cout << "Big Error!\n";
// 		exit(-1);
// 		}
// 	//std::cout << "Node: " << h1 << " " <<label1<< "\n";
// 	//std::cout << node->h << " " << node->label << "\n";
// 	tNode *res=new struct Node;
// 	res->label=label2;
// 	res->h=h2;
// 	res->father=node;
//
// 	//std::cout << "New node: " << h2 << " " <<label2<< "\n";
// 	//std::cout << res->h << " " << res->label << "\n";
//
// 	node->label=label1;
// 	node->h=h1;
// 	node->childs.push_back(res);
//
// 	index[h2][label2]=res;
// 	//std::cout << "Insertion: "<< h2 << " " << label2  << "\n";
// }

// tNode *init_tree(void)
// {
// 	tNode *res=new struct Node;
// 	//std::cout << "Size of struct: " << sizeof(struct Node) << "\n";
// 	//std::cout << "Size of struct: " << sizeof(res->h) << " " << sizeof(res->label)
// 	//<< " " << sizeof(res->father) << " " << sizeof(res->childs) << " " << sizeof(res->pixels) <<"\n";
// 	res->label=-1;
// 	res->h=-1;
// 	res->area=0;
// 	res->father=NULL;
//
//
// 	return res;
// }



///Reconstruct image from tree

///Build the component tree of image im
///Return a structure containing the image max-tree
///For now: trivial algorithm

// template <class T>
// tNode *computeComponentTree(Image <T> &im, FlatSE &se)
// {
// 	T hMax=im.getMax();
// 	T hMin=im.getMin();
//
// 	Image <T> tmp(im.getSize());
// 	Image <T> tmp2(im.getSize());
//
// 	Image <TLabel> Ch(im.getSize());
// 	Image <TLabel> TCh(im.getSize());
//
// 	int nbLevels=hMax+20;
// 	int maxCC=5000;
//
// 	tNode *tree=init_tree();
// 	tNode ***index=new struct Node**[nbLevels];
// 	for(int i=0; i<nbLevels; i++)
// 		index[i]=new struct Node *[maxCC];
// 	//std::cout << "Hmin= " << (int)hMin << "\n";
// 	index[hMin][1]=tree;
// 	//std::cout << "index: " << index[hMin][1]->h << " " << index[hMin][1]->label << "\n";
// 	//std::cout << "Tree: " << tree->h << " " << tree->label << "\n"; ;
// 	typedef std::vector <std::pair <int, TLabel> > tChildsList;
// 	typedef std::set <std::pair<TLabel,TLabel> > tEdgeSet;
// 	tEdgeSet edgeSet;
//
// 	std::vector<long int> pixels;
//
// 	for(int h=hMin; h<=hMax; h++)
// 		{
// 		std::cout << "h= " << h << "\n";
// 		tmp=im;
// 		tmp2=im;
// 		threshold(tmp,h,hMax);
// 		threshold(tmp2,h+1,hMax);
//
// 		Ch=labelConnectedComponents(tmp, se);
// 		//std::cout << "Niveau: " << h << " nb CC: " << Ch.getMax() << "\n";
// 		TCh=labelConnectedComponents(tmp2, se);
// 		//std::cout << "Niveau: " << h+1 << " nb CC: " << TCh.getMax() << "\n";
// 		for(int i=0; i<im.getBufSize(); i++)
// 			{
// 			if(Ch(i)!=0)
// 				{
// 				index[h][Ch(i)]->area++;
// 				}
// 			if(Ch(i)!=0 && im(i)>h)	//construct father/son relationship
// 				{
// 				edgeSet.insert(std::make_pair(Ch(i),TCh(i) ));
// 				}
// 			if(Ch(i)!=0 && im(i)==h)
// 				{
// 				index[h][Ch(i)]->pixels.push_back(i);
//
// 				assert(h==im(i));
// 				}
// 			}
//
// 		for(tEdgeSet::iterator it=edgeSet.begin(); it!=edgeSet.end(); ++it)
// 			{
// 			//std::cout << "Lien entre: C(" << h <<"," <<it->first<< ") et C("<< h+1 << ","<<it->second << ")\n";
// 			make_father(index, it->first, it->second, h, h+1);
// 			}
// 		edgeSet.clear();
// 		}
// 	std::cout << "Fin de la boucle\n";
// 	return tree;
// }


/*
template <class T>
tNode *computeComponentTreeBensMethod(Image <T> &im, FlatSE &se)
{
	T hMax=im.getMax();
	T hMin=im.getMin();
	int bufSize=im.getBufSize();
	se.setContext(im.getSize() );

	std::map<int, std::queue <long int> > pixels;

	std::vector<bool> status(bufSize);
	for(int i=0; i<status.size(); i++) status[i]=0;

	int nbLevels=hMax+20;
	int maxCC=5000;

	tNode *tree;
	tNode ***index=new struct Node**[nbLevels];
	for(int i=0; i<nbLevels; i++)
		index[i]=new struct Node *[maxCC];

	Image <bool> isProcessed(im.getSize());

	for(int i=0; i<bufSize; i++)
		{
		pixels[im(i)].push(i);
		}

	for(int h=hMax; h>=hMin; h--)
		{
		int nCC=1;
		while(!pixels[h].empty())
			{
			long int p=pixels[h].front();
			pixels[h].pop();

			if(status[p]==0)
				{
				tNode *Chn=init_tree(h,nCC);
				index[h][nCC]=Chn;
				Chn->pixels.push_back(p);

				status[p]=nCC;

				isProcessed.fill(false);

				std::queue <long int> fifo;
				fifo.push(p);
				isProcessed(p)=true;
				while(!fifo.empty() )
					{
					long int q=fifo.front();
					fifo.pop();
					for (int i=0; i<se.getNbPoints(); i++)
						{
						long int r=q+se.getOffset(i);

						if(im.isPosValid(r))
						 if(isProcessed(r)==false)
							{
							isProcessed(r)=true;
							if(im(r)>im(q))
								{
								fifo.push(r);
								std:: cout << "Pere (C(" << h << "," << nCC << ")=" << "C(" << (int)im(r) << "," << status[r] << ")\n";
								father(Chn, index[im(r)][status[r]]);
								}
							if(im(r)==im(q))
								{
								status[r]=nCC;
								Chn->pixels.push_back(r);
								fifo.push(r);
								}
							}
						}

					}
				nCC++;
				}


			}
		}
	tree=index[hMin][1];
	return tree;
}*/
/*
int computeArea(tNode *tree)
{
	if(tree!=0)
		{
		if(tree->childs.size()!=0)
			{
			for(int i=0; i<tree->childs.size(); i++)
				{
				 tree->area+=computeArea(tree->childs[i]);
				}

			}
		return tree->area;
		}
}

template <class T>
int flood(Image <T> &im, std::map <int, std::queue<int> > &oq, int h,  int hMin,
vector <int> &STATUS, vector <int> &number_nodes, vector <bool> &node_at_level, FlatSE &se, std::map <T,
std::map<TLabel, struct Node *> > &index )
{
	//std::cout << "h= " << h << "\n";
	int m;
	while(!oq[h].empty())
		{
		int p=oq[h].front();
		oq[h].pop();
		//std::cout << "Point trait�: " << p << " " << im(p) << "\n";
		assert(number_nodes[h]>=0);

		STATUS[p]=number_nodes[h];

		if(index[h].count(STATUS[p])==0)
			{
			tNode *Chn=init_tree(h,STATUS[p]);
			index[h][STATUS[p]]=Chn;
			//std::cout << "Cr�ation du noeud C("<< h << "," << STATUS[p] << ")\n";
			}
		index[h][STATUS[p]]->pixels.push_back(p);
		index[h][STATUS[p]]->area++;

		//struct NghbList *nghblist=im.getNghbList(p);

		for(int i=0; i<se.getNbPoints() ; i++)
			{
			int q=p+se.getOffset(i);

			if(im.isPosValid(q))
				{

				//std::cout << "Voisin: " << q << " " << im(q) << "\n";
				if(STATUS[q]==-2)
					{
					oq[im(q)].push(q);
					STATUS[q]=-1;
					node_at_level[im(p)]=true;
					node_at_level[im(q)]=true;

					if(im(q)>im(p))
						{

						m=im(q);

						do 	{
							m=flood(im,oq,m,hMin,STATUS,number_nodes,node_at_level,se,index);
							//std::cout << "m= " << m << "h= " << h << "\n";
							if(m<h) exit(-1);
							} while(m!=h);
						}
					}
				}
			}
		}
	//End of recursion: we have reached a regional maximum
	number_nodes[h]=number_nodes[h]+1;


 	m=h-1;
	while(m>=hMin && node_at_level[m]==false) m--;

	if(m>=hMin)
		{
		int i=number_nodes[h]-1;
		int j=number_nodes[m];
		if(index[m][j]==0)
			{
			//std::cout << "Redescente: Cr�ation du noeud C(" << m <<"," << j << ")\n";
			index[m][j]=init_tree(m,j);
			}
		//std:: cout << "Pere (C(" << h << "," << i << ")=" << "C(" << m << "," << j << ")\n";

 		father(index[m][j],index[h][i]);
		}

	//assert(h>=hMin && h < node_at_level.size());
	node_at_level[h]=false;
	return m;
}


///New method to deal with neighbors
template <class T>
int flood2(Image <T> &im, std::map <int, std::queue<int> > &oq, int h,  int hMin,
vector <int> &STATUS, vector <int> &number_nodes, vector <bool> &node_at_level, FlatSE &se, std::map <T,
std::map<TLabel, struct Node *> > &index )
{
	//std::cout << "h= " << h << "\n";
	int m;
	while(!oq[h].empty())
		{
		int p=oq[h].front();
		oq[h].pop();
		//std::cout << "Point trait�: " << p << " " << im(p) << "\n";
		assert(number_nodes[h]>=0);

		STATUS[p]=number_nodes[h];

		if(index[h].count(STATUS[p])==0)
			{
			tNode *Chn=init_tree(h,STATUS[p]);
			index[h][STATUS[p]]=Chn;
			//std::cout << "Cr�ation du noeud C("<< h << "," << STATUS[p] << ")\n";
			}
		index[h][STATUS[p]]->pixels.push_back(p);
		index[h][STATUS[p]]->area++;

		struct NghbList *nghblist=im.getNghbList(p);

		//std::cout << "Size nghblist: " << nghblist->size << " Offset list: \n";
		//for(int i=0; i<nghblist->size; i++) std::cout << nghblist->offset[i] << " " ;
		//std::cout << "\n";

		for(int i=0; i<se.getNbPoints() ; i++)
			{
			int q=p+se.getOffset(i);

			if(im.isPosValid(q))
				{

				//std::cout << "Voisin: " << q << " " << im(q) << "\n";
				if(STATUS[q]==-2)
					{
					oq[im(q)].push(q);
					STATUS[q]=-1;
					node_at_level[im(p)]=true;
					node_at_level[im(q)]=true;

					if(im(q)>im(p))
						{

						m=im(q);

						do 	{
							m=flood2(im,oq,m,hMin,STATUS,number_nodes,node_at_level,se,index);
							//std::cout << "m= " << m << "h= " << h << "\n";
							if(m<h) exit(-1);
							} while(m!=h);
						}
					}
				}
			}
		}
	number_nodes[h]=number_nodes[h]+1;


 	m=h-1;
	while(m>=hMin && node_at_level[m]==false) m--;

	if(m>=hMin)
		{
		int i=number_nodes[h]-1;
		int j=number_nodes[m];
		if(index[m][j]==0)
			{
			//std::cout << "Redescente: Cr�ation du noeud C(" << m <<"," << j << ")\n";
			index[m][j]=init_tree(m,j);
			}
		//std:: cout << "Pere (C(" << h << "," << i << ")=" << "C(" << m << "," << j << ")\n";

 		father(index[m][j],index[h][i]);
		}

	//assert(h>=hMin && h < node_at_level.size());
	node_at_level[h]=false;
	return m;
}

///Following Salembier recursive implementation...
template <class T>
tNode *computeComponentTree2V1(Image <T> &im, FlatSE &se)
{
	T hMax=im.getMax();
	T hMin=im.getMin();
	int bufSize=im.getBufSize();
	se.setContext(im.getSize() );

	tNode *tree;

	OrderedQueue <long int> fp;

	std::map <int, std::queue<int> > oq;

	std::map< T , std::map <TLabel, tNode *> > index;

	std::vector<int> STATUS(bufSize);
	for(int i=0; i<STATUS.size(); i++) STATUS[i]=-2;

	std::vector<int> number_nodes(hMax+1);
	std::vector<bool> node_at_level(hMax+1);
	for(int i=0; i<number_nodes.size(); i++)
		{
		number_nodes[i]=0;
		node_at_level[i]=false;
		}

	for(int i=0; i<im.getBufSize(); i++)
			if(im(i)==hMin)
				{
				oq[hMin].push(i);
				break;
				}

	//node_at_level[hMin]=true;
	flood(im,oq,hMin, hMin, STATUS, number_nodes, node_at_level, se,index);
	tree=index[hMin][0];

	computeArea(tree);

	for(int i=0; i<bufSize; i++)
		if(STATUS[i]<0)
			std::cout << "Point non trait�: "<< i << "\n";
	for(int i=0; i<=hMax; i++)	if(oq[i].size() !=0) std::cout <<" File non vide \n";

	return tree;
}


///Following Salembier recursive implementation...
template <class T>
tNode *computeComponentTree2(Image <T> &im, FlatSE &se)
{
	T hMax=im.getMax();
	T hMin=im.getMin();
	int bufSize=im.getBufSize();
	se.setContext(im.getSize() );

	tNode *tree;

	OrderedQueue <long int> fp;

	std::map <int, std::queue<int> > oq;

	std::map< T , std::map <TLabel, tNode *> > index;

	std::vector<int> STATUS(bufSize);
	for(int i=0; i<STATUS.size(); i++) STATUS[i]=-2;

	std::vector<int> number_nodes(hMax+1);
	std::vector<bool> node_at_level(hMax+1);
	for(int i=0; i<number_nodes.size(); i++)
		{
		number_nodes[i]=0;
		node_at_level[i]=false;
		}

	for(int i=0; i<im.getBufSize(); i++)
			if(im(i)==hMin)
				{
				oq[hMin].push(i);
				break;
				}

	//node_at_level[hMin]=true;
	flood2(im,oq,hMin, hMin, STATUS, number_nodes, node_at_level, se,index);
	tree=index[hMin][0];

	computeArea(tree);

	for(int i=0; i<bufSize; i++)
		if(STATUS[i]<0)
			std::cout << "Point non trait�: "<< i << "\n";
	for(int i=0; i<=hMax; i++)	if(oq[i].size() !=0) std::cout <<" File non vide \n";

	return tree;
}*/


}

#ifndef CGRAPH_H
#define CGRAPH_H

#include <vector>
#include <map>

#include "Image.h"
#include "FlatSE.h"

using namespace LibTIM;
using namespace std;

class RAGraph {

public :
    struct Vertex {
        int index;

        RGB color;


        std::vector <int> compNb;
        std::vector <int> nonCompNb;

        std::vector <int> allNb;  /*!< All neighbors */
        std::vector <Point <TCoord> > pixels;

        Vertex(int n) {
            this->index=n;
        }

        Vertex() {

        }

        void insertCompNb(int index) {

            for(int i=0; i<compNb.size(); i++) {
                if(this->compNb[i]==index) return;
            }
            //std::cout << "Sth inserted\n";
            compNb.push_back(index);
            allNb.push_back(index);
        }

        void insertNonCompNb(int index) {
            for(int i=0; i<nonCompNb.size(); i++) {
                if(nonCompNb[i]==index) return;
            }
            //std::cout << "Non comp Sth  inserted\n";
            nonCompNb.push_back(index);
            allNb.push_back(index);
        }
    };

     template <class T> void addBorders(Image<T> &im,
                                      const TCoord *preWidth,
                                      const TCoord *postWidth,
                                      T value)
    {
        TSize newSize[3];
        const TSize *oriSize = im.getSize();

        for (int i = 0; i < 3; i++)
        {
            newSize[i] = oriSize[i] + preWidth[i] + postWidth[i];
        }

        Image<T> temp(newSize);

        typename Image<T>::iterator it;
        typename Image<T>::iterator end=temp.end();

        std::fill(temp.begin(), end,value);
        temp.copy(im, preWidth[0],preWidth[1],preWidth[2]);

        im= temp;
    }

    Image <RGB> imSource;
    FlatSE connexity;
    Image <TLabel> imFlatZones;

    void computeRAGraph();
    void computeFlatZones();

public :
    std::vector <Vertex *> nodes;

    RAGraph(Image <RGB> imSource, FlatSE connexity) {
        this->imSource=imSource;
        this->connexity=connexity;
        computeRAGraph();
    }
    ~RAGraph() {
        for(int i=1; i<nodes.size(); i++) delete nodes[i];
    }

    void writeDot(const char *fileName);
    void print();
};


class CGraphWatcher {

    protected:
        int curProgress;
        int finalProgress;
    public:
        virtual void progressUpdate(int ncur, int nfinal)=0;
        virtual void progressUpdate()=0;
    CGraphWatcher(int finalProgress) {
        this->curProgress=0;
        this->finalProgress=finalProgress;
    }
};


class CColorOrdering {
    public:
         inline virtual bool islessequal(RGB &v, RGB &w)=0;
         inline virtual bool isequal(RGB &v, RGB &w)=0;
         inline virtual int getPriority(RGB &v)=0;
};

class CColorMarginalOrdering : public CColorOrdering {
    public:
          inline bool islessequal(RGB &v, RGB &w) {
            if(v[0]<=w[0] && v[1] <= w[1] && v[2]<=w[2])
                return true;
            else return false;
         }

         inline bool isequal(RGB &v, RGB &w) {
            if(v[0] == w[0] && v[1]==w[1] && v[2]==w[2]) return true;
            else return false;
         }

         inline int getPriority(RGB &v) {
            return -(v[0]+v[1]+v[2]);
         }
};

class CColorMarginalInverseOrdering : public CColorOrdering {
    public:
          bool islessequal(RGB &v, RGB &w) {
            if(v[0]>=w[0] && v[1] >= w[1] && v[2]>=w[2])
                return true;
            else return false;
         }

         bool isequal(RGB &v, RGB &w) {
            if(v[0] == w[0] && v[1]==w[1] && v[2]==w[2]) return true;
         else return false;
         }

          int getPriority(RGB &v) {
            return (v[0]+v[1]+v[2]);
         }
};


class CColorSatLumOrdering : public CColorOrdering {
     double getS(RGB &v) {
            U8 max=std::max(std::max(v[0],v[1]),v[2]);
            U8 min=std::min(std::min(v[0],v[1]),v[2]);
        if(max!=0)
            return (255.0*(max-min)/max);
        else return 0;
     }
     double getV(RGB &v) {
        return std::max(std::max(v[0],v[1]),v[2]);
     }

    public:

          bool islessequal(RGB &v, RGB &w) {

            if(getS(v)<=getS(w))
                return true;
            else return false;
         }

         bool isequal(RGB &v, RGB &w) {
            //if(v[0] == w[0] && v[1]==w[1] && v[2]==w[2]) return true;
            if(getS(v)==getV(v)) return true;
         else return false;
         }

          int getPriority(RGB &v) {
            return -(getS(v)+getV(v));
         }
};

class CGraph
{
    RAGraph *rag;

    Image<RGB> imSource;

public:
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

    CGraph(Image <RGB> &imSource, FlatSE &connexity) {
        this->imSource=imSource;
        //std::cout << "Compute RAG\n";
        this->rag=new RAGraph(imSource,connexity);
        //std::cout << "RAG computed\n";
        //this->rag->print();
        }
    CGraph(RAGraph *rag) : rag(rag) {}
    ~CGraph() { delete rag;}

    Node *componentGraphNaive(FlatSE &connexity);
    int computeGraph(CColorOrdering *order, CGraphWatcher *watcher);

    static Image<RGB> syntheticImage();
    static Image<RGB> syntheticImage2();

    std::vector<Node *> graph;
    int writeDot(const char *filename);

    //void computeArea();
    void areaFiltering(int areaMin);
    void reconstructMin();
    Image<RGB> constructImage(CColorOrdering *order);


    int writeDot(Node *root, const char *filename);
    static bool notComparable(Image<RGB> &im, Point<TCoord> &p, Point<TCoord> &q);

    bool isEqual(Node *n, Node *m);
    bool isIncluded(Node *n, Node *m);
    bool isIncludedFast(Node *n, Node *m, vector<bool> &tmp);
    void computeLinksFast(vector<Node *> &nodes);
    void computeLinks(vector<Node *> &nodes);
    Node *addRoot(vector<Node *> &nodes);
    vector<Node *> minimalElements(vector<Node *> &nodes, vector<bool> &tmp);
    void computeTransitiveReduction(vector<Node *> &nodes);

    int computeGraphInverse(CGraphWatcher *watcher);
    Image<RGB> constructImageInf();
    void areaFiltering(int areaMin, int areaMax);
    bool isLTE(RGB &v, RGB &w);
    Image<RGB> constructImageInverse();

    void contrastFiltering(int contrastMin);
    void contrastFiltering(int contrastMin, int contrastMax);
    void resetFiltering();

    void adaptiveAreaFiltering(int p);
    void adaptiveContrastFiltering(int p);
private:
    void paintNode(Image<RGB> &im, Node *n, RGB &value);
    void paintNodeSup(Image<RGB> &imRes, Node *n, RGB &value);

    vector<Node *> computeComponents(Image<RGB> &im, FlatSE &connexity);

    Node *root;
};

#endif // CGRAPH_H

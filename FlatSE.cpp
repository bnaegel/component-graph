#include "FlatSE.h"


namespace LibTIM {



///Structuring element from unsigned char image
///Point<TCoord> s greater than 0 are put into the SE
///WARNING: the center of the image is set to be the origin

    FlatSE::FlatSE(const Image<unsigned char> &im) {
        int dx = im.getSizeX();
        int dy = im.getSizeY();
        int dz = im.getSizeZ();

        int oImx = dx / 2;
        int oImy = dy / 2;
        int oImz = dz / 2;

        for (int z = 0; z < dz; z++)
            for (int y = 0; y < dy; y++)
                for (int x = 0; x < dx; x++) {
                    if (im(x, y, z) > 0) {
                        Point<TCoord> p(x - oImx, y - oImy, z - oImz);
                        addPoint(p);
                    }
                }
    }

    FlatSE &FlatSE::operator=(const FlatSE &src) {
        if (this != &src) {
            this->points = src.points;
            this->offsets = src.offsets;
            for (int i = 0; i < 3; i++) {
                this->negativeOffsets[i] = src.negativeOffsets[i];
                this->positiveOffsets[i] = src.positiveOffsets[i];
            }
        }
        return *this;
    }


    int FlatSE::getNbPoints() const {
        return points.size();
    }

    void FlatSE::setContext(const TSize *imSize) {
        offsets.clear();
        offsets.reserve(points.size());

        std::vector<Point<TCoord> >::iterator it;
        std::vector<Point<TCoord> >::iterator end = points.end();
        for (it = points.begin(); it != end; ++it) {
            int offset = it->x + (it->y) * imSize[0] + (it->z) * imSize[0] * imSize[1];
            offsets.push_back(offset);
        }
    }

    void FlatSE::setNegPosOffsets() {
        for (int i = 0; i < 3; i++) {
            negativeOffsets[i] = 0;
            positiveOffsets[i] = 0;
        }
        for (int i = 0; i < getNbPoints(); i++) {
            Point<TCoord> c = getPoint(i);
            negativeOffsets[0] = std::max(negativeOffsets[0], -(c.x));
            negativeOffsets[1] = std::max(negativeOffsets[1], -(c.y));
            negativeOffsets[2] = std::max(negativeOffsets[2], -(c.z));

            positiveOffsets[0] = std::max(positiveOffsets[0], (c.x));
            positiveOffsets[1] = std::max(positiveOffsets[1], (c.y));
            positiveOffsets[2] = std::max(positiveOffsets[2], (c.z));
        }
    }


// -------

    const TCoord *FlatSE::getNegativeOffsets() const {
        return negativeOffsets;
    }

// ---
    const TCoord *FlatSE::getPositiveOffsets() const {
        return positiveOffsets;
    }

/*! Convert structuring element into GImage. The center of image represents the origin.
*/

    Image<U8> FlatSE::toImage() {
        TSize imageSize[3];

        const TCoord *pOffsets = getPositiveOffsets();
        const TCoord *nOffsets = getNegativeOffsets();


        imageSize[0] = 1 + 2 * std::max(pOffsets[0], nOffsets[0]);
        imageSize[1] = 1 + 2 * std::max(pOffsets[1], nOffsets[1]);
        imageSize[2] = 1 + 2 * std::max(pOffsets[2], nOffsets[2]);

        Image<U8> result(imageSize);
        result.fill(0);

        //cout << imageSize[0] << " " << imageSize[1] << " " << imageSize[2] << "\n";

        //Coordinates of the origin in image
        Point<TCoord> oImage(imageSize[0] / 2, imageSize[1] / 2, imageSize[2] / 2);

        for (int i = 0; i < getNbPoints(); i++) {
            Point<TCoord> c = getPoint(i) + oImage;
            //c.print();
            result(c) = 255;
        }
        return result;
    }


    void FlatSE::makeSymmetric() {
        FlatSE result;

        for (std::vector<Point<TCoord> >::iterator it = this->points.begin(); it != this->points.end(); ++it) {
            Point<TCoord> p = *it;
            p.x = -p.x;
            p.y = -p.y;
            p.z = -p.z;
            result.points.push_back(p);
            result.setNegPosOffsets();
        }


        *this = result;
    }

/*! Basic neighborhood (4-neighborhood). Warning: do not contain the origin!
*/
    void FlatSE::make2DN4() {
        points.clear();
        offsets.clear();

        Point<TCoord> N(0, -1);
        Point<TCoord> S(0, 1);
        Point<TCoord> W(-1, 0);
        Point<TCoord> E(1, 0);

        points.push_back(N);
        points.push_back(S);
        points.push_back(W);
        points.push_back(E);

        this->setNegPosOffsets();
    }

/*! Basic neighborhood (4-neighborhood). Elementary cross
*/

    void FlatSE::make2DN5() {
        points.clear();
        offsets.clear();

        Point<TCoord> o(0, 0);
        Point<TCoord> N(0, -1);
        Point<TCoord> S(0, 1);
        Point<TCoord> W(-1, 0);
        Point<TCoord> E(1, 0);

        points.push_back(o);
        points.push_back(N);
        points.push_back(S);
        points.push_back(W);
        points.push_back(E);

        this->setNegPosOffsets();
    }

/*! Basic neighborhood (8-neighborhood). Warning: do not contain the origin!
*/
    void FlatSE::make2DN8() {
        points.clear();
        offsets.clear();

        Point<TCoord> N(0, -1);
        Point<TCoord> S(0, 1);
        Point<TCoord> W(-1, 0);
        Point<TCoord> E(1, 0);

        Point<TCoord> NW(-1, -1);
        Point<TCoord> NE(1, -1);
        Point<TCoord> SW(-1, 1);
        Point<TCoord> SE(1, 1);

        points.push_back(N);
        points.push_back(S);
        points.push_back(W);
        points.push_back(E);

        points.push_back(NW);
        points.push_back(NE);
        points.push_back(SW);
        points.push_back(SE);

        this->setNegPosOffsets();
    }

///same as before but includes origin

    void FlatSE::make2DN9() {
        points.clear();
        offsets.clear();

        Point<TCoord> N(0, -1);
        Point<TCoord> S(0, 1);
        Point<TCoord> W(-1, 0);
        Point<TCoord> E(1, 0);

        Point<TCoord> NW(-1, -1);
        Point<TCoord> NE(1, -1);
        Point<TCoord> SW(-1, 1);
        Point<TCoord> SE(1, 1);

        Point<TCoord> O(0, 0);

        points.push_back(N);
        points.push_back(S);
        points.push_back(W);
        points.push_back(E);

        points.push_back(NW);
        points.push_back(NE);
        points.push_back(SW);
        points.push_back(SE);
        points.push_back(O);

        this->setNegPosOffsets();
    }

    void FlatSE::make2DEuclidianBall(int r) {
        points.clear();
        offsets.clear();

        double rr = r * r;

        for (int x = -r; x <= r; x++)
            for (int y = -r; y <= r; y++) {
                if (x * x + y * y <= rr) {
                    Point<TCoord> p;
                    p.x = x;
                    p.y = y;
                    p.z = 0;
                    points.push_back(p);
                }
            }

        this->setNegPosOffsets();
    }

    template<class VoxelType>
    void FlatSE::makeBallEuclidian2D(Image<VoxelType> &img, double r) {
        points.clear();
        offsets.clear();

        double vx = img.getSpacingX();
        double vy = img.getSpacingY();

        int ux = (int) (r / vx);
        int uy = (int) (r / vy);

        double rr = r * r;

        for (int x = -ux; x <= ux; x++)
            for (int y = -uy; y <= uy; y++) {
                if (x * x * vx * vx + y * y * vy * vy <= rr) {
                    Point<TCoord> p;
                    p.x = x;
                    p.y = y;
                    p.z = 0;
                    points.push_back(p);
                }
            }
        this->setNegPosOffsets();
    }

    template<class VoxelType>
    void FlatSE::makeBallChessboard2D(Image<VoxelType> &img, double rx, double ry) {
        points.clear();
        offsets.clear();

        double vx = img.getVx();
        double vy = img.getVy();

        int ux = (int) (rx / vx);
        int uy = (int) (ry / vy);

        for (int x = -ux; x <= ux; x++)
            for (int y = -uy; y <= uy; y++) {
                Point<TCoord> p;
                p.x = x;
                p.y = y;
                p.z = 0;
                points.push_back(p);
            }
        this->setNegPosOffsets();
    }

    void FlatSE::make3DN26() {
        points.clear();
        offsets.clear();

        Point<TCoord> N(0, -1);
        Point<TCoord> S(0, 1);
        Point<TCoord> W(-1, 0);
        Point<TCoord> E(1, 0);

        Point<TCoord> NW(-1, -1);
        Point<TCoord> NE(1, -1);
        Point<TCoord> SW(-1, 1);
        Point<TCoord> SE(1, 1);

        //Origin is not included here
        //Point<TCoord>  O(0,0);

        Point<TCoord> UN(0, -1, 1);
        Point<TCoord> US(0, 1, 1);
        Point<TCoord> UW(-1, 0, 1);
        Point<TCoord> UE(1, 0, 1);

        Point<TCoord> UNW(-1, -1, 1);
        Point<TCoord> UNE(1, -1, 1);
        Point<TCoord> USW(-1, 1, 1);
        Point<TCoord> USE(1, 1, 1);
        Point<TCoord> UO(0, 0, 1);

        Point<TCoord> DN(0, -1, -1);
        Point<TCoord> DS(0, 1, -1);
        Point<TCoord> DW(-1, 0, -1);
        Point<TCoord> DE(1, 0, -1);

        Point<TCoord> DNW(-1, -1, -1);
        Point<TCoord> DNE(1, -1, -1);
        Point<TCoord> DSW(-1, 1, -1);
        Point<TCoord> DSE(1, 1, -1);
        Point<TCoord> DO(0, 0, -1);

        points.push_back(N);
        points.push_back(S);
        points.push_back(W);
        points.push_back(E);

        points.push_back(NW);
        points.push_back(NE);
        points.push_back(SW);
        points.push_back(SE);

        points.push_back(UN);
        points.push_back(US);
        points.push_back(UW);
        points.push_back(UE);

        points.push_back(UNW);
        points.push_back(UNE);
        points.push_back(USW);
        points.push_back(USE);
        points.push_back(UO);

        points.push_back(DN);
        points.push_back(DS);
        points.push_back(DW);
        points.push_back(DE);

        points.push_back(DNW);
        points.push_back(DNE);
        points.push_back(DSW);
        points.push_back(DSE);
        points.push_back(DO);

        this->setNegPosOffsets();
    }

    void FlatSE::make3DN27() {
        points.clear();
        offsets.clear();

        Point<TCoord> N(0, -1);
        Point<TCoord> S(0, 1);
        Point<TCoord> W(-1, 0);
        Point<TCoord> E(1, 0);

        Point<TCoord> NW(-1, -1);
        Point<TCoord> NE(1, -1);
        Point<TCoord> SW(-1, 1);
        Point<TCoord> SE(1, 1);

        //Origin is included here
        Point<TCoord> O(0, 0, 0);

        Point<TCoord> UN(0, -1, 1);
        Point<TCoord> US(0, 1, 1);
        Point<TCoord> UW(-1, 0, 1);
        Point<TCoord> UE(1, 0, 1);

        Point<TCoord> UNW(-1, -1, 1);
        Point<TCoord> UNE(1, -1, 1);
        Point<TCoord> USW(-1, 1, 1);
        Point<TCoord> USE(1, 1, 1);
        Point<TCoord> UO(0, 0, 1);

        Point<TCoord> DN(0, -1, -1);
        Point<TCoord> DS(0, 1, -1);
        Point<TCoord> DW(-1, 0, -1);
        Point<TCoord> DE(1, 0, -1);

        Point<TCoord> DNW(-1, -1, -1);
        Point<TCoord> DNE(1, -1, -1);
        Point<TCoord> DSW(-1, 1, -1);
        Point<TCoord> DSE(1, 1, -1);
        Point<TCoord> DO(0, 0, -1);

        points.push_back(N);
        points.push_back(S);
        points.push_back(W);
        points.push_back(E);

        points.push_back(NW);
        points.push_back(NE);
        points.push_back(SW);
        points.push_back(SE);

        points.push_back(UN);
        points.push_back(US);
        points.push_back(UW);
        points.push_back(UE);

        points.push_back(UNW);
        points.push_back(UNE);
        points.push_back(USW);
        points.push_back(USE);
        points.push_back(UO);

        points.push_back(DN);
        points.push_back(DS);
        points.push_back(DW);
        points.push_back(DE);

        points.push_back(DNW);
        points.push_back(DNE);
        points.push_back(DSW);
        points.push_back(DSE);
        points.push_back(DO);

        this->setNegPosOffsets();
    }

    void FlatSE::make3DAxialSegment(int l) {
        points.clear();
        offsets.clear();

        for (int i = -l; i <= l; i++)
            points.push_back(Point<TCoord>(0, 0, i));
        this->setNegPosOffsets();
    }

    template<class VoxelType>
    void FlatSE::makeBallEuclidian3D(Image<VoxelType> &img, double r) {
        points.clear();
        offsets.clear();

        double vx = img.getSpacingX();
        double vy = img.getSpacingY();
        double vz = img.getSpacingZ();

        int ux = (int) (r / vx);
        int uy = (int) (r / vy);
        int uz = (int) (r / vz);

        int uxux = ux * ux;
        int uyuy = uy * uy;
        int uzuz = uz * uz;
        double rr = r * r;

        std::cout << "Vx= " << vx << " " << " Vy= " << vy << " Vz= " << vz << "\n";
        std::cout << "ux= " << ux << " " << " uy= " << uy << " uz= " << uz << "\n";

        for (int x = -ux; x <= ux; x++)
            for (int y = -uy; y <= uy; y++)
                for (int z = -uz; z <= uz; z++) {
                    if (x * x * vx * vx + y * y * vy * vy + z * z * vz * vz <= rr) {
                        Point<TCoord> p;
                        p.x = x;
                        p.y = y;
                        p.z = z;
                        points.push_back(p);
                    }
                }
        this->setNegPosOffsets();
    }

///circle with specified thickness
    template<class T>
    void FlatSE::makeCircle2D(Image<T> &img, double r, double t) {
        points.clear();
        offsets.clear();

        double vx = img.getSpacingX();
        double vy = img.getSpacingY();

        int ux = (int) (r / vx);
        int uy = (int) (r / vy);

        double rr = r * r;
        double rrtt = (r + t) * (r + t);

        for (int x = -ux; x <= ux; x++)
            for (int y = -uy; y <= uy; y++)
                if (x * x * vx * vx + y * y * vy * vy <= rrtt && x * x * vx * vx + y * y * vy * vy >= rr) {
                    Point<TCoord> p;
                    p.x = x;
                    p.y = y;
                    p.z = 0;
                    points.push_back(p);
                }
        this->setNegPosOffsets();
    }

}// end namespace

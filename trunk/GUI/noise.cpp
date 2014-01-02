#include "noise.h"


Noise::Noise()
{
}

Image<RGB> Noise::addGaussianNoise(Image <RGB> &imSource, double mean, double sigma)
{
    Image <RGB> imRes(imSource.getSize());

    int dx=imSource.getSizeX();
    int dy=imSource.getSizeY();

    std::random_device rd;
    std::mt19937 gen(rd());

    std::normal_distribution<> d(mean,sigma);

    for(int y=0; y<dy; y++) {
        for(int x=0; x<dx; x++) {
            for(int c=0; c<3; c++) {
                int value;
                double noise=d(gen);
                value=(int)(imSource(x,y)[c]+ noise);
                if(value<0) value=0;
                if(value>255) value=255;
                imRes(x,y)[c]=value;
            }
        }
    }

    return imRes;
}


//Image<S16> Noise::addGaussianNoise(Image <U8> &imSource, double mean, double sigma)
//{
//    Image <U16> imRes(imSource.getSize());

//    int dx=imSource.getSizeX();
//    int dy=imSource.getSizeY();

//    std::random_device rd;
//    std::mt19937 gen(rd());

//    std::normal_distribution<> d(mean,sigma);

//    for(int y=0; y<dy; y++) {
//        for(int x=0; x<dx; x++) {
//            for(int c=0; c<3; c++) {
//                int value;
//                double noise=d(gen);
//                value=(S16)(imSource(x,y)[c]+ noise);

//                imRes(x,y)[c]=value;
//            }
//        }
//    }

//    return imRes;
//}


Image<RGB> Noise::addImpulseNoise(Image <RGB> &imSource, int rate)
{
    Image <RGB> imRes(imSource.getSize());

    int dx=imSource.getSizeX();
    int dy=imSource.getSizeY();

//    std::random_device rd;
//    std::mt19937 gen(rd());

    srand(time(NULL));

    for(int y=0; y<dy; y++) {
        for(int x=0; x<dx; x++) {
            for(int c=0; c<3; c++) {
                int value;
                int noise=rand()%100;
                if(noise<rate) {
                    if(noise%2==0) value=255;
                    else value=0;
                    imRes(x,y)[c]=value;
                }
                else imRes(x,y)[c]=imSource(x,y)[c];
            }
        }
    }

    return imRes;
}

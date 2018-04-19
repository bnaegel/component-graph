#ifndef NOISE_H
#define NOISE_H

#include <random>

#include "Image.h"

using namespace LibTIM;

class Noise
{
public:
    Noise();

    static Image <RGB> addGaussianNoise(Image <RGB> &im, double mean, double sigma);
    static Image <S16> addGaussianNoise(Image <U8> &im, double mean, double sigma);
    static Image<RGB> addImpulseNoise(Image<RGB> &imSource, int rate);
};

#endif // NOISE_H

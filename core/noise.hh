#ifndef NOISE__HH
#define NOISE__HH

#include "../CImg.h"
#include "global_core.hh"

void gaussianImageNoise(CImage& img, double sigma);
void saltAndPepperNoise(CImage& img, double r);

#endif // NOISE_HH

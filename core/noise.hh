#ifndef NOISE__HH
#define NOISE__HH

#include "../CImg.h"
#include "global_core.hh"

//void gaussianImageNoise(CImage& img, double sigma);
//void saltAndPepperNoise(CImage& img, double r);

#include "boost/random.hpp"
#include "boost/generator_iterator.hpp"
#include <set>

inline void gaussianImageNoise(CImage& img, double sigma)
{
//    boost::mt19937 rng;

//    boost::normal_distribution<> nd(0, sigma);

//    boost::variate_generator< boost::mt19937, boost::normal_distribution<> >
//                                dice(rng, nd);

//    for(int i = 0; i < img.width(); i++)
//    {
//        for(int h = 0; h < img.height(); h++)
//        {
//            float& point = img(i, h);
//            point += dice();
//            if(point > 1.f)
//                point = 1.f;
//            if(point < 0.f)
//                point = 0.f;
//        }
//    }
    img.get_noise(sigma, 0);
}

inline void saltAndPepperNoise(CImage& img, double r)
{
//    int points = img.width() * img.height() * r;
//    boost::mt19937 rng;
//    boost::uniform_int<> width(0, img.width()-1);
//    boost::uniform_int<> height(0, img.height()-1);

//    boost::variate_generator< boost::mt19937, boost::uniform_int<> >
//                        diceW(rng, width);
//    boost::variate_generator< boost::mt19937, boost::uniform_int<> >
//                        diceH(rng, height);

//    std::set< std::pair<int, int> > paintedPoints;
//    for(int i = 0; i < points; )
//    {
//        int x = diceW();
//        int y = diceH();

//        if(paintedPoints.count(std::make_pair(x, y)))
//            continue;

//        img(x, y) = (rand() % 2) ? 1.f : 0.f;
//        i++;
//    }
    img.get_noise(r, 2);
}

#endif // NOISE_HH

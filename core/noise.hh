#ifndef NOISE_HH
#define NOISE_HH

#include "../CImg.h"
#include <QList>

namespace Noise
{
    void gaussianNoise(cimg_library::CImg<float>& img, double sigma);
    void saltAndPepperNoise(cimg_library::CImg<float>& img, double r);

    enum NoiseType
    {
        Gaussian,
        SaltAndPepper
    };

    struct Type
    {
        NoiseType type;
        double value;

        Type(NoiseType nt, double nv) : type(nt), value(nv) {}
        Type() : type(Gaussian), value(0.0) {}
    };

    typedef QList<Type> Noises;
}

#endif // NOISE_HH

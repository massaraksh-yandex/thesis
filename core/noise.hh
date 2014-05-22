#ifndef NOISE_HH
#define NOISE_HH

#include <QMetaType>
#undef Bool
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
    };

    typedef QList<Type> Noises;
}

Q_DECLARE_METATYPE(Noise::Type)


#endif // NOISE_HH

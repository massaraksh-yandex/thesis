#ifndef KEYPOINT_HH
#define KEYPOINT_HH

#include "global.hh"

struct Keypoint
{
    typedef std::vector<Keypoint> Cont;
    typedef Cont::iterator iterator;

    double X; // колонка
    double Y; // ряд
    double Bl; // блур

    // смещение
    double dx;
    double dy;
    double dz;

    int octave;

    // градиент
    double magnitude;
    double angle;

    std::vector<std::pair<double,double> > angmag; // гистограмма
    int direction; // направление в гистограмме


    Keypoint() {}

    Keypoint(double _x, double _y, double _z, int _o) :
        X(_x), Y(_y), Bl(_z), octave(_o)
    { }

    Cont neighbourhood;
};


void buildDescriptor(Keypoint& point, const CImageDoG &DoG,
                     Descriptor &descriptors);

#endif // KEYPOINT_HH

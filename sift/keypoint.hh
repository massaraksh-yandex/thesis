#ifndef KEYPOINT_HH
#define KEYPOINT_HH

#include "global.hh"

struct Keypoint
{
    typedef std::vector<Keypoint> Cont;
    // x = image column, y = image row, z = scale (blur) index
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

    int direction;		// feature direction (index of histogram bin)
    std::vector<std::pair<double,double> > angmag;	// pair of angle,magnitude

    Cont neighbourhood;	// this saves angle and magnitude of neighbourhood of the feature
                                        // -- this is more like a temporatory variable (it could easily be private)

    Keypoint() {}

    Keypoint(double _x, double _y, double _z, int _o) :
        X(_x), Y(_y), Bl(_z), octave(_o)
    { }
};


void buildDescriptor(Keypoint& point, const CImageDoG &DoG,
                     Descriptor &descriptors);


typedef std::vector<Keypoint> Keypoints;

#endif // KEYPOINT_HH

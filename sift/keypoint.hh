#ifndef KEYPOINT_HH
#define KEYPOINT_HH

#include "global.hh"

struct Keypoint
{
    // x = image column, y = image row, z = scale (blur) index
    double x, y, z;		// pixel position
    double dx, dy, dz;	// subpixel offsets
    int octave;			// octave index
    double magnitude;	// feature magnitude
    double angle;		// feature direction (angle)
    int direction;		// feature direction (index of histogram bin)

    std::vector<Keypoint> neighbourhood;	// this saves angle and magnitude of neighbourhood of the feature
                                        // -- this is more like a temporatory variable (it could easily be private)
    std::vector<std::pair<double,double> > angmag;	// pair of angle,magnitude

    Keypoint() {}

    Keypoint(double _x, double _y, double _z, int _o) :
        x(_x), y(_y), z(_z), octave(_o)
    { }
};

void buildDescriptor(Keypoint& point, const CImageDoG &DoG,
                     Descriptor &descriptors);


typedef std::vector<Keypoint> Keypoints;

#endif // KEYPOINT_HH

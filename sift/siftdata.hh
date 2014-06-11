#ifndef SIFTDATA_HH
#define SIFTDATA_HH

#include "sift_global.hh"
#include "keypoint.hh"

struct SiftData
{
    CImageDoG dog;
    Keypoint::Cont points;
};

#endif // SIFTDATA_HH

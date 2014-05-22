#ifndef SIFTDATA_HH
#define SIFTDATA_HH

#include "global.hh"
#include "keypoint.hh"

struct SiftData
{
    CImageDoG dog;
    Keypoints points;
};

#endif // SIFTDATA_HH

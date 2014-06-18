#ifndef SIFTDATA_HH
#define SIFTDATA_HH

#include "sift_global.hh"
#include "siftkeypoint.hh"

struct SiftData
{
    CImageDoG dog;
    SiftKeypoint::Cont points;
};

#endif // SIFTDATA_HH

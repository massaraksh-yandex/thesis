#ifndef IMAGEACTIONS_H
#define IMAGEACTIONS_H

#include "../global_defines.hh"

struct BaseImageAction
{
    virtual void make(CImageUnsigned&) = 0;
};

struct GaussianNoiseAction : public BaseImageAction
{
    uint sigma;
    GaussianNoiseAction(uint s) : sigma(s) { }
    void make(CImageUnsigned& image) { image.noise(0, sigma); }
};

struct SaltAndPepperNoiseAction : public BaseImageAction
{
    uint broken;
    SaltAndPepperNoiseAction(uint s) : broken(s) { }
    void make(CImageUnsigned& image) { image.noise(2, broken); }
};

struct RotateAction : public BaseImageAction
{
    float angle;
    RotateAction(float a) : angle(a) { }
    void make(CImageUnsigned& image) { image.rotate(angle); }
};

struct ZoomAction : public BaseImageAction
{
    float ratio;
    ZoomAction(float a) : ratio(-100.f*a) { }
    void make(CImageUnsigned& image) { image.resize(ratio, ratio); }
};

typedef QList<BaseImageAction*> ImageActionList;

#endif // IMAGEACTIONS_H

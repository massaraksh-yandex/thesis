#ifndef SIFT_HH
#define SIFT_HH

#include "sift_global.hh"
#include "siftdata.hh"
#include "siftkeypoint.hh"

class Sift
{
    void buildPyramidAndDoG();
    int computeKeypoints();
    int clarifyKeypoints();
    int filterKeypoints();
    void finishKeypoints();

    void buildDescriptor(SiftKeypoint& point, const CImageDoG &DoG,
                         DescriptorArray &descriptors, KeypointList &points);

    bool minimumInLayer(const CImage &img, float pix, int x, int y, bool dontCheckXY);
    bool maximumInLayer(const CImage &img, float pix, int x, int y, bool dontCheckXY);

    void subpixelExtrema(CImageVec &octave, SiftKeypoint &feature);
    void prepareSigmas();
    int kernelSize(double sigma);

    double CONTRAST;
    double CORNER;

    CImageUnsigned img;
    SiftData _data;

public:
    Sift(CImageUnsigned *image, double contrast, double corner);

    double contrast() const { return CONTRAST; }
    double corner() const { return CORNER; }

    void computeDescriptors(DescriptorArray& array, KeypointList& points);
};

extern "C"
{
    void SIFT_EXPORT *create(CImageUnsigned* image, double param1, double param2);
    void SIFT_EXPORT  clear(void* data);
    void SIFT_EXPORT  build(void* data, DescriptorArray* descriptors, KeypointList* keypoints);
    void SIFT_EXPORT  getParams(void* data, double* param1, double* param2);

    void SIFT_EXPORT info(LibraryInfo* info);
}


#endif // SIFT_HH

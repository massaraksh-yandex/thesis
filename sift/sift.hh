#ifndef SIFT_HH
#define SIFT_HH

#include "sift_global.hh"
#include "global.hh"
#include "siftdata.hh"

class SIFTSHARED_EXPORT Sift
{
    cimg_library::CImg<unsigned char> img;
    SiftData _data;
public:
    Sift(const std::string fileName) : CONTRAST(0.02), CORNER(10)
    {
        img.load(fileName.c_str());
    }

    Sift(const CImage image) : img(image)
    { }

    void buildPyramid();
    void getFeatureCandidates();
    void getSubPixelLocations();
    void removeUnstableFeatures();
    void computeFeatureAttributes();

    SiftData& work();

    SiftData& data() { return _data; }

private:
    bool isMin(const CImage &img, float px, int pos_x, int pos_y, bool dont_check_pos_xy);
    bool isMax(const CImage &img, float px, int pos_x, int pos_y, bool dont_check_pos_xy);
    bool getSubPixelExtrema(CImageVec &octave, Keypoint &feature);
    CImage normalize(const cimg_library::CImg<unsigned char> &img);
    void prepareSigmas();
    int getKernelSize(double sigma);
    double CONTRAST;
    double CORNER;
};

extern "C" {
    void SIFTSHARED_EXPORT computeDescriptorsByName(std::string *name, Descriptor* out);
    void SIFTSHARED_EXPORT computeDescriptorsByImage(CImage* image, Descriptor* out);
}

#endif // SIFT_HH

#ifndef SIFT_HH
#define SIFT_HH

#include <QObject>

#include "global.hh"
#include "siftdata.hh"

class Sift : public QObject
{
    Q_OBJECT

    cimg_library::CImg<unsigned char> img;
    SiftData _data;
public:
    Sift(QObject* obj);
    Sift(QString fileName, QObject* obj);
    Sift(CImagePtr image, QObject* obj);

    void load(QString str);
    void load(CImagePtr image);

    void buildPyramid();
    int getFeatureCandidates();
    int getSubPixelLocations();
    int removeUnstableFeatures();
    void computeFeatureAttributes();

    DescriptorPtr work();

    SiftData& data() { return _data; }

private:
    bool minimumInLayer(const CImage &img, float px, int pos_x, int pos_y, bool dontCheckXY);
    bool maximumInLayer(const CImage &img, float px, int pos_x, int pos_y, bool dont_check_pos_xy);
    void getSubPixelExtrema(CImageVec &octave, Keypoint &feature);
    CImage normalize(const cimg_library::CImg<unsigned char> &img);
    void prepareSigmas();
    int getKernelSize(double sigma);
    double CONTRAST;
    double CORNER;
};


#endif // SIFT_HH

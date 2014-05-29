#ifndef SIFT_HH
#define SIFT_HH

#include <QObject>

#include "global.hh"
#include "siftdata.hh"
#include "keypoint.hh"

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

    void buildPyramidAndDoG();
    int computeKeypoints();
    int clarifyKeypoints();
    int filterKeypoints();
    void finishKeypoints();

    void formKeypoints();

    DescriptorPtr computeDescriptors(QList<QPair<int,int>>& points);

    SiftData& data() { return _data; }

private:
    bool minimumInLayer(const CImage &img, float pix, int x, int y, bool dontCheckXY);
    bool maximumInLayer(const CImage &img, float pix, int x, int y, bool dontCheckXY);

    void subpixelExtrema(CImageVec &octave, Keypoint &feature);
    CImage normalize(const cimg_library::CImg<unsigned char> &img);
    void prepareSigmas();
    int kernelSize(double sigma);

    double CONTRAST;
    double CORNER;
};


#endif // SIFT_HH

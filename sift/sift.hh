#ifndef SIFT_HH
#define SIFT_HH

#include <QObject>

#include "global.hh"
#include "siftdata.hh"
#include "keypoint.hh"

class Sift : public QObject
{
    Q_OBJECT

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

//    void interrupt();
private:
    bool minimumInLayer(const CImage &img, float pix, int x, int y, bool dontCheckXY);
    bool maximumInLayer(const CImage &img, float pix, int x, int y, bool dontCheckXY);

    void subpixelExtrema(CImageVec &octave, Keypoint &feature);
    void prepareSigmas();
    int kernelSize(double sigma);

    double CONTRAST;
    double CORNER;

    cimg_library::CImg<unsigned char> img;
    SiftData _data;
};


#endif // SIFT_HH

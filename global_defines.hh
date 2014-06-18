#include <QSharedPointer>
#include <QVector>
#include <QList>
#include "../CImg.h"

typedef QVector<double> Descriptor;
typedef QList<Descriptor> DescriptorArray;
typedef QSharedPointer<DescriptorArray> DescriptorArrayPtr;
typedef cimg_library::CImg<float> CImage;
typedef cimg_library::CImg<unsigned char> CImageUnsigned;
typedef QSharedPointer<CImage> CImagePtr;

struct Keypoint
{
    int x;
    int y;
    Descriptor* descriptor;

    Keypoint() {}
    Keypoint(int _x, int _y) : x(_x), y(_y) { }
};
typedef QList<Keypoint> KeypointList;

enum LibraryType
{
    LibAlgorithm,
    LibComparator
};

struct LibraryInfo
{
    QString path;
    LibraryType type;
    QString info;
    QList<QString> paramNames;
};
typedef QList<LibraryInfo> LibraryInfoList;

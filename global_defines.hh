#ifndef GLOBAL_DEFINES_HH
#define GLOBAL_DEFINES_HH

#include <QSharedPointer>
#include <QVector>
#include <QList>
#include "../CImg.h"

typedef QVector<double> Descriptor;
typedef QList<Descriptor> DescriptorArray;
typedef QSharedPointer<DescriptorArray> DescriptorArrayPtr;
typedef cimg_library::CImg<float> CImage;
typedef cimg_library::CImg<unsigned char> CImageUnsigned;
typedef QSharedPointer<CImageUnsigned> CImageUnsignedPtr;
typedef QSharedPointer<CImage> CImagePtr;
typedef QMap<QString, double> ParamMap;
typedef QVector<double> VectorDouble;

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
    LibraryUnknown,
    LibAlgorithm,
    LibComparator
};

struct LibraryInfo
{
    QString path;
    LibraryType type;
    QString info;
    QString statusMessage;
    bool valid;

    LibraryInfo() : type(LibraryUnknown), valid(false) { }
};
typedef QList<LibraryInfo> LibraryInfoList;

inline QString LibraryAPIVersion() { return QString("0"); }
typedef void (*GetLibraryAPIVersion)(QString*);
typedef void (*LibraryInfoFunc)(LibraryInfo*);


#endif

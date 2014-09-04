#pragma once

#include <QSharedPointer>
#include <QVector>
#include <QList>
#include "../CImg.h"
#include "libraryinfo.hh"

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

inline QString LibraryAPIVersion() { return QString("0"); }
typedef void (*GetLibraryAPIVersion)(QString*);
typedef void (*LibraryInfoFunc)(LibraryInfo*);

template<class Instance>
class SingleInit
{
protected:
    Instance _instance;
public:
    SingleInit() { }
    virtual ~SingleInit() { }
    const Instance* instance() const { return &_instance; }
};


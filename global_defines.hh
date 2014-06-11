#include <QSharedPointer>
#include <QVector>
#include <QList>
#include "../CImg.h"

typedef QVector<double> Descriptor;
typedef QList<Descriptor> DescriptorArray;
typedef QSharedPointer<DescriptorArray> DescriptorArrayPtr;
typedef cimg_library::CImg<float> CImage;
typedef QSharedPointer<CImage> CImagePtr;


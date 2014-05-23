#ifndef GLOBAL_CORE_HH
#define GLOBAL_CORE_HH

#include <QSharedPointer>
#include "../CImg.h"
#include "sift.hh"
#include "noise.hh"

typedef QSharedPointer<CImage> CImagePtr;
typedef QSharedPointer<Descriptor> DescriptorPtr;

#endif // GLOBAL_CORE_HH

#ifndef FUNCTIONSFORTEST_HH
#define FUNCTIONSFORTEST_HH

#include <QSharedPointer>

#include <functional>
#include <algorithm>

#include "global_core.hh"
#include "logging.hh"
#include "tree.hh"

typedef Tree KDTree;
typedef Tree* KDTreePtr;

DescriptorArrayPtr computeDescriptor(CImagePtr img);
ImageTestResults compareDescriptors(DescriptorArrayPtr src, KDTreePtr tree);
CImagePtr computeNoiseImage(CImagePtr src, QPair<ImageNoiseType, double> type);
KDTreePtr buildKDTrees(DescriptorPtr d);

void compareTwoImages(int i, KDTreePtr tr, Map &res, Descriptor &im1Desc, Descriptor& im2Desc);


#endif // FUNCTIONSFORTEST_HH

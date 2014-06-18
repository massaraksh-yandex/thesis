#ifndef FUNCTIONSFORTEST_HH
#define FUNCTIONSFORTEST_HH

#include <QSharedPointer>

#include <functional>
#include <algorithm>

#include "global_core.hh"
#include "logging.hh"
#include "tree.hh"

DescriptorArrayPtr computeDescriptor(CImagePtr img);
ImageTestResults compareDescriptors(DescriptorArrayPtr src, TreePtr tree);
CImagePtr computeNoiseImage(CImagePtr src, QPair<ImageNoiseType, double> type);
TreePtr buildKDTrees(DescriptorArrayPtr d);

void compareTwoImages(int i, TreePtr tr, Map &res, Descriptor &im1Desc, Descriptor& im2Desc);


#endif // FUNCTIONSFORTEST_HH

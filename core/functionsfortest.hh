#ifndef FUNCTIONSFORTEST_HH
#define FUNCTIONSFORTEST_HH

#include <QSharedPointer>

#include <functional>
#include <algorithm>

#include "global_core.hh"
#include "logging.hh"
#include "tree.hh"

DescriptorArrayPtr computeDescriptor(CImageUnsignedPtr img);
ImageTestResults compareDescriptors(DescriptorArrayPtr src, TreePtr tree);
CImageUnsignedPtr computeNoiseImage(CImageUnsignedPtr src, QPair<ImageNoiseType, double> type);
TreePtr buildKDTrees(DescriptorArrayPtr d);

void compareTwoImages(int i, TreePtr tr, Map &res, DescriptorArray &im1Desc, DescriptorArray& im2Desc);


#endif // FUNCTIONSFORTEST_HH

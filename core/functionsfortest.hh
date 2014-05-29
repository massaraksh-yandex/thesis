#ifndef FUNCTIONSFORTEST_HH
#define FUNCTIONSFORTEST_HH

#include <QSharedPointer>

#include <functional>
#include <algorithm>

#include "point_multiset.hpp"
#include "global_core.hh"
#include "logging.hh"

typedef spatial::point_multiset<128, QList<double>> KDTree;
typedef QSharedPointer<KDTree> KDTreePtr;

DescriptorPtr computeDescriptor(CImagePtr img);
double compareDescriptors(DescriptorPtr src, KDTreePtr tree);
CImagePtr computeNoiseImage(CImagePtr src, QPair<ImageNoiseType, double> type);
KDTreePtr buildKDTrees(DescriptorPtr d);

void compareTwoImages(int i, KDTreePtr tr, Map &res, Descriptor &im1Desc, Descriptor& im2Desc);


#endif // FUNCTIONSFORTEST_HH

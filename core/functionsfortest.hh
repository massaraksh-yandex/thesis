#ifndef FUNCTIONSFORTEST_HH
#define FUNCTIONSFORTEST_HH

#include <QSharedPointer>

#include <functional>
#include <algorithm>

#include "point_multiset.hpp"
#include "global_core.hh"
#include "noise.hh"

typedef spatial::point_multiset<128, QList<double> > KDTree;
typedef QSharedPointer<KDTree > KDTreePtr;

double compareDescriptors(DescriptorPtr src, KDTreePtr tree);
CImagePtr computeNoiseImage(CImagePtr src, Noise::Type type);
KDTreePtr buildKDTrees(DescriptorPtr d);

#endif // FUNCTIONSFORTEST_HH

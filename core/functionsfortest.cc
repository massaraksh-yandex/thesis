#include <QDebug>
#include "functionsfortest.hh"
#include "core.hh"

#include "point_multiset.hpp"
#include "bits/spatial_euclidian_neighbor.hpp"

DescriptorPtr computeDescriptor(CImagePtr img)
{
    DescriptorPtr out;

    Sift* sift = new Sift(img, 0);
    out = sift->work();

    return out;
}

double compareDescriptors(DescriptorPtr src, KDTreePtr tree)
{
    KDTree& tr = *tree.data();
    Descriptor d = *src;

    auto euclidianFn = [](double sum, double el) { return sum + el*el; };
    int failed = 0;

    qDebug() << "in" << d.size();
    for(int i = 0; i < d.size(); i++)
    {
        if(i % 100 == 1)qDebug() << i;
        auto iter = spatial::euclidian_neighbor_begin(tr, d[i]);

        double first = std::accumulate(iter->begin(), iter->end(), 0.0, euclidianFn);
        iter++;

        if(iter == spatial::euclidian_neighbor_end(tr, d[i]))
            continue;

        double second = std::accumulate(iter->begin(), iter->end(), 0.0, euclidianFn);

        if(std::sqrt(second / first) <= 2.5)
            failed++;
    }

    qDebug() << "out";

    return (double)(d.size() - failed) / d.size();
}

CImagePtr computeNoiseImage(CImagePtr src, QPair<ImageNoiseType, double> type)
{
    CImagePtr im(new CImage(*src));

    switch(type.first)
    {
    case GIN: gaussianImageNoise(*im, type.second); break;
    case SAPIN: saltAndPepperNoise(*im, type.second); break;
    default:break;
    }

    return im;
}

KDTreePtr buildKDTrees(DescriptorPtr d)
{
    KDTreePtr tree(new KDTree());
    for(int i = 0; i != d->size(); i++)
    {
        tree->insert(d->at(i));
    }

    return tree;
}

#include "functionsfortest.hh"
#include "core.hh"

#include "point_multiset.hpp"
#include "bits/spatial_euclidian_neighbor.hpp"

double compareDescriptors(DescriptorPtr src, KDTreePtr tree)
{
    KDTree& tr = *tree.data();
    Descriptor d = *src;

    auto euclidianFn = [](double sum, double el) { return sum + el*el; };
    int compared = 0;
    for(int i = 0; i < d.size(); i++)
    {
        auto iter = spatial::euclidian_neighbor_begin(tr, d[i]);

        double first = std::accumulate(iter->begin(), iter->end(), 0.0, euclidianFn);
        iter++;
        double second = std::accumulate(iter->begin(), iter->end(), 0.0, euclidianFn);

        if(std::sqrt(second / first) > 2.5)
            compared++;
    }

    return (double)compared / d.size();
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

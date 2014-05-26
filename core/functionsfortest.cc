#include <QDebug>
#include "functionsfortest.hh"
#include "core.hh"

#include "point_multiset.hpp"
#include "bits/spatial_euclidian_neighbor.hpp"

#include <ctime>

DescriptorPtr computeDescriptor(CImagePtr img)
{
    DescriptorPtr out;

    Sift* sift = new Sift(img, 0);
    sift->formKeypoints();
    out = sift->computeDescriptors();

    return out;
}

double compareDescriptors(Descriptor d, KDTreePtr tree)
{
    KDTree& tr = *tree.data();

    auto euclidianFn = [](double sum, double el) { return sum + el*el; };
    int failed = 0;

    for(int i = 0; i < d.size(); i++)
    {
        if(i % 100 == 1)
            qDebug() << i << d.size();

        auto iter = spatial::euclidian_neighbor_begin(tr, d[i]);
        QList<double> list = *iter;
        double first = std::accumulate(list.begin(), list.end(), 0.0, euclidianFn);


        iter++;

        if(iter == spatial::euclidian_neighbor_end(tr, d[i]))
            continue;

        list = *iter;
        double second = std::accumulate(list.begin(), list.end(), 0.0, euclidianFn);

        if(std::sqrt(second / first) <= 1.5)
            failed++;
    }

    return (double)(d.size() - failed) / d.size();
}

CImagePtr computeNoiseImage(CImagePtr src, QPair<ImageNoiseType, double> type)
{
    CImagePtr im(new CImage(*src));

    switch(type.first)
    {
    case GIN:   *im = src->get_noise(type.second * 100, 0); break;
    case SAPIN: *im = src->get_noise(type.second * 100, 2); break;
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

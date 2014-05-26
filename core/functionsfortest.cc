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
    out = sift->work();

    return out;
}

double compareDescriptors(DescriptorPtr src, KDTreePtr tree)
{
    qDebug() << reinterpret_cast<long long>(tree.data());

    KDTree& tr = *tree.data();
    Descriptor d = *src;

    auto euclidianFn = [](double sum, double el) { return sum + el*el; };
    int failed = 0;

    for(int i = 0; i < d.size(); i++)
    {
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

//    clock_t fir = 0, sec = 0;
//    for(int i = 0; i < times.size(); i++)
//    {
//        if(i % 2 == 0)
//            fir += times[i];
//        else
//            sec += times[i];
//    }

//    qDebug() << fir / (times.size()*0.5)  << sec / (times.size()*0.5);
    qDebug() << reinterpret_cast<long long>(tree.data());

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

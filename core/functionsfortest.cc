#include <QDebug>
#include "functionsfortest.hh"
#include "core.hh"
#include <ctime>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/moment.hpp>

using namespace boost::accumulators;

DescriptorArrayPtr computeDescriptor(CImagePtr img)
{
    DescriptorArrayPtr out(new DescriptorArray());

    Sift* sift = new Sift(img, 0);
    sift->formKeypoints();
    KeypointCoords d;
    out = sift->computeDescriptors(d);
    delete sift;

    return out;
}

ImageTestResults compareDescriptors(DescriptorArrayPtr de, KDTreePtr tree)
{
    Descriptor& d = *de;
    accumulator_set<double, stats<tag::mean, tag::moment<2> > > accum;

    int notFailed = 0, missedFirst = 0,
        missedSecond = 0, missedCond = 0;
    for(int i = 0; i < d.size(); i++)
    {
        if(i % 100 == 1)
            qDebug() << i << d.size();

        kdres* kdRes = kd_nearest(tree, &d[i][0]);

        QVector<double> results(128);
        QVector<double> secVect;

        int resSize = kd_res_size(kdRes);
        if(resSize == 1)
        {            
            notFailed++;
            secVect = d[i];
            kd_res_item(kdRes, &results[0]);
        }
        else if(resSize >= 2)
        {
            notFailed+=2;
            secVect.resize(128);
            kd_res_item(kdRes, &results[0]);
            kd_res_next(kdRes);
            kd_res_item(kdRes, &secVect[0]);
        }
        else
            continue;

        double first = 0.0, second = 0.0;
        for(int h = 0; h < d[i].size(); h++)
        {
            first += results[h]*results[h];
            second += secVect[h]*secVect[h];
        }

        kd_res_free(kdRes);
        double ratio = std::sqrt(second / first);

        if(ratio <= 1.15)
        {
            notFailed+=2;
        }
    }
    qDebug() << missedFirst << "," << notFailed << "," << d.size();
    ImageTestResults res;
    res.rate = (double)notFailed / (4*d.size());
    res.average = mean(accum);
    res.sigma = std::sqrt(moment<2>(accum));
    kd_clear(tree);

    return res;
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
    KDTreePtr tree = kd_create(128);
    for(int i = 0; i != d->size(); i++)
    {
        Descriptor& dd = *d;
        kd_insert(tree, &dd[i][0], 0);
    }

    return tree;
}

void compareTwoImages(int i, KDTreePtr tr, Map &res, Descriptor& im1Desc,
                      Descriptor& im2Desc)
{
//    auto euclidianFn = [](double sum, double el) { return sum + el*el; };

    kdres* kdRes = kd_nearest(tr, &im2Desc[i][0]);
//    auto iter = spatial::euclidian_neighbor_begin(*tr, im2Desc[i]);

    if(kd_res_size(kdRes) < 1)
        return;

    int index = -1;
    QVector<double> resVec(128);
    kd_res_item(kdRes, &resVec[0]);
    for(int h = 0; h < im1Desc.size(); h++)
        if(resVec == im1Desc[h])
            index = h;

    if(index == -1)
        return;

//    kd_res_next(kdRes);
//    kd_res_item(kdRes, &resVec[0]);
    kd_res_free(kdRes);

    double first = 0.0, second = 0.0;
    for(int h = 0; h < resVec.size(); h++)
    {
        first += resVec[h]*resVec[h];
        second += im2Desc[i][h]*im2Desc[i][h];
    }

//    double first = std::accumulate(iter->begin(), iter->end(), 0.0, euclidianFn);

//    iter++;

//    if(iter == spatial::euclidian_neighbor_end(*tr, im2Desc[i]))
//        return;

//    double second = std::accumulate(iter->begin(), iter->end(), 0.0, euclidianFn);

    if(std::sqrt(second / first) <= 1.5)
    {
        res[index] = i;
    }
}

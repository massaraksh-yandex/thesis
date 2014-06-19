#include <QDebug>
#include "functionsfortest.hh"
#include "core.hh"
#include "algorithm.hh"
#include <ctime>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/moment.hpp>

using namespace boost::accumulators;

DescriptorArrayPtr computeDescriptor(CImageUnsignedPtr img)
{
    DescriptorArrayPtr out(new DescriptorArray());
    KeypointList kl;

    Algorithm algorithm(*img, Algorithm::defaultValues());
    algorithm.computeDescriptors(*out, kl);

    return out;
}

ImageTestResults compareDescriptors(DescriptorArrayPtr de, TreePtr tree)
{
    DescriptorArray& d = *de;
    accumulator_set<double, stats<tag::mean, tag::moment<2> > > accum;

//    int notFailed = 0, missedFirst = 0,
//        missedSecond = 0, missedCond = 0;
//    for(int i = 0; i < d.size(); i++)
//    {
//        if(i % 100 == 1)
//            qDebug() << i << d.size();

//        kdres* kdRes = kd_nearest(tree, &d[i][0]);

//        QVector<double> results(128);
//        QVector<double> secVect;

//        int resSize = kd_res_size(kdRes);
//        if(resSize == 1)
//        {
//            notFailed++;
//            secVect = d[i];
//            kd_res_item(kdRes, &results[0]);
//        }
//        else if(resSize >= 2)
//        {
//            notFailed+=2;
//            secVect.resize(128);
//            kd_res_item(kdRes, &results[0]);
//            kd_res_next(kdRes);
//            kd_res_item(kdRes, &secVect[0]);
//        }
//        else
//            continue;

//        double first = 0.0, second = 0.0;
//        for(int h = 0; h < d[i].size(); h++)
//        {
//            first += results[h]*results[h];
//            second += secVect[h]*secVect[h];
//        }

//        kd_res_free(kdRes);
//        double ratio = std::sqrt(second / first);

//        if(ratio <= 1.15)
//        {
//            notFailed+=2;
//        }
//    }
//    qDebug() << missedFirst << "," << notFailed << "," << d.size();
    ImageTestResults res;
//    res.rate = (double)notFailed / (4*d.size());
//    res.average = mean(accum);
//    res.sigma = std::sqrt(moment<2>(accum));
//    kd_clear(tree);

    return res;
}

CImageUnsignedPtr computeNoiseImage(CImageUnsignedPtr src, QPair<ImageNoiseType, double> type)
{
    CImageUnsignedPtr im(new CImageUnsigned(*src));
    switch(type.first)
    {
    case GIN:   *im = src->get_noise(type.second * 100, 0); break;
    case SAPIN: *im = src->get_noise(type.second * 100, 2); break;
    default:break;
    }
    return im;
}

TreePtr buildKDTrees(DescriptorArrayPtr d)
{
    TreePtr tree(new Tree(128));
    for(int i = 0; i != d->size(); i++)
    {
        tree->push((*d)[i]);
    }

    return tree;
}

// TODO delete
void compareTwoImages(int i, TreePtr tr, Map &res, DescriptorArray& im1Desc,
                      DescriptorArray& im2Desc)
{


//    kdres* kdRes = kd_nearest(tr, &im2Desc[i][0]);

//    if(kd_res_size(kdRes) < 1)
//        return;

//    int index = -1;
//    QVector<double> resVec(128);
//    kd_res_item(kdRes, &resVec[0]);
//    for(int h = 0; h < im1Desc.size(); h++)
//        if(resVec == im1Desc[h])
//            index = h;

//    if(index == -1)
//        return;

//    kd_res_free(kdRes);

//    if(tr->compareWith())
//    {
//        res[index] = i;
//    }
}

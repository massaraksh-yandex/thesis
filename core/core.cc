#include <QTextStream>
#include <QtConcurrent/QtConcurrent>
#include <QSharedPointer>
#include <QFile>
#include <QDir>

#include "core.hh"
#include "point_multiset.hpp"
#include "bits/spatial_euclidian_neighbor.hpp"

#include <string>
#include <functional>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/moment.hpp>
#include <boost/accumulators/statistics/moment.hpp>


BuildDescriptorsByName Core::computeByName;
BuildDescriptorsByImage Core::computeByImage;

typedef QSharedPointer<CImage> CImagePtr;
typedef QSharedPointer<Descriptor> DescriptorPtr;
typedef spatial::point_multiset<128, QList<double> > KDTree;
typedef QSharedPointer<KDTree > KDTreePtr;
#include <algorithm>

using namespace boost::accumulators;

QPair<double, double> compareDescriptors(DescriptorPtr src, KDTreePtr tree)
{
    KDTree& tr = *tree.data();
    Descriptor d = *src;

    QList<double> zero;
    std::fill_n(zero.begin(), 128, 0.0);
    QList<double> results;
    results.reserve(d.size());
    accumulator_set<double, stats<tag::mean, tag::moment<2> > > acc;
    for(int i = 0; i < d.size(); i++)
    {
        spatial::euclidian<KDTree, double, double> metric;
        auto begin = spatial::euclidian_neighbor_begin(tr, d[i]);

        double first = metric.distance_to_key(128, zero, *begin++);
        double second = metric.distance_to_key(128, zero, *begin);;

        acc(second / first);
    }

    return qMakePair(mean(acc), moment<2>(acc));
}

CImagePtr computeNoiseImage(CImagePtr src, Noise::Type type)
{
    CImagePtr im(new CImage(*im));

    switch(type.type)
    {
    case Noise::Gaussian: Noise::gaussianNoise(*im, type.value); break;
    case Noise::SaltAndPepper: Noise::saltAndPepperNoise(*im, type.value); break;
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

DescriptorPtr computeDescriptor(CImagePtr img)
{
    DescriptorPtr out(new Descriptor());
    Core::computeByImage(img.data(), out.data());
    return out;
}

DescriptorId Core::generateId() const
{
    static DescriptorId rr = 0;
    return rr++;
}

void Core::load()
{
    if(!lib.load()) {
        emit log(Log::Error, "Cannot load library " + lib.fileName());
        return;
    }

    computeByName = (BuildDescriptorsByName)lib.resolve("computeDescriptorsByName");
    computeByImage = (BuildDescriptorsByImage)lib.resolve("computeDescriptorsByImage");
}

void Core::computeDescriptors(QString image)
{
    Descriptor desk;
    std::string str = image.toStdString();
    computeByName(&str, &desk);

    data.push_back(desk);

    emit computingFinished(data.size()-1);
}

void Core::writeDescriptor(DescriptorId id, QString name)
{
    if(data.size() >= id)
    {
        emit log(Log::Error, "Unknown descriptor " + QString::number(id));
        return;
    }

    QFile file(name);
    if(file.open(QFile::ReadOnly | QFile::Text))
    {
        emit log(Log::Error, "Cannot open file " + name);
        return;
    }

    Descriptor& d = data[id];
    QTextStream stream(&file);

    for(int i = 0; i < d.size(); i++)
    {
        for(int h = 0; h < d[i].size(); h++)
        {
            stream << d[i][h] << " ";
        }
        stream << endl;
    }
    file.close();
}

double Core::compareImages(DescriptorId im1, DescriptorId im2, bool _emit)
{
    
}

void Core::testImages(QString dirName, Noise::Noises types)
{
    QDir dir(dirName);
    dir.setNameFilters(QStringList() << "*.jpg" << "*.png");

    QFileInfoList files = dir.entryInfoList();

    emit progress(0, files.size());
    QList<QPair<double, double> > results;
    for(int i = 0; i < files.size(); i++)
    {
        QFileInfo& fi = files[i];
        try
        {
            CImagePtr image(new CImage());
            image->load(fi.absoluteFilePath().toStdString().c_str());
            emit log(Log::Message, QString("Файл").arg(fi.fileName()));

            std::function<CImagePtr(Noise::Type)> buildNoised = [image](Noise::Type type) { return computeNoiseImage(image, type); };

            auto images = QtConcurrent::blockingMapped<QList<CImagePtr> >(types.begin(), types.end(), buildNoised);
            emit log(Log::Message, QString("\tСозданы изображения с шумами"));

            images.push_back(image);

            auto descriptors = QtConcurrent::blockingMapped<QList<DescriptorPtr> >(images, computeDescriptor);
            emit log(Log::Message, QString("\tДескрипторы посчитаны"));

            DescriptorPtr sourceDescr = descriptors.last();
            descriptors.pop_back();

            auto forest = QtConcurrent::blockingMapped<QList<KDTreePtr> >(descriptors, buildKDTrees);
            emit log(Log::Message, QString("\tК-мерные деревья сформированы"));


            std::function<QPair<double, double>(KDTreePtr)> compareTrees = [sourceDescr](KDTreePtr tree) { return compareDescriptors(sourceDescr, tree); };

            QList<QPair<double, double> > currentResults =
                    QtConcurrent::blockingMapped<QList<QPair<double, double> > >(forest, compareTrees);

            emit log(Log::Message, QString("\tОбработка закончена"));

            results += currentResults;
        }
        catch(...)
        {
            emit log(Log::Fail, QString("%1: Ошибка, пропускается...").arg(fi.fileName()));
            continue;
        }

        emit progress(i+1, files.size());
    }

    emit testingFinished();
}


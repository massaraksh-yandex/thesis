#include <QTextStream>
#include <QtConcurrent/QtConcurrent>
#include <QSharedPointer>
#include <QFile>
#include <QDir>

#include "functionsfortest.hh"
#include "core.hh"

#include <functional>
#include <algorithm>
#include <string>

typedef spatial::point_multiset<128, QList<double> > KDTree;
typedef QSharedPointer<KDTree > KDTreePtr;

BuildDescriptorsByName Core::computeByName;
BuildDescriptorsByImage Core::computeByImage;

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
        emit log(Log::Error, 0, "Cannot load library " + lib.fileName());
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
        emit log(Log::Error, 0, "Unknown descriptor " + QString::number(id));
        return;
    }

    QFile file(name);
    if(file.open(QFile::ReadOnly | QFile::Text))
    {
        emit log(Log::Error, 0, "Cannot open file " + name);
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
    TestingResults results;

    emit progress(0, files.size());
    for(int i = 0; i < files.size(); i++)
    {
        QFileInfo& fi = files[i];
        try
        {
            CImagePtr image(new CImage());
            image->load(fi.absoluteFilePath().toStdString().c_str());
            emit log(Log::Message, 0, QString("Файл").arg(fi.fileName()));

            std::function<CImagePtr(Noise::Type)> buildNoised = [image](Noise::Type type) { return computeNoiseImage(image, type); };

            auto images = QtConcurrent::blockingMapped<QList<CImagePtr> >(types.begin(), types.end(), buildNoised);
            emit log(Log::Message, 1, QString("Созданы изображения с шумами"));

            images.push_back(image);

            auto descriptors = QtConcurrent::blockingMapped<QList<DescriptorPtr> >(images, computeDescriptor);
            emit log(Log::Message, 1, QString("Дескрипторы посчитаны"));

            DescriptorPtr sourceDescr = descriptors.last();
            descriptors.pop_back();

            auto forest = QtConcurrent::blockingMapped<QList<KDTreePtr> >(descriptors, buildKDTrees);
            emit log(Log::Message, 1, QString("К-мерные деревья сформированы"));


            std::function<double(KDTreePtr)> compareTrees = [sourceDescr](KDTreePtr tree) { return compareDescriptors(sourceDescr, tree); };

            QList<double> currentResults = QtConcurrent::blockingMapped<QList<double> >(forest, compareTrees);

            emit log(Log::Message, 1, QString("Обработка закончена"));

            results.push_back(TestingResult(fi.fileName(), currentResults));
        }
        catch(...)
        {
            emit log(Log::Fail, 0, QString("%1: Ошибка, пропускается...").arg(fi.fileName()));
            continue;
        }

        emit progress(i+1, files.size());
    }

    emit testingFinished(results);
}


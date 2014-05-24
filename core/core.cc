#include <QTextStream>
#include <QtConcurrent/QtConcurrent>
#include <QSharedPointer>
#include <QFile>
#include <QDir>

#include "functionsfortest.hh"
#include "core.hh"
#include "sift.hh"

#include <functional>
#include <algorithm>
#include <string>


typedef spatial::point_multiset<128, QList<double> > KDTree;
typedef QSharedPointer<KDTree > KDTreePtr;

DescriptorPtr computeDescriptor(CImagePtr img)
{
    DescriptorPtr out;
    qDebug() << "in";

    Sift* sift = new Sift(img, 0);
    out = sift->work();

    qDebug() << "out";
    return out;
}

DescriptorId Core::generateId() const
{
    static DescriptorId rr = 0;
    return rr++;
}

void Core::computeDescriptors(QString image)
{
    Descriptor desk;

    Sift* sift = new Sift(image, 0);

    data.push_back(sift->work());

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

    Descriptor& d = *data[id];
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

void Core::compareImages(DescriptorId im1, DescriptorId im2)
{
    
}

void Core::testImages(QString dirName, ImageNoises types)
{
    qDebug() << "here";
    QDir dir(dirName);
    QStringList l; l << "*.jpg" << "*.png";
    dir.setNameFilters(l);

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
            emit log(Log::Message, 0, QString("Файл %1").arg(fi.fileName()));

            std::function<CImagePtr(QPair<ImageNoiseType, double>)> buildNoised =
                    [image](QPair<ImageNoiseType, double> type) { return computeNoiseImage(image, type); };

            auto images = QtConcurrent::blockingMapped<QList<CImagePtr> >(types.begin(),
                                                                          types.end(), buildNoised);
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


Q_DECLARE_METATYPE(DescriptorId)
Q_DECLARE_METATYPE(TestingResult)
Q_DECLARE_METATYPE(TestingResults)
Q_DECLARE_METATYPE(Log::LogType)
Q_DECLARE_METATYPE(ImageNoiseType)
Q_DECLARE_METATYPE(ImageNoisePair)
Q_DECLARE_METATYPE(ImageNoises)



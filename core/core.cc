#include <QtConcurrent/QtConcurrent>
#include <QSharedPointer>
#include <QTextStream>
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

void Core::keypointsFromFile(QString str, KeypointCoords& coords, Descriptor& desc, QString& error)
{
    QFile file(str);

    if(file.open(QFile::Text | QFile::ReadOnly))
    {
        error = "Не могу открыть файл" + str;
        return;
    }

    QTextStream stream(&file);

    while(!stream.atEnd())
    {
        auto lineStr1 = stream.readLine();
        if(lineStr1.isEmpty())
            break;
        auto lineStr = lineStr1.split(',');

        bool bX, bY;
        int x = lineStr[0].toInt(&bX);
        int y = lineStr[1].toInt(&bY);

        if(!bX || !bY)
        {
            error = "Ошибка в файле";
            return;
        }

        coords.push_back(qMakePair(x, y));

        QList<double> lineDouble;
        lineDouble.reserve(lineStr.size() - 2);
        for(int i = 2; i < lineStr.size(); i++)
        {
            QString& str = lineStr[i];
            bool b;
            double dd = str.toDouble(&b);
            if(!bX)
            {
                error = "Ошибка в файле";
                return;
            }

            lineDouble.push_back(dd);
        }

        if(lineDouble.size() != 128)
        {
            error = "Ошибка в файле";
            return;
        }
        desc.push_back(lineDouble);
    }
}

void Core::computeDescriptorsToFile(QString image, QString filename)
{
    QFile file(filename);
    if(!file.open(QFile::Text | QFile::WriteOnly))
    {
        emit log(Log::Error, 0, "Не могу открыть файл " + filename);
        emit failed();
        return;
    }
    emit progress(0, 0);
    Sift* sift = new Sift(image, 0);

    KeypointCoords coords = sift->formKeypoints();
    DescriptorPtr ptr = sift->computeDescriptors();

    QTextStream stream(&file);

    for(int i = 0; i < coords.size(); i++)
    {
        stream << coords[i].first << "," << coords[i].second << ",";
        Descriptor::value_type& d = (*ptr)[i];
        for(int h = 0; h < d.size()-1; h++)
            stream << d[h] << ",";
        stream << d.back() << endl;
    }

    delete sift;

    emit progress(0, 1);
    emit log(Log::Message, 0, "Дескрипторы записаны в файл " + filename);
    emit writingFinished();
}

void Core::compareImages(QString im1, QString im2, int types)
{
    KeypointCoords im1Coords, im2Coords;
    Descriptor im1Desc, im2Desc;

    QFile f1(im1), f2(im2);
    if(!f1.open(QFile::Text | QFile::WriteOnly))
    {
        emit log(Log::Error, 0, "Не могу открыть файл " + im1);
        emit failed();
        return;
    }
    if(!f2.open(QFile::Text | QFile::WriteOnly))
    {
        emit log(Log::Error, 0, "Не могу открыть файл " + im2);
        emit failed();
        return;
    }

    QString error;
    if(types & 1)
    {
        keypointsFromFile(im1, im1Coords, im1Desc, error);
        if(!error.isEmpty())
        {
            emit log(Log::Error, 0, error);
            emit failed();
            return;
        }
    }
    else
    {
        Sift* sift = new Sift(im1, 0);

        im1Coords = sift->formKeypoints();
        im1Desc = *sift->computeDescriptors();
        delete sift;
    }

    if(types & 2)
    {
        keypointsFromFile(im2, im2Coords, im2Desc, error);
        if(!error.isEmpty())
        {
            emit log(Log::Error, 0, error);
            emit failed();
            return;
        }
    }
    else
    {
        Sift* sift = new Sift(im2, 0);

        im2Coords = sift->formKeypoints();
        im2Desc = *sift->computeDescriptors();
        delete sift;
    }

    KDTreePtr tree(new KDTree());
    for(int i = 0; i != im1Desc[i].size(); i++)
    {
        tree->insert(im1Desc[i]);
    }

    QMap<double, double> res;

    for(int i = 0; i < im1Desc.size(); i++)
    {
        emit progress(i, im1Desc.size());
        compareTwoImages(i, tree, im1Desc, res, im2Desc);
    }

    emit compared(res, im1Coords, im2Coords);
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
        qDebug() << fi.fileName();
        try
        {
            CImagePtr image(new CImage());
            image->load(fi.absoluteFilePath().toStdString().c_str());
            emit log(Log::Message, 0, QString("Файл %1").arg(fi.fileName()));

            std::function<CImagePtr(QPair<ImageNoiseType, double>)> buildNoised =
                    [image](QPair<ImageNoiseType, double> type) { return computeNoiseImage(image, type); };

            auto images = QtConcurrent::blockingMapped<QList<CImagePtr> >(types.begin(),
                                                                          types.end(), buildNoised);
            emit log(Log::Message, 2, QString("Созданы изображения с шумами"));

            images.push_back(image);

            auto descriptors = QtConcurrent::blockingMapped<QList<DescriptorPtr> >(images, computeDescriptor);
            emit log(Log::Message, 2, QString("Дескрипторы посчитаны"));
            for(CImagePtr p : images) p->clear();

            DescriptorPtr sourceDescr = descriptors.last();
            descriptors.pop_back();

            auto forest = QtConcurrent::blockingMapped<QList<KDTreePtr> >(descriptors, buildKDTrees);
            emit log(Log::Message, 2, QString("К-мерные деревья сформированы"));

            std::function<double(KDTreePtr)> compareTrees =
                    [sourceDescr](KDTreePtr tree)
            { return compareDescriptors(sourceDescr, tree); };

            QList<double> currentResults = QtConcurrent::blockingMapped<QList<double> >(forest, compareTrees);
            for(KDTreePtr tr : forest) tr.clear();

            emit log(Log::Message, 2, QString("Обработка закончена"));

            TestingResult res;
            res.filename = fi.fileName();
            for(double d : currentResults)
                res.results.push_back(d);
            results.push_back(res);
        }
        catch(...)
        {
            emit log(Log::Fail, 0, QString("%1: Ошибка, пропускается...").arg(fi.fileName()));
            continue;
        }

        emit progress(i+1, files.size());
    }

    for(int i = 0; i < results.size(); i++)
        qDebug() << results[i].filename << results[i].results;

    emit testingFinished(results);
}

Q_DECLARE_METATYPE(Keypoint)
Q_DECLARE_METATYPE(TestingResult)
Q_DECLARE_METATYPE(TestingResults)
Q_DECLARE_METATYPE(Log::LogType)
Q_DECLARE_METATYPE(ImageNoiseType)
Q_DECLARE_METATYPE(ImageNoisePair)
Q_DECLARE_METATYPE(ImageNoises)

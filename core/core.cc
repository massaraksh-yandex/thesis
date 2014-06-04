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

    if(file.open(QFile::Text | QFile::ReadOnly)) {
        error = "Не могу открыть файл" + str + "\n";
        return;
    }

    QTextStream stream(&file);

    while(!stream.atEnd()) {
        auto lineStr1 = stream.readLine();
        if(lineStr1.isEmpty())
            break;
        auto lineStr = lineStr1.split(',');

        bool bX, bY;
        int x = lineStr[0].toInt(&bX);
        int y = lineStr[1].toInt(&bY);

        if(!bX || !bY) {
            error = "Ошибка в файле\n";
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
            if(!b) {
                error = "Ошибка в файле\n";
                return;
            }

            lineDouble.push_back(dd);
        }

        if(lineDouble.size() != 128) {
            error = "Ошибка в файле\n";
            return;
        }
        desc.push_back(lineDouble);
    }
}

void Core::buildDescriptors(QString image, QString filename)
{
    emit running(true);
    _interrupt = false;

    QFile file(filename);
    if(!file.open(QFile::Text | QFile::WriteOnly)) {
        emit log(Log::Error, 0, "Не могу открыть файл " + filename + "\n");
        emit running(false);
        return;
    }

    emit progress(0, 0);
    emit log(Log::Message, 0, "Построение SIFT-дескрипторов для изображения " + filename);

    KeypointCoords coords;
    DescriptorPtr ptr;

    std::shared_ptr<Sift> sift(new Sift(image, 0));
    sift->formKeypoints();
    ptr = sift->computeDescriptors(coords);

    QTextStream stream(&file);
    for(int i = 0; i < coords.size(); i++) {
        if(_interrupt) {
            emit running(false);
            return;
        }

        stream << coords[i].first << "," << coords[i].second << ",";
        Descriptor::value_type& d = (*ptr)[i];
        for(int h = 0; h < d.size()-1; h++)
            stream << d[h] << ",";
        stream << d.back() << endl;
    }

    emit log(Log::Message, 0, "Дескрипторы записаны в файл " + filename + "\n");
    emit running(false);
    emit progress(0, 1);
    emit buildDescriptorsComplete();
}

void Core::compareImages(QString im1, QString im2, int types)
{
    emit running(true);
    emit log(Log::Message, 0, "Начало сравнивания изображений...\n");
    _interrupt = false;

    KeypointCoords coords[2];
    Descriptor descr[2];
    QString ims[] = { im1, im2 };

    QString error;
    std::shared_ptr<Sift> sift(new Sift(0));

    for(int i = 0; i < 2; i++) {
        if(_interrupt) {
            emit running(false);
            return;
        }

        if(types & (i+1)) {
            keypointsFromFile(ims[i], coords[i], descr[i], error);
            if(!error.isEmpty()) {
                emit log(Log::Error, 0, error);
                emit running(false);
                return;
            }
        }
        else {
            sift->load(ims[i]);
            sift->formKeypoints();
            descr[i] = *sift->computeDescriptors(coords[i]);
        }
    }

    KDTreePtr tree(new KDTree());
    for(int i = 0; i != descr[0].size(); i++) {
        if(_interrupt) {
            emit running(false);
            return;
        }
        tree->insert(descr[0][i]);
    }

    emit log(Log::Message, 0, "Сравнение дескрипторов\n");
    Map res;
    for(int i = 0; i < descr[1].size(); i++) {
        if(_interrupt) {
            emit running(false);
            return;
        }

        emit progress(i, descr[1].size());
        compareTwoImages(i, tree, res, descr[0], descr[1]);
    }

    emit running(false);
    emit progress(0, 1);
    emit compareImagesComplete(res, coords[0], coords[1]);
}

void Core::testImages(QString dirName, ImageNoises types)
{
    emit running(true);
    _interrupt = false;

    QDir dir(dirName);
    QStringList l; l << "*.jpg" << "*.png";
    dir.setNameFilters(l);

    QFileInfoList files = dir.entryInfoList();
    TestingResults results;

    emit progress(0, files.size());
    for(int i = 0; i < files.size(); i++)
    {
        if(_interrupt) {
            emit running(false);
            return;
        };

        QFileInfo& fi = files[i];
        qDebug() << fi.fileName();
        try
        {
            CImagePtr image(new CImage());
            image->load(fi.absoluteFilePath().toStdString().c_str());
            emit log(Log::Message, 0, QString("Файл %1\n").arg(fi.fileName()));

            std::function<CImagePtr(QPair<ImageNoiseType, double>)> buildNoised =
                    [image](QPair<ImageNoiseType, double> type) { return computeNoiseImage(image, type); };

            auto images = QtConcurrent::blockingMapped<QList<CImagePtr> >(types.begin(),
                                                                          types.end(), buildNoised);
            emit log(Log::Message, 2, QString("Созданы изображения с шумами\n"));

            images.push_back(image);

            auto descriptors = QtConcurrent::blockingMapped<QList<DescriptorPtr> >(images, computeDescriptor);
            emit log(Log::Message, 2, QString("Дескрипторы посчитаны\n"));
            for(CImagePtr p : images) p->clear();

            DescriptorPtr sourceDescr = descriptors.last();
            descriptors.pop_back();

            auto forest = QtConcurrent::blockingMapped<QList<KDTreePtr> >(descriptors, buildKDTrees);
            emit log(Log::Message, 2, QString("К-мерные деревья сформированы\n"));

            std::function<double(KDTreePtr)> compareTrees =
                    [sourceDescr](KDTreePtr tree)
            { return compareDescriptors(sourceDescr, tree); };

            QList<double> currentResults = QtConcurrent::blockingMapped<QList<double> >(forest, compareTrees);
            for(KDTreePtr tr : forest) tr.clear();

            emit log(Log::Message, 2, QString("Обработка закончена\n"));

            TestingResult res;
            res.filename = fi.fileName();
            for(double d : currentResults)
                res.results.push_back(d);
            results.push_back(res);
        }
        catch(...)
        {
            emit log(Log::Fail, 0, QString("%1 пропускается...\n").arg(fi.fileName()));
            continue;
        }

        emit progress(i+1, files.size());
    }

    emit running(false);
    emit testimagesComplete(results);
}

void Core::interrupt()
{
    _interrupt = true;
}

Q_DECLARE_METATYPE(Keypoint)
Q_DECLARE_METATYPE(TestingResult)
Q_DECLARE_METATYPE(TestingResults)
Q_DECLARE_METATYPE(Log::LogType)
Q_DECLARE_METATYPE(ImageNoiseType)
Q_DECLARE_METATYPE(ImageNoisePair)
Q_DECLARE_METATYPE(ImageNoises)

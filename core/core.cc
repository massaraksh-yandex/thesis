#include <QtConcurrent/QtConcurrent>
#include <QSharedPointer>
#include <QTextStream>
#include <QFile>
#include <QDir>

#include "functionsfortest.hh"
#include "core.hh"
#include "algorithm.hh"

#include <functional>
#include <algorithm>
#include <string>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/moment.hpp>

using namespace boost::accumulators;

void Core::keypointsFromFile(QString str, KeypointCoords& coords, DescriptorArray& desc, QString& error)
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

        QVector<double> lineDouble;
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
    emit log(Log::Message, 0, QString("Построение SIFT-дескрипторов для изображения %1\n").arg(filename));

    KeypointList coords;
    DescriptorArray ptr;

    Algorithm sift(image, 0.03, 10.);
    sift.computeDescriptors(ptr, coords);

    QTextStream stream(&file);
    for(int i = 0; i < coords.size(); i++) {
        if(_interrupt) {
            emit running(false);
            return;
        }

        stream << coords[i].x << "," << coords[i].y << ",";
        DescriptorArray::value_type& d = ptr[i];
        for(int h = 0; h < d.size()-1; h++)
            stream << d[h] << ",";
        stream << d.back() << endl;
    }

    emit log(Log::Message, 0, "Дескрипторы записаны в файл " + filename + "\n");
    emit running(false);
    emit progress(0, 1);
    emit buildDescriptorsComplete();
}

void Core::compareImages(QString im1, QString im2)
{
    emit running(true);
    _interrupt = false;

    KeypointList coords[2];
    DescriptorArray descr[2];
    QString ims[] = { im1, im2 };


    for(int i = 0; i < 2; i++) {
        if(_interrupt) {
            emit running(false);
            return;
        }
        Algorithm alg(ims[i], 0.03, 10.0);
        emit log(Log::Message, 0,
                 QString("Построение SIFT-дескрипторов для изображения %1\n").arg(ims[i]));

        alg.computeDescriptors(descr[i], );
        sift->formKeypoints();
        descr[i] = *sift->computeDescriptors(coords[i]);
    }

    TreePtr tree(128);
    for(int i = 0; i != descr[0].size(); i++) {
        if(_interrupt) {
            emit running(false);
            return;
        }
        tree->push(descr[0][i]);
    }

    emit log(Log::Message, 0, "Сравнение дескрипторов\n");
    Map res;
    QString finishMessage;
    for(int i = 0; i < descr[1].size(); i++) {
        if(_interrupt) {
            goto END;
        }

        emit progress(i, descr[1].size());
        compareTwoImages(i, tree, res, descr[0], descr[1]);
    }

    finishMessage = QString("Дескрипторы первого изображения:  %1\n"
                                    "Дескрипторы второго изображения:  %2\n"
                                    "Совпавшие дескрипторы: %3\n").arg(descr[0].size())
                                    .arg(descr[1].size()).arg(res.size());

    emit progress(0, 1);
    emit log(Log::Message, 0, finishMessage);
    END:;
    kd_clear(tree);
    emit running(false);

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
    TestingResultList results;
    QVector<accumulator_set<double, stats<tag::mean> >> accumRate(types.size()),
                                                        accumAv(types.size()),
                                                        accumSigma(types.size());

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

            std::function<ImageTestResults(KDTreePtr)> compareTrees =
                    [sourceDescr](KDTreePtr tree)
            { return compareDescriptors(sourceDescr, tree); };

            QList<ImageTestResults> currentResults =
                    QtConcurrent::blockingMapped<QList<ImageTestResults> >(forest, compareTrees);

            emit log(Log::Message, 2, QString("Обработка закончена\n"));

            for(int h = 0; h < currentResults.size(); h++)
            {
                accumAv[h](currentResults[h].average);
                accumRate[h](currentResults[h].rate);
                accumSigma[h](currentResults[h].sigma);
            }
        }
        catch(...)
        {
            emit log(Log::Fail, 0, QString("%1 пропускается...\n").arg(fi.fileName()));
            continue;
        }

        emit progress(i+1, files.size());
    }

    for(int i = 0; i < accumAv.size(); i++)
    {
        ImageTestResults out;
        out.average = mean(accumAv[i]);
        out.rate = mean(accumRate[i]);
        out.sigma = mean(accumSigma[i]);

        results.push_back(out);
//        qDebug() << "out == " << results.back().average << results.back().rate << results.back().sigma;
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
Q_DECLARE_METATYPE(TestingResultList)
Q_DECLARE_METATYPE(Log::LogType)
Q_DECLARE_METATYPE(ImageNoiseType)
Q_DECLARE_METATYPE(ImageNoisePair)
Q_DECLARE_METATYPE(ImageNoises)
Q_DECLARE_METATYPE(ImageTestResults)

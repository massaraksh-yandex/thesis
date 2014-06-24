#include "compareimages.hh"
#include "algorithm.hh"
#include "tree.hh"
#include "functionsfortest.hh"
#include <QtConcurrent>


CompareImages::CompareImages(QObject *parent) :
    QObject(parent)
{
    connect(&_computingAlgorithm, SIGNAL(finished()), SLOT(algorithmExecuted()));
    connect(&_comparingDescriptor, SIGNAL(finished()), this, SLOT(algorithmExecuted()), Qt::QueuedConnection);
}

void CompareImages::exec(CompareImagesData data)
{
    if(_computingAlgorithm.isRunning() || _comparingDescriptor.isRunning())
        throw std::logic_error("Trying to compare images while running");

    _data = data;

    _algs.clear();
    _algs.push_back(AlgorithmPtr(new Algorithm(data.image1, data.params, false)));
    _algs.push_back(AlgorithmPtr(new Algorithm(data.image2, data.params, false)));

    QFuture<void> comp = QtConcurrent::map(_algs,
                                           [&_algs](AlgorithmPtr ptr)
                                           { ptr->execute(); } );
    _computingAlgorithm.setFuture(comp);
}

void CompareImages::algorithmExecuted()
{

}

void CompareImages::compareDescriptors()
{
    KeypointList& coords[] = { _algs[0]->keypointList(), _algs[1]->keypointList() };
    DescriptorArray& descr[] = { _algs[0]->descriptor(), _algs[1]->descriptor()   };

    TreePtr tree = buildKDTree(QSharedPointer(&descr[0]));

    emit log(Log::Message, 0, "Сравнение дескрипторов\n");
    QString finishMessage;
    CompareImagesResult result;
    QList<QPair<Keypoint, Keypoint>> Map;

    auto compareDs = [&result, &Map](Keypoint* point) -> int
    {
        Keypoint kp;
        return tree->compareWith(*point[i].descriptor, kp);
    };


    for(int i = 0; i < coords[1].size(); i++) {

        if(!_interrupt) {
            Keypoint kp;
            if(tree->compareWith(*coords[1][i].descriptor, kp))
            {
                result.equals++;
            }
        }
    }
//            emit progress(i, descr[1].size());

    if(!_interrupt) {
        finishMessage = QString("Дескрипторы первого изображения:  %1\n"
                                        "Дескрипторы второго изображения:  %2\n"
                                        "Совпавшие дескрипторы: %3\n").arg(descr[0].size())
                                        .arg(descr[1].size()).arg(res.size());

    }
    emit progress(0, 1);
    emit log(Log::Message, 0, finishMessage);

    emit finished(result);
}

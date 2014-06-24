#ifndef COMPAREIMAGES_HH
#define COMPAREIMAGES_HH

#include "global_core.hh"
#include "algorithm.hh"

#include <QFutureWatcher>
#include <QObject>

struct CompareImagesData
{
    CImageUnsignedPtr image1;
    CImageUnsignedPtr image2;
    VectorDouble params;
};

struct CompareImagesResult
{
    CompareImagesData data;
    uint equals;
    QList<QPair<Keypoint, Keypoint>> map;
};

class Alorithm;
class CompareImages : public QObject
{
    Q_OBJECT

    CompareImagesData _data;

    QList<AlgorithmPtr> _algs;

    QFutureWatcher<void> _computingAlgorithm;
    QFutureWatcher<CompareImagesResult> _comparingDescriptor;

    ADD_LOGGING
    ADD_PROGRESS
    ADD_INTERRUPTABLE(_computingAlgorithm)

    void compareDescriptors();
public:
    explicit CompareImages(QObject *parent = 0);

    void exec(CompareImagesData data);

signals:
    void finished(CompareImagesResult);

private slots:
    void algorithmExecuted();
};

#endif // COMPAREIMAGES_HH

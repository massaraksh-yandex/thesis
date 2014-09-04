#include <QtConcurrent>
#include <QDebug>
#include "algorithm.hh"

uint Algorithm::_mutex = 0;

Algorithm::Algorithm(QString image, const VectorDouble &vector, bool async, QObject* parent = 0)
    : QObject(parent), _async(async)
{
    CImageUnsigned im(image.toStdString().c_str());
    _data = api->create(&im, &vector);
    _mutex++;

    connect(&_watchDog, SIGNAL(finished()), SLOT(computingFinished()));
}

Algorithm::Algorithm(CImageUnsigned& image, const VectorDouble &vector, bool async, QObject *parent)
    : QObject(parent), _async(async)
{
    _data = api->create(&image, &vector);
    _mutex++;
}

Algorithm::~Algorithm()
{
    api->clear(_data);
    _mutex--;
}

double Algorithm::param(uint i) const
{
    VectorDouble p;
    api->getParams(_data, &p);

    return p[i];
}

QString Algorithm::paramName(uint i)
{
    QStringList p;
    api->getParamNames(&p);

    return p[i];
}

VectorDouble Algorithm::defaultValues()
{
    VectorDouble p;
    api->getDefaultValues(&p);

    return p;
}

void Algorithm::execAsync(int val)
{
    _async = (bool)val;
}

void Algorithm::execute()
{
    if(_watchDog.isRunning())
        throw std::logic_error("Trying to execute algorithm while running");

    if(_async)
        _watchDog.setFuture(QtConcurrent::run(api->build, _data, &_descriptor, &_keypointList));
    else
        api->build(_data, &_descriptor, &_keypointList);
}

void Algorithm::tryCancel()
{
    if(_async && _watchDog.isRunning()) {
        _watchDog.cancel();
        _watchDog.waitForFinished();
    }
}

void Algorithm::computingFinished()
{
    emit finished();
}

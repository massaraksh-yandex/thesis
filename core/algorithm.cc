#include <QtConcurrent>
#include <QDebug>
#include "algorithm.hh"

uint Algorithm::_mutex = 0;
Algorithm::Create 			Algorithm::_create;
Algorithm::Clear 			Algorithm::_clear;
Algorithm::Build 			Algorithm::_build;
Algorithm::GetParams 		Algorithm::_getParams;
Algorithm::GetDefaultValues Algorithm::_getDefaultValues;
Algorithm::GetParamNames 	Algorithm::_getParamNames;

Algorithm::Algorithm(QString image, const VectorDouble &vector, bool async, QObject* parent = 0)
    : QObject(parent), _async(async)
{
    CImageUnsigned im(image.toStdString().c_str());
    _data = _create(&im, &vector);
    _mutex++;

    connect(&_watchDog, SIGNAL(finished()), SLOT(computingFinished()));
}

Algorithm::Algorithm(CImageUnsigned& image, const VectorDouble &vector, bool async, QObject *parent)
    : QObject(parent), _async(async)
{
    _data = _create(&image, &vector);
    _mutex++;
}

Algorithm::~Algorithm()
{
    _clear(_data);
    _mutex--;
}

double Algorithm::param(uint i) const
{
    VectorDouble p;
    _getParams(_data, &p);

    return p[i];
}

QString Algorithm::paramName(uint i)
{
    QStringList p;
    _getParamNames(&p);

    return p[i];
}

VectorDouble Algorithm::defaultValues()
{
    VectorDouble p;
    _getDefaultValues(&p);

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
        _watchDog.setFuture(QtConcurrent::run(_build, _data, &_descriptor, &_keypointList));
    else
        _build(_data, &_descriptor, &_keypointList);
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

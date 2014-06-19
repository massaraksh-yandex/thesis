#include <QDebug>
#include "algorithm.hh"

uint Algorithm::_mutex = 0;
Algorithm::Create 			Algorithm::_create;
Algorithm::Clear 			Algorithm::_clear;
Algorithm::Build 			Algorithm::_build;
Algorithm::GetParams 		Algorithm::_getParams;
Algorithm::GetDefaultValues Algorithm::_getDefaultValues;
Algorithm::GetParamNames 	Algorithm::_getParamNames;

Algorithm::Algorithm(QString image, const VectorDouble &vector)
{
    CImageUnsigned im(image.toStdString().c_str());
    _data = _create(&im, &vector);
    _mutex++;
}

Algorithm::Algorithm(CImageUnsigned& image, const VectorDouble &vector)
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

void Algorithm::computeDescriptors(DescriptorArray& ptr, KeypointList &points)
{
    _build(_data, &ptr, &points);
}

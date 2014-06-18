#include <QDebug>
#include "algorithm.hh"

uint Algorithm::_mutex = 0;
Algorithm::Create Algorithm::_create;
Algorithm::Clear Algorithm::_clear;
Algorithm::Build Algorithm::_build;
Algorithm::GetParams Algorithm::_get;

Algorithm::Algorithm(QString image, const VectorDouble& vector)
    : paramSize(vector.size())
{
    CImageUnsigned im(image.toStdString());
    _data = _create(im, vector.size(), &vector[0]);
    _mutex++;
}

Algorithm::Algorithm(CImageUnsigned image, const VectorDouble& vector)
    : paramSize(vector.size())
{
    _data = _create(image, vector.size(), &vector[0]);
    _mutex++;
}

Algorithm::~Algorithm()
{
    _clear(_data);
    _mutex--;
}

double Algorithm::param(uint i) const
{
    VectorDouble p(paramSize);
    _get(_data, paramSize, &p[0]);

    return p[i];
}

void Algorithm::computeDescriptors(DescriptorArray& ptr, KeypointList &points)
{
    _build(_data, &ptr, &points);
}

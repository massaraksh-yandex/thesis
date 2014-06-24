#include <QStringList>
#include "tree.hh"
#include <cmath>

Tree::Create  Tree::_create;
Tree::Clear   Tree::_clear;
Tree::Insert  Tree::_insert;
Tree::Nearest Tree::_nearest;
Tree::GetDefaultValues Tree::_getDefaultValues;
Tree::GetParamNames Tree::_getParamNames;

uint Tree::_mutex = 0;


Tree::Tree(uint size)
{
    _data = _create(size);
    _mutex++;
}

Tree::~Tree()
{
    _clear(_data);
    _mutex--;
}

void Tree::push(Descriptor& vector)
{
    _insert(_data, &vector);
}

bool Tree::compareWith(const Descriptor &d, Keypoint& point)
{
    Keypoint* points[2];

    int resSize = _nearest(_data, &d, points[0], points[1]);
    if(resSize < 2)
        return false;

    double first = 0.0, second = 0.0;
    for(int h = 0; h < d.size(); h++)
    {
        first +=  std::pow((*points[0]->descriptor)[h], 2.0);
        second += std::pow((*points[1]->descriptor)[h], 2.0);
    }

    if(second / first <= defaultValues()[0]) {
        point = *points[0];
        return true;
    }
    else
        return false;
}

QStringList Tree::paramNames()
{
    QStringList p;
    _getParamNames(&p);

    return p;
}

VectorDouble Tree::defaultValues()
{
    VectorDouble p;
    _getDefaultValues(&p);

    return p;
}

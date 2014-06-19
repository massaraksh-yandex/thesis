#include <QStringList>
#include "tree.hh"

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

bool Tree::compareWith(Descriptor descriptor)
{
    Descriptor* descriptors[2];

    int resSize = _nearest(_data, &descriptor, descriptors[0], descriptors[1]);
    if(resSize < 2)
        return false;

    double first = 0.0, second = 0.0;
    for(int h = 0; h < descriptor.size(); h++)
    {
        first +=  (*descriptors[0])[h] * (*descriptors[0])[h];
        second += (*descriptors[1])[h] * (*descriptors[1])[h];
    }

    if(second / first <= defaultValues()[0])
        return true;
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

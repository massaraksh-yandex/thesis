#include "kdtree.h"
#include "tree.hh"


Tree::Tree(uint size, QObject *parent)
: QObject(parent)
{
    _data = kd_create(size);
}

Tree::~Tree()
{
    kd_clear(_data);
    delete _data;
    deleteLater();
}

void Tree::push(Descriptor vector, int realIndex)
{
    int* data = new int(realIndex);
    kd_insert(_data, &vector[0], data);
}

bool Tree::compareWith(Descriptor descriptor, double threshold)
{
    kdres* kdRes = kd_nearest(_data, &descriptor[0]);
    Descriptor* descriptors[2];
    int* datas[] = {0, 0};

    int resSize = kd_res_size(kdRes);
    if(resSize >= 2)
    {
        datas[0] = (int*)kd_res_item(kdRes, &(*descriptors[0])[0]);
        kd_res_next(kdRes);
        datas[1] = (int*)kd_res_item(kdRes, &(*descriptors[1])[0]);
    }
    else
    {
        kd_res_free(kdRes);
        return false;
    }

    double first = 0.0, second = 0.0;
    for(int h = 0; h < descriptor.size(); h++)
    {
        first +=  (*descriptors[0])[h] * (*descriptors[0])[h];
        second += (*descriptors[1])[h] * (*descriptors[1])[h];
    }

    kd_res_free(kdRes);

    if(second / first <= threshold)
        return true;
    else
        return false;
}

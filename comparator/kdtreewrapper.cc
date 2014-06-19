#include <QStringList>
#include "kdtree.h"
#include "kdtreewrapper.hh"
#include <stdexcept>

void *create(unsigned size)
{
    kdtree* tree = kd_create(size);
    return tree;
}

void clear(void* tr)
{
    if(!tr)
        throw std::invalid_argument("pointer cannot be null");

    kdtree* tree = (kdtree*)(tr);
    kd_clear(tree);
}

void insert(void* tr, const double* data)
{
    if(!tr || !data)
        throw std::invalid_argument("pointer cannot be null");

    kdtree* tree = (kdtree*)(tr);
    kd_insert(tree, data, 0);
}

int nearest(void* tr, const Descriptor *descriptor, Descriptor *firstNearest, Descriptor *secondNearest)
{
    if(!tr || !descriptor || !firstNearest || !secondNearest)
        throw std::invalid_argument("pointer cannot be null");

    kdtree* tree = (kdtree*)(tr);
    kdres* res = kd_nearest(tree, &(*descriptor)[0]);
    int size = kd_res_size(res);

    if(size >= 2)
    {
        kd_res_item(res, &(*firstNearest)[0]);
        kd_res_next(res);
        kd_res_item(res, &(*secondNearest)[0]);
    }
    kd_res_free(res);

    return size;
}

void info(LibraryInfo *info)
{
    if(!info)
        throw std::invalid_argument("pointer cannot be null");

    info->info = QObject::tr("Библиотека libkdtree для обработки дескрипторов");
    info->type = LibComparator;
}

void getLibraryAPIVersion(QString* version)
{
    if(!version)
        throw std::invalid_argument("pointer cannot be null");

    *version = LibraryAPIVersion();
}

void getDefaultValues(VectorDouble* params)
{
    if(!params)
        throw std::invalid_argument("pointer cannot be null");

    params->clear();
    params->push_back(1.5);
}

void getParamNames(QStringList* params)
{
    if(!params)
        throw std::invalid_argument("pointer cannot be null");

    params->clear();
    params->push_back("Nearest descriptors rate");
}


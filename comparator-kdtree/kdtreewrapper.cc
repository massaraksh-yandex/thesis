#include <QStringList>
#include "kdtree.h"
#include "kdtreewrapper.hh"
#include <stdexcept>
#include "global_defines.hh"
#include "macros.hh"

void *create(unsigned size)
{
    kdtree* tree = kd_create(size);
    return tree;
}

void clear(void* tr)
{
    ASERT_NOT_NULL(tr);

    kdtree* tree = (kdtree*)(tr);
    kd_clear(tree);
}

void insert(void* tr, Keypoint *attach)
{
    ASERT_NOT_NULL(tr);
    ASERT_NOT_NULL(attach);

    kdtree* tree = (kdtree*)(tr);
    kd_insert(tree, &((*attach->descriptor)[0]), attach);
}

int nearest(void* tr, const Descriptor *descriptor, Keypoint *firstNearest, Keypoint *secondNearest)
{
    ASERT_NOT_NULL(tr);
    ASERT_NOT_NULL(descriptor);
    ASERT_NOT_NULL(firstNearest);
    ASERT_NOT_NULL(secondNearest);

    kdtree* tree = (kdtree*)(tr);
    kdres* res = kd_nearest(tree, &(*descriptor)[0]);
    int size = kd_res_size(res);

    if(size >= 2) {
        double* fItem = 0;
        double* sItem = 0;
        firstNearest = (Keypoint*)kd_res_item(res, fItem);
        kd_res_next(res);
        secondNearest = (Keypoint*)kd_res_item(res, sItem);
    }
    else {
        firstNearest = 0;
        secondNearest = 0;
    }

    kd_res_free(res);

    return size;
}

void getLibraryAPIVersion(QString* version)
{
    ASERT_NOT_NULL(version);

    *version = LibraryAPIVersion();
}

void getDefaultValues(VectorDouble* params)
{
    ASERT_NOT_NULL(params);

    params->clear();
    params->push_back(1.5);
}

void getParamNames(QStringList* params)
{
    ASERT_NOT_NULL(params);

    params->clear();
    params->push_back("Nearest descriptors rate");
}

class SingleInitKdTreeInfo : public SingleInit<LibraryInfo>
{
public:
    SingleInitKdTreeInfo() : SingleInit<LibraryInfo>() {
        _instance.info = QObject::tr("Библиотека libkdtree для обработки дескрипторов");
        _instance.type = Comparator;
    }
};

class SingleInitKdTreeApi : public SingleInit<ApiTree>
{
public:
    SingleInitKdTreeApi() : SingleInit<ApiTree>() {
        _instance._clear = &clear;
        _instance.create = &create;
        _instance._getDefaultValues = &getDefaultValues;
        _instance._getParamNames = &getParamNames;
        _instance.insert = &insert;
        _instance.nearest = &nearest;
        _instance.version = API_VERSION;
    }
};

const LibraryInfo *info()
{
    static SingleInitKdTreeInfo si;
    return si.instance();
}

const ApiTree *getApi()
{
    static SingleInitKdTreeApi si;
    return si.instance();
}

#ifndef ALGORITHM_HH
#define ALGORITHM_HH

#include <QMutex>

#include "global_core.hh"

class LibraryLoader;
class Algorithm
{
    friend class LibraryLoader;

    typedef void* (*Create)(CImageUnsigned* image, const VectorDouble*);
    typedef void  (*Clear) (void*);
    typedef void  (*Build) (void*, DescriptorArray*, KeypointList*);
    typedef void  (*GetParams)(void*, VectorDouble*);
    typedef void  (*GetDefaultValues)(VectorDouble*);
    typedef void  (*GetParamNames)(QStringList*);

    static Create _create;
    static Clear _clear;
    static Build _build;
    static GetParams _getParams;
    static GetDefaultValues _getDefaultValues;
    static GetParamNames _getParamNames;

    static uint _mutex;

    void* _data;

public:
    Algorithm(CImageUnsigned &image, const VectorDouble& vector);
    Algorithm(QString image, const VectorDouble &vector);
    ~Algorithm();

    double param(uint i) const;
    void computeDescriptors(DescriptorArray &ptr, KeypointList &points);

    static QString paramName(uint i);

    static VectorDouble defaultValues();
    static bool haveInstanses() { return _mutex != 0; }
};

#endif // ALGORITHM_HH

#ifndef ALGORITHM_HH
#define ALGORITHM_HH

#include <QMutex>

#include "global_core.hh"

class LibraryLoader;
class Algorithm
{
public:
    typedef QVector<double> VectorDouble;
    typedef QMap<QString, double> ParamMap;

private:
    friend class LibraryLoader;

    typedef void* (*Create)(CImagePtr image, int, double*);
    typedef void  (*Clear) (void*);
    typedef void  (*Build) (void*, DescriptorArray*, KeypointList*);
    typedef void  (*GetParams)(void*, int, double*);

    static Create _create;
    static Clear _clear;
    static Build _build;
    static GetParams _get;

    static uint _mutex;

    void* _data;
    int paramSize;

public:
    Algorithm(CImageUnsigned image, const VectorDouble& vector);
    Algorithm(QString image, const VectorDouble& vector);
    ~Algorithm();

    double param(uint i) const;
    ParamMap params() const;

    void computeDescriptors(DescriptorArray &ptr, KeypointList &points);

    static bool haveInstanses() { return _mutex != 0; }
};

#endif // ALGORITHM_HH

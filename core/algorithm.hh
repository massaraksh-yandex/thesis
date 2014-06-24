#ifndef ALGORITHM_HH
#define ALGORITHM_HH

#include <QFutureWatcher>

#include "global_core.hh"

class LibraryLoader;
class Algorithm : QObject
{
    Q_OBJECT

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
    DescriptorArray _descriptor;
    KeypointList _keypointList;

    QFutureWatcher<void> _watchDog;

    bool _async;

public:
    Algorithm(CImageUnsigned &image, const VectorDouble& vector, bool async = true, QObject* parent = 0);
    Algorithm(QString image, const VectorDouble &vector, bool async = true, QObject* parent = 0);
    ~Algorithm();

    DescriptorArray& descriptor()   { return _descriptor; }
    KeypointList&    keypointList() { return _keypointList; }
    double param(uint i) const;

    static QString paramName(uint i);

    static VectorDouble defaultValues();
    static bool haveInstanses() { return _mutex != 0; }

public slots:
    void execAsync(int);
    void execute();
    void tryCancel();

signals:
    void finished();

private slots:
    void computingFinished();
};

typedef QSharedPointer<Algorithm> AlgorithmPtr;

#endif // ALGORITHM_HH

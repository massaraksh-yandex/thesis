#ifndef KDTREEWRAPPER_HH
#define KDTREEWRAPPER_HH

#include <QString>
#include <QMap>
#include "comparator_global.hh"
#include "global_defines.hh"

extern "C"
{
    void COMPARATORSHARED_EXPORT *create (unsigned size);
    void COMPARATORSHARED_EXPORT  clear  (void* tree);
    void COMPARATORSHARED_EXPORT  insert (void* tree, const double *data);
    int COMPARATORSHARED_EXPORT   nearest(void*, double* descriptor,
                                          double* firstNearest, double* secondNearest);

    void COMPARATORSHARED_EXPORT info(LibraryInfo* type);
}
#endif // KDTREEWRAPPER_HH

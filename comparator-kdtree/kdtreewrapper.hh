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
    void COMPARATORSHARED_EXPORT  insert (void* tree, Keypoint* attach);
    int  COMPARATORSHARED_EXPORT  nearest(void*, const Descriptor *descriptor,
                                          Keypoint* firstNearest, Keypoint* secondNearest);

    void COMPARATORSHARED_EXPORT  getDefaultValues(VectorDouble* params);
    void COMPARATORSHARED_EXPORT  getParamNames(QStringList* params);
    void COMPARATORSHARED_EXPORT info(LibraryInfo* type);
    void COMPARATORSHARED_EXPORT getLibraryAPIVersion(QString* version);

}
#endif // KDTREEWRAPPER_HH

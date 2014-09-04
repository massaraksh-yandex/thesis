#ifndef KDTREEWRAPPER_HH
#define KDTREEWRAPPER_HH

#include "api.hh"
#include "comparator_global.hh"

extern "C"
{
    const ApiTree* COMPARATORSHARED_EXPORT getApi();
    const LibraryInfo* COMPARATORSHARED_EXPORT info();
}
#endif // KDTREEWRAPPER_HH

#ifndef SIFT_GLOBAL_HH
#define SIFT_GLOBAL_HH

#include <QtCore/qglobal.h>

#if defined(SIFT_LIBRARY)
#  define SIFTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define SIFTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // SIFT_GLOBAL_HH

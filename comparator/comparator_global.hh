#ifndef COMPARATOR_GLOBAL_HH
#define COMPARATOR_GLOBAL_HH

#include <QtCore/qglobal.h>

#if defined(COMPARATOR_LIBRARY)
#  define COMPARATORSHARED_EXPORT Q_DECL_EXPORT
#else
#  define COMPARATORSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // COMPARATOR_GLOBAL_HH

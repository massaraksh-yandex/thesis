#ifndef GLOBAL_HH
#define GLOBAL_HH

#include <QtCore/qglobal.h>
#include <QSharedPointer>
#include <QList>

#include "global_defines.hh"
#include "../CImg.h"

#include <boost/numeric/ublas/matrix.hpp>

typedef QList<cimg_library::CImg<float> > CImageVec;
typedef QList<CImageVec> CImageDoG;
typedef boost::numeric::ublas::matrix<double> Matrix;


#if defined(SIFT_LIBRARY)
#  define SIFT_EXPORT Q_DECL_EXPORT
#else
#  define SIFT_EXPORT Q_DECL_IMPORT
#endif

#endif // GLOBAL_HH

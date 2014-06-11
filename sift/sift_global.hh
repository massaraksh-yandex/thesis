#ifndef GLOBAL_HH
#define GLOBAL_HH

#include "../global_defines.hh"
#include "boost/numeric/ublas/matrix.hpp"
#include <vector>
#include <QList>
#include <QSharedPointer>
#include "../CImg.h"

typedef std::vector<cimg_library::CImg<float> > CImageVec;
typedef std::vector<CImageVec> CImageDoG;
typedef boost::numeric::ublas::matrix<double> Matrix;

#endif // GLOBAL_HH

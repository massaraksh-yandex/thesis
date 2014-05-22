#ifndef GLOBAL_HH
#define GLOBAL_HH

#include <boost/numeric/ublas/matrix.hpp>
#include <vector>
#include <QList>
#include "../CImg.h"

typedef cimg_library::CImg<float> CImage;
typedef std::vector<cimg_library::CImg<float> > CImageVec;
typedef std::vector<std::vector<cimg_library::CImg<float> > > CImageDoG;
typedef boost::numeric::ublas::matrix<double> Matrix;
typedef QList<QList<double> > Descriptor;

#endif // GLOBAL_HH

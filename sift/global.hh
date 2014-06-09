#ifndef GLOBAL_HH
#define GLOBAL_HH

#include <boost/numeric/ublas/matrix.hpp>
#include <vector>
#include <list>
#include <QList>
#include <QSharedPointer>
#include "../CImg.h"

typedef cimg_library::CImg<float> CImage;
typedef QSharedPointer<CImage> CImagePtr;
typedef std::vector<cimg_library::CImg<float> > CImageVec;
typedef std::vector<std::vector<cimg_library::CImg<float> > > CImageDoG;
typedef boost::numeric::ublas::matrix<double> Matrix;
typedef QList<QVector<double> > Descriptor;
typedef QSharedPointer<Descriptor> DescriptorPtr;

#endif // GLOBAL_HH

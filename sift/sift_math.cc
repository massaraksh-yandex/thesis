#include <boost/numeric/ublas/lu.hpp>

#include "sift_math.hh"

using namespace boost::numeric::ublas;

namespace Math {
int determinant_sign(const boost::numeric::ublas::permutation_matrix<std::size_t> &pm)
{	// This was taken from http://www.anderswallin.net/2010/05/matrix-determinant-with-boostublas/
    int pm_sign = 1;
    size_t size = pm.size();
    for(size_t i = 0; i < size; ++i)
        if (i != pm(i))
            pm_sign *= -1; // swap_rows would swap a pair of rows here, so we change sign
    return pm_sign;
}

double Gaussian2D(double dx, double dy, double sigma)	// could be optimized byt not computing the Gaussian all over again
{	// dx - distance from the center on x-axis; dy - analogy to the dx
    return 1.0 / (2*PI()*sigma*sigma) * exp(-(dx*dx+dy*dy)/(2.0*sigma*sigma));
}

// compute a partial derivative of a 3D function
void diff3D(CImageVec &space, int x, int y, int z, Matrix &diff)
{
    diff(0,0) = double(space[z  ](x+1,y  ) - space[z  ](x-1,y  )) / 2.0;
    diff(1,0) = double(space[z  ](x  ,y+1) - space[z  ](x  ,y-1)) / 2.0;
    diff(2,0) = double(space[z+1](x  ,y  ) - space[z-1](x  ,y  )) / 2.0;
}

// put together a 3x3 Hessian matrix
void hessian3x3(const CImageVec &space, int x, int y, int z, Matrix &h_mat)
{
    //     (Dxx, Dxy, Dxz)
    // H = (Dxy, Dyy, Dyz)
    //     (Dxz, Dyz, Dzz)
    // ===================
    h_mat(0,0) = space[z  ](x+1,y  ) + space[z  ](x-1,y  ) - 2 * space[z](x,y);	// Dxx
    h_mat(1,1) = space[z  ](x  ,y+1) + space[z  ](x  ,y-1) - 2 * space[z](x,y);	// Dyy
    h_mat(2,2) = space[z+1](x  ,y  ) + space[z-1](x  ,y  ) - 2 * space[z](x,y);	// Dzz
    //
    h_mat(1,0) = h_mat(0,1) = space[z  ](x+1,y+1) + space[z  ](x-1,y-1) - space[z  ](x+1,y-1) - space[z  ](x-1,y+1);	// Dxy
    h_mat(2,0) = h_mat(0,2) = space[z+1](x+1,y  ) + space[z-1](x-1,y  ) - space[z-1](x+1,y  ) - space[z+1](x-1,y  );	// Dxz
    h_mat(2,1) = h_mat(1,2) = space[z+1](x  ,y+1) + space[z-1](x  ,y-1) - space[z-1](x  ,y+1) - space[z+1](x  ,y-1);	// Dyz
}

// put together a 2x2 Hessian matrix
void hessian2x2(const CImage &space, int x, int y, Matrix &h_mat)
{
    //     (Dxx, Dxy)
    // H = (Dxy, Dyy)
    // ==============
    h_mat(0,0) = space(x+1,y  ) + space(x-1,y  ) - 2 * space(x,y);	// Dxx
    h_mat(1,1) = space(x  ,y+1) + space(x  ,y-1) - 2 * space(x,y);	// Dyy
    //
    h_mat(1,0) = h_mat(0,1) = space(x+1,y+1) + space(x-1,y-1) - space(x+1,y-1) - space(x-1,y+1);	// Dxy
}

bool inverse(Matrix &input, Matrix &inverse)
{	// This was taken from http://savingyoutime.wordpress.com/2009/09/21/c-matrix-inversion-boostublas/
    // create a working copy of the input
    Matrix mat(input);

    // create a permutation matrix for the LU-factorization
    permutation_matrix<size_t> pm(mat.size1());

    // perform LU-factorization
    int res = lu_factorize(mat, pm);
    if (res != 0) return false;

    // create identity matrix of "inverse"
    inverse.assign(identity_matrix<double> (mat.size1()));

    // backsubstitute to get the inverse
    lu_substitute(mat, pm, inverse);

    return true;
}

double determinant(Matrix &m)
{
    double det;
    if((m.size1() == 2) && (m.size2() == 2))
    {	// cross-rule
        det = m(0,0) * m(1,1) - m(0,1) * m(1,0);
    }
    else
    {	// This was taken from http://www.anderswallin.net/2010/05/matrix-determinant-with-boostublas/
        boost::numeric::ublas::permutation_matrix<std::size_t> pm(m.size1());
        det = 1.0;
        if(boost::numeric::ublas::lu_factorize(m,pm))
            det = 0.0;
        else
        {
            for(int i = 0, im = m.size1(); i < im; i++)
                det *= m(i,i); // multiply by elements on diagonal
            det = det * determinant_sign(pm);
        }
    }
    return det;
}

double sigma(int x, int y)
{
    static double sigmas[4][5] = 	// 4-octaves,5-blurs
    {	// 1.radek jsem rozepsal, aby bylo videt, kde se ty cisla v dalsich radcich vzaly (ale i ty jsou trochu rozepsany, aby byl videt nasobek mocniny dvou)
        //{ 1.0*base*0.5, 1.0*base*base*0.5, 1.0*base*base*base*0.5, 1.0*base*base*base*base*0.5, 1.0*base*base*base*base*base*0.5 },
        { 1.0*base()*0.5, 1.0*1.0, 1.0*base(), 1.0*2.0, 1.0*base()*2.0 },
        { 2.0*base()*0.5, 2.0*1.0, 2.0*base(), 2.0*2.0, 2.0*base()*2.0 },
        { 4.0*base()*0.5, 4.0*1.0, 4.0*base(), 4.0*2.0, 4.0*base()*2.0 },
        { 8.0*base()*0.5, 8.0*1.0, 8.0*base(), 8.0*2.0, 8.0*base()*2.0 }
    };

    return sigmas[x][y];
}

}

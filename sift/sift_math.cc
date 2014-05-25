#include <boost/numeric/ublas/lu.hpp>

#include "sift_math.hh"
#include "keypoint.hh"

using namespace boost::numeric::ublas;

namespace Math {
double Gaussian2D(double x, double y, double s)
{
    double pi = (2*PI()*s*s);
    double ex = exp(-(x*x+y*y)/(2.0*s*s));
    return 1.0/pi*ex;
}

// частичная 3д функция
void diff3D(CImageVec &space, const Keypoint& kp, Matrix &diff)
{
    int x = kp.X;
    int y = kp.Y;
    int z = kp.Bl;
    diff(0,0) = double(space[z  ](x+1,y  ) - space[z  ](x-1,y  )) / 2.0;
    diff(1,0) = double(space[z  ](x  ,y+1) - space[z  ](x  ,y-1)) / 2.0;
    diff(2,0) = double(space[z+1](x  ,y  ) - space[z-1](x  ,y  )) / 2.0;
}

void H3x3(const CImageVec &space, Keypoint& kp, Matrix &our)
{
    // xx xy xz
    // xy yy yz
    // xz yz zz
    int x = kp.X;
    int y = kp.Y;
    int z = kp.Bl;

    our(0,0) = space[z  ](x+1,y  ) + space[z  ](x-1,y  ) - 2 * space[z](x,y); // xx
    our(1,1) = space[z  ](x  ,y+1) + space[z  ](x  ,y-1) - 2 * space[z](x,y); // yy
    our(2,2) = space[z+1](x  ,y  ) + space[z-1](x  ,y  ) - 2 * space[z](x,y); // zz

    our(1,0) = our(0,1)
               = space[z](x+1,y+1)+space[z](x-1,y-1)-space[z](x+1,y-1)-space[z](x-1,y+1); // xy
    our(2,0) = our(0,2)
               = space[z+1](x+1,y)+space[z-1](x-1,y)-space[z-1](x+1,y)-space[z+1](x-1,y); // xz
    our(2,1) = our(1,2)
               = space[z+1](x,y+1)+space[z-1](x,y-1)-space[z-1](x,y+1)-space[z+1](x,y-1); // yz
}

void H2x2(const CImage &space, int x, int y, Matrix &out)
{
    // xx xy
    // xy yy
    out(0,0) = space(x+1,y  ) + space(x-1,y  ) - 2 * space(x,y); // xx
    out(1,1) = space(x  ,y+1) + space(x  ,y-1) - 2 * space(x,y); // yy
    //
    out(1,0) = out(0,1)
            = space(x+1,y+1) + space(x-1,y-1) - space(x+1,y-1) - space(x-1,y+1); // xy
}

bool inverse(Matrix &input, Matrix &inverse)
{
    Matrix mat(input);

    // фигачим перестановки
    permutation_matrix<int> pm(mat.size1());

    // LU
    if (lu_factorize(mat, pm))
        return false;

    inverse.assign(identity_matrix<double> (mat.size1()));

    lu_substitute(mat, pm, inverse);

    return true;
}

int detSign(const permutation_matrix<int> &pm)
{
    int sign = 1;
    for(int i = 0; i < pm.size(); ++i)
        if (i != pm(i))
            sign *= -1;
    return sign;
}

double det(Matrix &m)
{
    if((m.size1() == 2) && (m.size2() == 2))
    {
        return m(0,0)*m(1,1)-m(0,1)*m(1,0);
    }
    else
    {
        permutation_matrix<int> pm(m.size1());
        if(lu_factorize(m,pm))
        {
            return 0.0;
        }
        else
        {
            double det = 1.0;
            for(int i = 0; i < m.size1(); i++)
                det *= m(i,i);
            return det * detSign(pm);
        }
    }
}

double sigma(int x, int y)
{
    static double sigmas[4][5] =
    {
        { 1.0*base()*0.5, 1.0*1.0, 1.0*base(), 1.0*2.0, 1.0*base()*2.0 },
        { 2.0*base()*0.5, 2.0*1.0, 2.0*base(), 2.0*2.0, 2.0*base()*2.0 },
        { 4.0*base()*0.5, 4.0*1.0, 4.0*base(), 4.0*2.0, 4.0*base()*2.0 },
        { 8.0*base()*0.5, 8.0*1.0, 8.0*base(), 8.0*2.0, 8.0*base()*2.0 }
    };

    return sigmas[x][y];
}

}

#ifndef _MATH_H_
#define _MATH_H_

#include <vector>
#include <map>

#include "global.hh"

namespace Math
{
    inline double PI() { return 3.141592653589793238462643383; }
    inline constexpr double base() { return sqrt(2.0); }

    double sigma(int x, int y);
    double Gaussian2D(double dx, double dy, double sigma);
    void diff3D(CImageVec &space, int x, int y, int z, Matrix &diff);
    void hessian3x3(const CImageVec &space, int x, int y, int z, Matrix &h_mat);
    void hessian2x2(const CImage &space, int x, int y, Matrix &h_mat);
    bool inverse(Matrix &input, Matrix &inverse);
    double determinant(Matrix &m);
}

#endif	// _MATH_H_

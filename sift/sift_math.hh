#ifndef _MATH_H_
#define _MATH_H_

#include <vector>
#include <map>

#include "global.hh"
#include "keypoint.hh"

namespace Math
{
    inline constexpr double PI() { return 3.141592653589793238462643383; }
    inline constexpr double base() { return sqrt(2.0); }
    inline constexpr double epsilon() { return 0.001; }

    double sigma(int x);
    double Gaussian(double x, double y, double sigma);
    double det(Matrix &m);

    int kernelSize(double sigma);

    void diff3(CImageVec &space, const Keypoint& kp, Matrix &diff);
    void H3x3(const CImageVec &space, Keypoint& kp, Matrix &our);
    void H2x2(const CImage &space, int x, int y, Matrix &out);

    bool inverse(Matrix &input, Matrix &inverse);
}

#endif	// _MATH_H_

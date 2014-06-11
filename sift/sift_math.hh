#ifndef SIFT_MATH_H_
#define SIFT_MATH_H_

#include <vector>
#include <map>

#include "sift_global.hh"
#include "keypoint.hh"

namespace Math
{
    inline constexpr double PI() { return 3.141592653589793238462643383; }
    inline constexpr double base() { return sqrt(2.0); }
    inline constexpr double epsilon() { return 0.001; }

    double Sigma(int x);
    double Gaussian(double x, double y, double Sigma);
    double Det(Matrix &m);

    int kernelSize(double Sigma);

    void Diff3(CImageVec &space, const Keypoint& kp, Matrix &diff);
    void H3x3(const CImageVec &space, Keypoint& kp, Matrix &our);
    void H2x2(const CImage &space, int x, int y, Matrix &out);

    bool MakeInverse(Matrix &input, Matrix &inverse);
}

#endif	// _MATH_H_

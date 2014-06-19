#ifndef SIFTKEYPOINT_HH
#define SIFTKEYPOINT_HH

#include "sift_global.hh"

struct SiftKeypoint
{
    typedef QList<SiftKeypoint> Cont;
    typedef std::vector<std::pair<double,double> > Histogram;
    typedef Cont::iterator iterator;

    typedef std::vector<double> VectorDouble;
    typedef std::vector<VectorDouble> MatrixDouble;
    typedef std::vector<MatrixDouble> D3Histogram;

    double X; // колонка
    double Y; // ряд
    double Bl; // блур

    // смещение
    double dx;
    double dy;
    double dz;

    int octave;

    // градиент
    double magnitude;
    double angle;

    Histogram angmag; // гистограмма

    SiftKeypoint() {}

    SiftKeypoint(double _x, double _y, double _z, int _o) :
        X(_x), Y(_y), Bl(_z), octave(_o)
    { }

    Cont neighbourhood;
};


#endif // SIFTKEYPOINT_HH

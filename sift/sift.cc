#include <QDebug>
#include "sift.hh"

#include "sift_math.hh"

#include <cmath>
#include <vector>
#include <string>

using namespace std;


bool Sift::minimumInLayer(const CImage &img, float pix, int x, int y, bool dontCheckXY)
{
    if(img(x-1, y-1) >= pix)
        return false;
    else if(img(x-1, y) >= pix)
        return false;
    else if(img(x-1, y+1) >= pix)
        return false;
    else if(img(x, y-1) >= pix)
        return false;
    else if(img(x, y) >= pix){
        if(!dontCheckXY)
            return false;
    }

    if(img(x, y+1) >= pix)
        return false;
    else if(img(x+1, y-1) >= pix)
        return false;
    else if(img(x+1, y) >= pix)
        return false;
    else if(img(x+1, y+1) >= pix)
        return false;
    else
        return true;
}

bool Sift::maximumInLayer(const CImage &img, float pix, int x, int y, bool dontCheckXY)
{
    if(img(x-1, y-1) <= pix)
        return false;
    else if(img(x-1, y) <= pix)
        return false;
    else if(img(x-1, y+1) <= pix)
        return false;
    else if(img(x, y-1) <= pix)
        return false;
    else if(img(x, y) <= pix){
        if(!dontCheckXY)
            return false;
    }

    if(img(x, y+1) <= pix)
        return false;
    else if(img(x+1, y-1) <= pix)
        return false;
    else if(img(x+1, y) <= pix)
        return false;
    else if(img(x+1, y+1) <= pix)
        return false;
    else
        return true;
}

void Sift::subpixelExtrema(CImageVec &octave, Keypoint &keypoint)
{
    boost::numeric::ublas::matrix<double> D(3,1),
                                          H(3,3),
                                          HInverce(3,3);

    Math::diff3(octave, keypoint, D);
    Math::H3x3(octave, keypoint, H);

    if(!Math::inverse(H, HInverce))
        return;

    // апроксимация Х - субпиксельная позиция
    boost::numeric::ublas::matrix<double> shift(3,1);
    shift = boost::numeric::ublas::prod(HInverce, D);

    keypoint.dx = -shift(0,0);
    keypoint.dy = -shift(1,0);
    keypoint.dz = -shift(2,0);
}

CImage Sift::normalize(const cimg_library::CImg<unsigned char> &img)
{
    CImage out(img);
    for(int y = 0; y < img.height(); y++)
        for(int x = 0; x < img.width(); x++)
            out(x,y) = float(img(x,y)) / 255.0f;

    return out;
}

Sift::Sift(QObject *obj = 0) : QObject(obj), CONTRAST(0.03), CORNER(10.0)
{
}

Sift::Sift(QString fileName, QObject* obj = 0) : QObject(obj), CONTRAST(0.03), CORNER(10.0)
{
    img.load(fileName.toStdString().c_str());
}

Sift::Sift(CImagePtr image, QObject* obj) : QObject(obj), CONTRAST(0.03), CORNER(10.0), img(*image)
{ }

void Sift::load(QString str)
{
    _data.dog.clear();
    _data.points.clear();
    img.load(str.toStdString().c_str());
}

void Sift::load(CImagePtr image)
{
    _data.dog.clear();
    _data.points.clear();
    img = *image;
}

void Sift::buildPyramidAndDoG()
{
    CImageDoG pyramid;

    QVector<CImage> images; images.reserve(4);

    images.append(normalize(img).blur(0.5).get_resize(-200, -200, -100, -100, 1).blur(1.0));
    images.append(images[0].get_resize( -50,  -50, -100, -100, 1));
    images.append(images[1].get_resize( -50,  -50, -100, -100, 1));
    images.append(images[2].get_resize( -50,  -50, -100, -100, 1));

    for(int i = 0; i < images.size(); i++)
    {
        pyramid.push_back(CImageVec());
        pyramid[i].push_back(images[i]);
        double sigma = Math::base();
        for(int s = 1; s < 5; s++)
        {
            pyramid[i].push_back(pyramid[i][s-1].get_blur(sigma));
            sigma *= Math::base();
        }
    }
    for(int i = 0; i < pyramid.size(); i++)
    {
        _data.dog.push_back(CImageVec());
        for(int s = 1; s < pyramid[i].size(); s++)
            _data.dog[i].push_back(pyramid[i][s-1] - pyramid[i][s]);
    }
}


int Sift::computeKeypoints()
{
    for(size_t i = 0; i < _data.dog.size(); i++)
    {
        for(size_t s = 1; s < (_data.dog[i].size()-1); s++)
        {
            for(int y = 1; y < (_data.dog[i][s].height()-1); y++)
            {
                for(int x = 1; x < (_data.dog[i][s].width()-1); x++)
                {
                    float px = _data.dog[i][s](x,y);

                    bool max = true;
                    bool min = true;
                    for(int k = -1; k < 2; k++)
                        min = min && minimumInLayer(_data.dog[i][s+k], px, x, y, k == 0 );

                    for(int k = -1; k < 2; k++)
                        max = max && maximumInLayer(_data.dog[i][s+k], px, x, y, k == 0 );

                    if(min || max)
                        _data.points.push_back(Keypoint(x, y, s, i));
                }
            }
        }
    }
    return _data.points.size();
}

int Sift::clarifyKeypoints()
{
    const int MaxIters = 5;

    Keypoint::Cont nArray;
    for(Keypoint::iterator f_it = _data.points.begin(); f_it != _data.points.end(); ++f_it)
    {
        CImage& img = _data.dog[f_it->octave][f_it->Bl];
        CImageVec& oct = _data.dog[f_it->octave];

        double maxX = img.width() - 2;
        double maxY = img.height() - 2;
        double maxZ = oct.size() - 2;
        bool isStable = false;

        for(int it = 0; it < MaxIters; it++)
        {
            subpixelExtrema(oct, *f_it);

            // достаточно близко
            if((f_it->dx < 0.5) &&
               (f_it->dy < 0.5) &&
               (f_it->dz < 0.5))
            {
                isStable = true;
                break;
            }

            if(f_it->dx >= 0.5)
                f_it->X += 1;
            if(f_it->dy >= 0.5)
                f_it->Y += 1;
            if(f_it->dz >= 0.5)
                f_it->Bl += 1;
            // выход за границы
            if((f_it->X < 1)    || (f_it->Y < 1)    || (f_it->Bl < 1) ||
               (f_it->X > maxX) || (f_it->Y > maxY) || (f_it->Bl > maxZ))
            {
                isStable = false;
                break;
            }
        }
        if(isStable)
        {
            nArray.push_back(*f_it);
        }
    }
    _data.points.swap(nArray);
    return _data.points.size();
}

int Sift::filterKeypoints()
{
    // удаляем низкоконтрастные точки или точки на ребре
    boost::numeric::ublas::matrix<double> H(2,2);
    double contrast, tr, det;
    Keypoint::Cont nArray;
    for(Keypoint::iterator f_it = _data.points.begin(); f_it != _data.points.end(); f_it++)
    {
        contrast = _data.dog[f_it->octave][f_it->Bl](f_it->X,f_it->Y);
        if(qAbs(contrast) >= CONTRAST)
            nArray.push_back(*f_it);
    }
    _data.points.swap(nArray);
    nArray.clear();

    // удаляем на рёбрах
    for(Keypoint::iterator f_it = _data.points.begin(); f_it != _data.points.end(); ++f_it)
    {
        Math::H2x2(_data.dog[f_it->octave][f_it->Bl], f_it->X, f_it->Y, H);
        tr = H(0,0) + H(1,1);
        det = Math::det(H);

        double value = (tr*tr)/det;
        double corner = (CORNER+1)*(CORNER+1)/CORNER;
        if((det >= 0) && (value <= corner))
            nArray.push_back(*f_it);
    }
    _data.points.swap(nArray);

    return _data.points.size();
}

void Sift::finishKeypoints()
{
    for(Keypoint::Cont::value_type& point : _data.points)
    {
        point.neighbourhood.clear();
        double sigma = Math::sigma(point.octave, 0);
        int kernelSize = Math::kernelSize(sigma);

        for(int i = 0; i < kernelSize; i++)
        {
            for(int j = 0; j < kernelSize; j++)
            {
                int x = point.X - (kernelSize/2) + j;
                int y = point.Y - (kernelSize/2) + i;

                CImage& img = _data.dog[point.octave][point.Bl];

                bool correct = (x <= 0) || (y <= 0) ||
                        (x >= img.width()-1) || (y >= img.height()-1);
                if(!correct)
                    point.neighbourhood.push_back(Keypoint(x, y, point.Bl, point.octave));
            }
        }

        // считаем магнитуды и углы для точек окна
        for(Keypoint::Cont::value_type& neibour : point.neighbourhood)
        {
            CImage& img = _data.dog[neibour.octave][neibour.Bl];

            neibour.magnitude = sqrt(pow(img(neibour.X+1,neibour.Y)   - img(neibour.X-1,neibour.Y),   2.0f)
                                   + pow(img(neibour.X,  neibour.Y+1) - img(neibour.X,  neibour.Y-1), 2.0f));

            neibour.angle = atan2((img(neibour.X,  neibour.Y+1) - img(neibour.X,  neibour.Y-1)),
                                  (img(neibour.X+1,neibour.Y)   - img(neibour.X-1,neibour.Y)));
        }

        // строим гистограмму с 36 бинами
        Keypoint::VectorDouble hist(36, 0.0);
        for(Keypoint::Cont::value_type& neibour : point.neighbourhood)
        {
            neibour.angle = 180 + (neibour.angle * 180.0 / Math::PI());	// градусы
            int indX = neibour.angle / 10;

            double x = neibour.X - point.X;
            double y = neibour.Y - point.Y;
            hist[indX] += neibour.magnitude * Math::Gaussian(x, y, 1.5*sigma);
        }

        // максимальный бин
        int maxInd = std::max_element(hist.begin()++, hist.end()) - hist.begin();
        point.angmag.push_back(std::make_pair(maxInd*10, hist[maxInd]));	// направление точки

        // дополнительные направления
        double threshold = hist[maxInd]*0.8;
        for(int i = 0, im = hist.size(); i < im; ++i)
        {
            if(hist[i] >= threshold)
                point.angmag.push_back(std::make_pair(i*10,hist[i]));	// дополнительные направления
        }
    }
}

KeypointCoords Sift::formKeypoints()
{
    buildPyramidAndDoG();
    computeKeypoints();
    clarifyKeypoints();
    filterKeypoints();
    finishKeypoints();

    KeypointCoords out;
    out.reserve(_data.points.size());

    for(Keypoint& kp: _data.points)
        out.push_back(qMakePair((int)kp.X, (int)kp.Y));

    return out;
}

DescriptorPtr Sift::computeDescriptors()
{
    DescriptorPtr d(new Descriptor());
    for(Keypoint kp : _data.points)
    {
        buildDescriptor(kp, _data.dog, *d);
    }

    return d;
}




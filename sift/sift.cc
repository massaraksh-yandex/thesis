#include <QDebug>
#include "sift.hh"

#include "sift_math.hh"

#include <cmath>
#include <vector>
#include <string>

using namespace std;

constexpr double epsilon = 0.001;

int Sift::getKernelSize(double sigma)
{
    int maxIters = 20, i;
    for (i = 0; i < maxIters; i++)
    {
        double top = i*i*-1;
        if ( exp( top / (2.0*sigma*sigma)) < epsilon)
            break;
    }

    return 2*i-1;
}

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

    Math::diff3D(octave, keypoint, D);
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

    for(int i = 0; i < 4; i++)
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
    for(Keypoint::iterator f_it = _data.points.begin(); f_it != _data.points.end(); )
    {
        contrast = _data.dog[f_it->octave][f_it->Bl](f_it->X,f_it->Y);
        if(fabs(contrast) < CONTRAST)
        {
            f_it = _data.points.erase(f_it);	// returns the following element
            continue;
        }
        // the following element
        ++f_it;
    }
    for(Keypoint::iterator f_it = _data.points.begin(); f_it != _data.points.end(); )
    {
        // 2. the edge-like points check
        Math::H2x2(_data.dog[f_it->octave][f_it->Bl], f_it->X, f_it->Y, H);
        tr = H(0,0) + H(1,1);
        det = Math::det(H);
        if((det < 0) || (((tr*tr)/det) > ((CORNER+1)*(CORNER+1)/CORNER)))
        {
            f_it = _data.points.erase(f_it);	// returns the following element
            continue;
        }
        // the following element
        ++f_it;
    }

    return _data.points.size();
}

void Sift::finishKeypoints()
{
    int kernelSize;
    double sigma;
    for(Keypoint::iterator f_it = _data.points.begin(); f_it != _data.points.end(); ++f_it)
    {
        f_it->neighbourhood.clear();
        sigma = Math::sigma(f_it->octave, 0);
        kernelSize = getKernelSize(sigma);
        // 1. vyriznout okenko o velikosti kernelu pro Gaussian 1.5*sigma
        //    -- body, u kterych by se okenko neveslo do obrazku zahodit
        //    -- sigma je zakladni hodnota sigmy pro dany scale
        for(int i = 0; i < kernelSize; i++)
        {
            for(int j = 0; j < kernelSize; j++)
            {
                Keypoint point( f_it->X - (kernelSize/2) + j,
                                f_it->Y - (kernelSize/2) + i,
                                f_it->Bl, f_it->octave);
                // kontrola rozmeru
                bool correct = (point.X <= 0) || (point.Y <= 0) ||
                                (point.X >= _data.dog[f_it->octave][f_it->Bl].width()-1) ||
                                (point.Y >= _data.dog[f_it->octave][f_it->Bl].height()-1);
                if(!correct)
                    f_it->neighbourhood.push_back(point);
            }
        }
        // 2. pro kazdy bod okenka spocitat magnitudu a uhel
        //    -- m(x,y)=sqrt(((L(x+1,y)-L(x-1,y))^2 + (L(x,y+1)-L(x,y-1))^2));
        //    -- angle(x,y)=atan((L(x,y+1)-L(x,y-1)) / (L(x+1,y)-L(x-1,y)));
        for(Keypoint::iterator n_it = f_it->neighbourhood.begin(); n_it != f_it->neighbourhood.end(); ++n_it)
        {
            n_it->magnitude = sqrt(pow(_data.dog[n_it->octave][n_it->Bl](n_it->X+1,n_it->Y) - _data.dog[n_it->octave][n_it->Bl](n_it->X-1,n_it->Y), 2.0f)
                                 + pow(_data.dog[n_it->octave][n_it->Bl](n_it->X,n_it->Y+1) - _data.dog[n_it->octave][n_it->Bl](n_it->X,n_it->Y-1), 2.0f));
            // angle is in radians
            n_it->angle = atan2((_data.dog[n_it->octave][n_it->Bl](n_it->X,n_it->Y+1) - _data.dog[n_it->octave][n_it->Bl](n_it->X,n_it->Y-1)),
                                (_data.dog[n_it->octave][n_it->Bl](n_it->X+1,n_it->Y) - _data.dog[n_it->octave][n_it->Bl](n_it->X-1,n_it->Y)));
            //n_it->angle = atan((data.dog[n_it->octave][n_it->Bl](n_it->X,n_it->Y+1) - data.dog[n_it->octave][n_it->Bl](n_it->X,n_it->Y-1)) /
            //				   (data.dog[n_it->octave][n_it->Bl](n_it->X+1,n_it->Y) - data.dog[n_it->octave][n_it->Bl](n_it->X-1,n_it->Y)));
        }
        // 3. nahazet do histogramu uhlu o 36 binech, tedy 1bin=10stupnu nebo PI/18rad
        //    -- magnituda binu se pocita jako suma pres body v binu: m(x,y)*Gauss(x,y,1.5*sigma)
        vector<double> hist(36, 0.0);
        for(Keypoint::iterator n_it = f_it->neighbourhood.begin(); n_it != f_it->neighbourhood.end(); ++n_it)
        {
            n_it->angle = 180 + (n_it->angle * 180.0 / Math::PI());	// rad2deg
            hist[int(n_it->angle) / 10] += n_it->magnitude * Math::Gaussian2D(n_it->X - f_it->X, n_it->Y - f_it->Y, 1.5*sigma);
        }
        // 4. uhel s nejvyssi magnitudou --> orientace featury! -- tato magnituda = 100%
        int max_i = 0;
        for(size_t i = 1, im = hist.size(); i < im; ++i)
        {
            if(hist[max_i] < hist[i])
                max_i = i;
        }
        f_it->angmag.push_back(std::make_pair(max_i*10, hist[max_i]));	// max_i*10 = uhel; max_i je z intervalu 0-35;nasobek 10ti to prevede na stupne 0-350
        //    -- kazda dalsi magnituda dosahujici alespon 80% --> dalsi featura! (stejny bod, jina magnituda a uhel)
        double threshold = hist[max_i] * 0.8;
        for(size_t i = 0, im = hist.size(); i < im; ++i)
        {
            if(hist[i] >= threshold)
                f_it->angmag.push_back(std::make_pair(i*10,hist[i]));	// prevod z indexu na stupne stejne jako vyse s max_i
        }
    }
}

DescriptorPtr Sift::work()
{
    DescriptorPtr d(new Descriptor());
    buildPyramidAndDoG();
    computeKeypoints();
    clarifyKeypoints();
    filterKeypoints();
    finishKeypoints();

    for(Keypoint kp : _data.points)
    {
        buildDescriptor(kp, _data.dog, *d);
    }

    return d;
}




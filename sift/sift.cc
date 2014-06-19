#include <QDebug>
#include "sift.hh"

#include "sift_math.hh"

#include <cmath>
#include <vector>
#include <string>
#include <stdexcept>

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

void Sift::subpixelExtrema(CImageVec &octave, SiftKeypoint &keypoint)
{
    boost::numeric::ublas::matrix<double> D(3,1),
                                          H(3,3),
                                          HInverce(3,3);

    Math::Diff3(octave, keypoint, D);
    Math::H3x3(octave, keypoint, H);

    if(!Math::MakeInverse(H, HInverce))
        return;

    // апроксимация Х - субпиксельная позиция
    boost::numeric::ublas::matrix<double> shift(3,1);
    shift = boost::numeric::ublas::prod(HInverce, D);

    keypoint.dx = -shift(0,0);
    keypoint.dy = -shift(1,0);
    keypoint.dz = -shift(2,0);
}


Sift::Sift(CImageUnsigned *image, double contrast, double corner)
    : CONTRAST(contrast), CORNER(corner), img(*image)
{ }

void Sift::buildPyramidAndDoG()
{
    CImageDoG pyramid;
    QVector<CImage> images;

    CImage newIm(img);
    for(int y = 0; y < img.height(); y++)
        for(int x = 0; x < img.width(); x++)
            newIm(x,y) = float(img(x,y)) / 255.0f;

    images.append(newIm.blur(0.5).get_resize(-200, -200, -100, -100, 1).blur(1.0));
    images.append(images[0].get_resize( -50,  -50, -100, -100, 1));
    images.append(images[1].get_resize( -50,  -50, -100, -100, 1));
    images.append(images[2].get_resize( -50,  -50, -100, -100, 1));

    for(int i = 0; i < images.size(); i++)
    {
        pyramid.push_back(CImageVec());
        pyramid[i].push_back(images[i]);
        double sigma = Math::base();
        for(int s = 1; s < 5; s++, sigma *= Math::base())
        {
            pyramid[i].push_back(pyramid[i][s-1].get_blur(sigma));
        }
    }
    for(CImageDoG::size_type i = 0; i < pyramid.size(); i++)
    {
        _data.dog.push_back(CImageVec());
        for(CImageDoG::size_type s = 1; s < pyramid[i].size(); s++)
            _data.dog[i].push_back(pyramid[i][s-1] - pyramid[i][s]);
    }
}


int Sift::computeKeypoints()
{
    for(CImageDoG::size_type i = 0; i < _data.dog.size(); i++)
    {
        for(int s = 1; s < (_data.dog[i].size()-1); s++)
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
                        _data.points.push_back(SiftKeypoint(x, y, s, i));
                }
            }
        }
    }
    return _data.points.size();
}

int Sift::clarifyKeypoints()
{
    const int MaxIters = 5;

    SiftKeypoint::Cont nArray;
    for(SiftKeypoint& kp : _data.points)
    {
        CImage& img = _data.dog[kp.octave][kp.Bl];
        CImageVec& oct = _data.dog[kp.octave];

        double maxX = img.width() - 2;
        double maxY = img.height() - 2;
        double maxZ = oct.size() - 2;
        bool isStable = false;

        for(int it = 0; it < MaxIters; it++)
        {
            subpixelExtrema(oct, kp);

            // достаточно близко
            if((kp.dx < 0.5) &&
               (kp.dy < 0.5) &&
               (kp.dz < 0.5))
            {
                isStable = true;
                break;
            }

            if(kp.dx >= 0.5)
                kp.X += 1;
            if(kp.dy >= 0.5)
                kp.Y += 1;
            if(kp.dz >= 0.5)
                kp.Bl += 1;
            // выход за границы
            if((kp.X < 1)    || (kp.Y < 1)    || (kp.Bl < 1) ||
               (kp.X > maxX) || (kp.Y > maxY) || (kp.Bl > maxZ))
            {
                isStable = false;
                break;
            }
        }
        if(isStable)
        {
            nArray.push_back(kp);
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
    SiftKeypoint::Cont nArray;
    for(SiftKeypoint& kp : _data.points)
    {
        contrast = _data.dog[kp.octave][kp.Bl](kp.X,kp.Y);
        if(qAbs(contrast) >= CONTRAST)
            nArray.push_back(kp);
    }
    _data.points.swap(nArray);
    nArray.clear();

    // удаляем на рёбрах
    for(SiftKeypoint& kp : _data.points)
    {
        Math::H2x2(_data.dog[kp.octave][kp.Bl], kp.X, kp.Y, H);
        tr = H(0,0) + H(1,1);
        det = Math::Det(H);

        double value = (tr*tr)/det;
        double corner = (CORNER+1)*(CORNER+1)/CORNER;
        if((det >= 0) && (value <= corner))
            nArray.push_back(kp);
    }
    _data.points.swap(nArray);

    return _data.points.size();
}

void Sift::finishKeypoints()
{
    for(SiftKeypoint::Cont::value_type& point : _data.points)
    {
        point.neighbourhood.clear();
        double sigma = Math::Sigma(point.octave);
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
                    point.neighbourhood.push_back(SiftKeypoint(x, y, point.Bl, point.octave));
            }
        }

        // считаем магнитуды и углы для точек окна
        for(SiftKeypoint::Cont::value_type& neibour : point.neighbourhood)
        {
            CImage& img = _data.dog[neibour.octave][neibour.Bl];

            neibour.magnitude = sqrt(pow(img(neibour.X+1,neibour.Y)   - img(neibour.X-1,neibour.Y),   2.0f)
                                   + pow(img(neibour.X,  neibour.Y+1) - img(neibour.X,  neibour.Y-1), 2.0f));

            neibour.angle = atan2((img(neibour.X,  neibour.Y+1) - img(neibour.X,  neibour.Y-1)),
                                  (img(neibour.X+1,neibour.Y)   - img(neibour.X-1,neibour.Y)));
        }

        // строим гистограмму с 36 бинами
        SiftKeypoint::VectorDouble hist(36, 0.0);
        for(SiftKeypoint::Cont::value_type& neibour : point.neighbourhood)
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

void Sift::computeDescriptors(DescriptorArray &array, KeypointList &points)
{
    _data.dog.clear();
    _data.points.clear();

    buildPyramidAndDoG();
    computeKeypoints();
    clarifyKeypoints();
    filterKeypoints();
    finishKeypoints();

    array.clear();
    points.clear();
    for(SiftKeypoint kp : _data.points)
    {
        buildDescriptor(kp, _data.dog, array, points);
    }
}


void Sift::buildDescriptor(SiftKeypoint& point, const CImageDoG &DoG,
                           DescriptorArray &descriptors, KeypointList& points)
{
    point.neighbourhood.clear();

    // заполняет окно дескриптора
    for(int i = 0; i < 16; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            int x = point.X-(16/2)+j;
            int y = point.Y-(16/2)+i;

            if(!((x <= 0) || (y <= 0) ||
                 (x >= DoG[point.octave][point.Bl].width()-1) ||
                 (y >= DoG[point.octave][point.Bl].height()-1)))
            {
                point.neighbourhood.push_back(SiftKeypoint(x, y, point.Bl, point.octave));
            }
        }
    }

    //  Считаем градиенты для окна
    for(SiftKeypoint& kp : point.neighbourhood)
    {
        const CImage& img = DoG[kp.octave][kp.Bl];
        kp.magnitude = sqrt(pow(img(kp.X+1,kp.Y)   - img(kp.X-1,kp.Y),   2.0f)
                          + pow(img(kp.X,  kp.Y+1) - img(kp.X,  kp.Y-1), 2.0f));

        // в радианах
        kp.angle = atan2((img(kp.X,  kp.Y+1) - img(kp.X,  kp.Y-1)),
                         (img(kp.X+1,kp.Y)   - img(kp.X-1,kp.Y)));
    }

    for(SiftKeypoint::Histogram::value_type& peak : point.angmag)
    {
        SiftKeypoint::D3Histogram hist(4, SiftKeypoint::MatrixDouble());
        for(int i = 0; i < 4; i++)
        {
            hist[i].resize(4);
            for(int h = 0; h < 4; h++)
                hist[i][h].resize(8);
        }
        // 4x4x8  x y angle
        for(SiftKeypoint& neib : point.neighbourhood)
        {
            neib.angle = 180.0 + (neib.angle * 180.0 / Math::PI());
            neib.angle = peak.first - neib.angle;

            // углы должны быть от 0 до 360 градусов
            while(neib.angle < 0.0)
                neib.angle = 360.0 - neib.angle;
            while(neib.angle >= 360.0)
                neib.angle -= 360.0;

            int iX = (neib.X-point.X+8)/4;
            int iY = (neib.Y-point.Y+8)/4;
            int iZ = neib.angle/45;

            double gauss = Math::Gaussian(neib.X - point.X, neib.Y - point.Y, 16/2);
            hist[iX][iY][iZ] += neib.magnitude * gauss;
        }

        descriptors.push_back(QVector<double>());

        double fact[] = {0.5, 1.0, 2.0, 4.0};
        points.push_back(Keypoint((int)point.X * fact[point.octave],
                                  (int)point.Y * fact[point.octave]));
        DescriptorArray::value_type& line = descriptors.back();

        for(uint x = 0; x < hist.size(); x++)
            for(uint y = 0; y < hist[x].size(); y++)
                for(uint z = 0; z < hist[x][y].size(); z++)
                    line.push_back(hist[x][y][z]);

        auto accumSqr = [](double accum, double val) { return accum+val*val; };

        double norm = std::accumulate(line.begin(), line.end(), 0.0, accumSqr);
        std::for_each(line.begin(), line.end(), [norm](double& a){ a /= norm; if(a > 0.2) a = 0.2; });

        norm = std::accumulate(line.begin(), line.end(), 0.0, accumSqr);
        std::for_each(line.begin(), line.end(), [norm](double& a){ a /= norm; });
    }
}

void *create(CImageUnsigned *image, const VectorDouble *params)
{
    if(!image)
        throw std::invalid_argument("pointer cannot be null");

    if(params->size() != 2)
        throw std::invalid_argument("size of array must be 2");

    Sift* sift = new Sift(image, params->at(0), params->at(1));
    return sift;
}

void clear(void* data)
{
    if(!data)
        throw std::invalid_argument("pointer cannot be null");

    Sift* sift = (Sift*)data;
    delete sift;
}

void build(void* data, DescriptorArray* descriptors, KeypointList* keypoints)
{
    if(!data || !descriptors || !keypoints)
        throw std::invalid_argument("pointer cannot be null");

    Sift* sift = (Sift*)data;
    sift->computeDescriptors(*descriptors, *keypoints);
}

void getParams(void* data, VectorDouble *params)
{
    if(!data || !params)
        throw std::invalid_argument("pointer cannot be null");

    Sift* sift = (Sift*)data;
    params->clear();
    params->push_back(sift->contrast());
    params->push_back(sift->corner());
}

void info(LibraryInfo *info)
{
    if(!info)
        throw std::invalid_argument("pointer cannot be null");

    info->info = QObject::tr("Реализация алгоритма SIFT");
    info->type = LibAlgorithm;
}


void getLibraryAPIVersion(QString *version)
{
    if(!version)
        throw std::invalid_argument("pointer cannot be null");

    *version = LibraryAPIVersion();
}


void getDefaultValues(VectorDouble *params)
{
    if(!params)
        throw std::invalid_argument("pointer cannot be null");

    params->clear();
    params->push_back(0.03);
    params->push_back(10.0);
}


void getParamNames(QStringList *params)
{
    if(!params)
        throw std::invalid_argument("pointer cannot be null");

    params->clear();
    params->push_back("Contrast");
    params->push_back("Corner");
}

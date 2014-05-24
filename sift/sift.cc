#include <QDebug>
#include "sift.hh"

#include "sift_math.hh"

#include <cmath>
#include <vector>
#include <string>

using namespace std;

int Sift::getKernelSize(double sigma)	// this method was taken over from AIShack
{
    int MAX_KERNEL_SIZE = 20, i;
    for (i = 0; i < MAX_KERNEL_SIZE; i++)
    {
        double top = i*i*-1;
        if ( exp( top / (2.0*sigma*sigma)) < 0.001)
            break;
    }

    return 2*i-1;
}

bool Sift::isMin(const CImage &img, float px, int pos_x, int pos_y, bool dont_check_pos_xy)
{
    if(img(pos_x-1, pos_y-1) >= px) return false;
    if(img(pos_x-1, pos_y  ) >= px) return false;
    if(img(pos_x-1, pos_y+1) >= px) return false;
    if(img(pos_x  , pos_y-1) >= px) return false;
    if(img(pos_x  , pos_y  ) >= px) if(!dont_check_pos_xy) return false;
    if(img(pos_x  , pos_y+1) >= px) return false;
    if(img(pos_x+1, pos_y-1) >= px) return false;
    if(img(pos_x+1, pos_y  ) >= px) return false;
    if(img(pos_x+1, pos_y+1) >= px) return false;
    return true;
}

bool Sift::isMax(const cimg_library::CImg<float> &img, float px, int pos_x, int pos_y, bool dont_check_pos_xy)
{
    if(img(pos_x-1, pos_y-1) <= px) return false;
    if(img(pos_x-1, pos_y  ) <= px) return false;
    if(img(pos_x-1, pos_y+1) <= px) return false;
    if(img(pos_x  , pos_y-1) <= px) return false;
    if(img(pos_x  , pos_y  ) <= px) if((dont_check_pos_xy == false)) return false;
    if(img(pos_x  , pos_y+1) <= px) return false;
    if(img(pos_x+1, pos_y-1) <= px) return false;
    if(img(pos_x+1, pos_y  ) <= px) return false;
    if(img(pos_x+1, pos_y+1) <= px) return false;

    return true;
}

bool Sift::getSubPixelExtrema(CImageVec &octave, Keypoint &feature)
{
    using namespace boost::numeric::ublas;
    //
    // compute dD
    matrix<double> dD(3,1);	// [0] = dD/dx, [1] = dD/dy, [2] = dD/dz; D = pixel
    Math::diff3D(octave, feature.x, feature.y, feature.z, dD);
    // compute H = (d^2)D/d(x^2) ... second order derivative
    matrix<double> H(3,3), H_1(3,3);	// H_1 = inverse of H
    Math::hessian3x3(octave, feature.x, feature.y, feature.z, H);
    // compute aX (approcimation of X - the subpixel position); aX = - (H^(-1)) * dD
    if(!Math::inverse(H, H_1)) return false;
    matrix<double> aX(3,1); aX = prod(H_1, dD);
    feature.dx = -aX(0,0); feature.dy = -aX(1,0); feature.dz = -aX(2,0);
    return true;
}

CImage Sift::normalize(const cimg_library::CImg<unsigned char> &img)
{
    CImage norm(img);
    for(int y = 0, ym = img.height(); y < ym; y++)
        for(int x = 0, xm = img.width(); x < xm; x++)
            norm(x,y) = float(img(x,y)) / 255.0f;

    return norm;
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

void Sift::buildPyramid()
{
    CImageDoG pyramid;
    //
    /* 1. varianta */
    CImage img_aa = normalize(img).blur(0.5);	// normalization & antialiasing
    // Octaves (resample)
    // 1st arg = 200% of x, 2nd arg = 200% of y, 3rd arg = 100% of z, 4th arg = 100% of c, 5th arg = interpolation (3-linear,1-none), rest is default; blur(0.5) is antialiasing
    CImage img_m2   = img_aa  .get_resize(-200, -200, -100, -100, 1).blur(1.0);	// pre-blur the base image
    CImage img_orig = img_m2  .get_resize(- 50, - 50, -100, -100, 1);	// negative value means percentage(%); positive value is absolute size in px
    CImage img_d2   = img_orig.get_resize(- 50, - 50, -100, -100, 1);
    CImage img_d4   = img_d2  .get_resize(- 50, - 50, -100, -100, 1);
    //
    const CImage* images[4] = { &img_m2, &img_orig, &img_d2, &img_d4 };
    //
    // Scale spaces (blur)
    for(int i = 0; i < 4; i++)
    {
        pyramid.push_back(vector<cimg_library::CImg<float> >());
        pyramid[i].push_back(*(images[i]));
        double sigma = Math::base();
        for(size_t s = 1; s < 5; s++)
        {	// use only the first column (m_sigmas[0]), because the multiplication (2x,4x,8x) is equivalent to downsampling
            // -- f.e. if you resample an image by 1/2, then you consider only every 2nd pixel - downsampling is used
            //         instead of bluring with a bigger kernel because of performace
            pyramid[i].push_back(pyramid[i][s-1].get_blur(sigma));	// CImg::blur(sigma) uses the Gaussian blur
            sigma *= Math::base();
        }
    }
    for(size_t i = 0, im = pyramid.size(); i < im; i++)
    {
        _data.dog.push_back(CImageVec());
        for(size_t s = 1, sm = pyramid[i].size(); s < sm; s++)
            _data.dog[i].push_back(pyramid[i][s-1] - pyramid[i][s]);
    }
}


int Sift::getFeatureCandidates()
{
    for(size_t i = 0, im = _data.dog.size(); i < im; i++)
    {
        for(size_t s = 1, sm = _data.dog[i].size() - 1; s < sm; s++)
        {
            for(int y = 1, ym = _data.dog[i][s].height() - 1; y < ym; y++)
            {
                for(int x = 1, xm = _data.dog[i][s].width() - 1; x < xm; x++)
                {
                    float px = _data.dog[i][s](x,y);
                    // check for local extrema

                    bool max = true;
                    bool min = true;
                    for(int k = -1; k < 2; k++)
                        min = min && isMin(_data.dog[i][s+k], px, x, y, k == 0 );

                    for(int k = -1; k < 2; k++)
                        max = max && isMax(_data.dog[i][s+k], px, x, y, k == 0 );

                    if(min || max)
                        _data.points.push_back(Keypoint(x, y, s, i));	// save the feature candidate
                }
            }
        }
    }
    return _data.points.size();
#ifdef _DEBUG
    cout << '\t' << data.points.size() << " local extremas was detected." << endl;
#endif	// _DEBUG
}

int Sift::getSubPixelLocations()
{

    const int MAX_ITERATIONS = 5;
    //
    bool stable;
    double max_x, max_y, max_z;
    int y = 0;
    for(vector<Keypoint>::iterator f_it = _data.points.begin(); f_it != _data.points.end(); y++ /*++f_it*/)
    {
        stable = false;
        max_x = _data.dog[f_it->octave][f_it->z].width() - 2;
        max_y = _data.dog[f_it->octave][f_it->z].height() - 2;
        max_z = _data.dog[f_it->octave].size() - 2;
        for(int it = 0; it < MAX_ITERATIONS; it++)
        {
            getSubPixelExtrema(_data.dog[f_it->octave], *f_it);
            if((f_it->dx < 0.5) && (f_it->dy < 0.5) && (f_it->dz < 0.5))
            {
                stable = true;
                break;	// stable feature
            }
            // else update position and try it again
            f_it->x += double(f_it->dx >= 0.5);
            f_it->y += double(f_it->dy >= 0.5);
            f_it->z += double(f_it->dz >= 0.5);
            // check for out-of-image location
            if((f_it->x < 1) || (f_it->y < 1) || (f_it->z < 1) || (f_it->x > max_x) || (f_it->y > max_y) || (f_it->z > max_z))
            {
                stable = false;
                break;
            }
        }
        if(!stable)	// unstable feature
        {
            f_it = _data.points.erase(f_it);
            continue;
        }
        ++f_it;
    }
    return _data.points.size();
#ifdef _DEBUG
    cout << '\t' << data.points.size() << " local extremas remain after the subpixel localization step." << endl;
#endif	// _DEBUG
}

int Sift::removeUnstableFeatures()
{
    // remove data.points with low contrast or points that lie on an edge
    boost::numeric::ublas::matrix<double> H(2,2);
    double contrast, tr, det;
    for(vector<Keypoint>::iterator f_it = _data.points.begin(); f_it != _data.points.end(); )
    {
        // pozn.: na zkusebnich obrazcich davaji obe verze totozne vysledky, coz je dobre, ale
        //        zatim to necham pixoleve, kdyz stejne subpixelovou informaci dal nepouzivam
        //   -- pixel version
        contrast = _data.dog[f_it->octave][f_it->z](f_it->x,f_it->y);
        if(fabs(contrast) < CONTRAST)
        {
            f_it = _data.points.erase(f_it);	// returns the following element
            continue;
        }
        // the following element
        ++f_it;
    }
#ifdef _DEBUG
    cout << '\t' << data.points.size() << " local extremas remain after the low contrast elimination step." << endl;
#endif	// _DEBUG
    int y = _data.points.size();
    for(vector<Keypoint>::iterator f_it = _data.points.begin(); f_it != _data.points.end(); )
    {
        // 2. the edge-like points check
        Math::hessian2x2(_data.dog[f_it->octave][f_it->z], f_it->x, f_it->y, H);
        tr = H(0,0) + H(1,1);
        det = Math::determinant(H);
        if((det < 0) || (((tr*tr)/det) > ((CORNER+1)*(CORNER+1)/CORNER)))
        {
            f_it = _data.points.erase(f_it);	// returns the following element
            continue;
        }
        // the following element
        ++f_it;
    }

    return _data.points.size();
#ifdef _DEBUG
    cout << '\t' << data.points.size() << " local extremas remain after the edgy points elimination step." << endl;
#endif	// _DEBUG
}

void Sift::computeFeatureAttributes()
{
    int kernelSize;
    double sigma;
    for(vector<Keypoint>::iterator f_it = _data.points.begin(); f_it != _data.points.end(); ++f_it)
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
                Keypoint point( f_it->x - (kernelSize/2) + j,
                                f_it->y - (kernelSize/2) + i,
                                f_it->z, f_it->octave);
                // kontrola rozmeru
                bool correct = (point.x <= 0) || (point.y <= 0) ||
                                (point.x >= _data.dog[f_it->octave][f_it->z].width()-1) ||
                                (point.y >= _data.dog[f_it->octave][f_it->z].height()-1);
                if(!correct)
                    f_it->neighbourhood.push_back(point);
            }
        }
        // 2. pro kazdy bod okenka spocitat magnitudu a uhel
        //    -- m(x,y)=sqrt(((L(x+1,y)-L(x-1,y))^2 + (L(x,y+1)-L(x,y-1))^2));
        //    -- angle(x,y)=atan((L(x,y+1)-L(x,y-1)) / (L(x+1,y)-L(x-1,y)));
        for(vector<Keypoint>::iterator n_it = f_it->neighbourhood.begin(); n_it != f_it->neighbourhood.end(); ++n_it)
        {
            n_it->magnitude = sqrt(pow(_data.dog[n_it->octave][n_it->z](n_it->x+1,n_it->y) - _data.dog[n_it->octave][n_it->z](n_it->x-1,n_it->y), 2.0f)
                                 + pow(_data.dog[n_it->octave][n_it->z](n_it->x,n_it->y+1) - _data.dog[n_it->octave][n_it->z](n_it->x,n_it->y-1), 2.0f));
            // angle is in radians
            n_it->angle = atan2((_data.dog[n_it->octave][n_it->z](n_it->x,n_it->y+1) - _data.dog[n_it->octave][n_it->z](n_it->x,n_it->y-1)),
                                (_data.dog[n_it->octave][n_it->z](n_it->x+1,n_it->y) - _data.dog[n_it->octave][n_it->z](n_it->x-1,n_it->y)));
            //n_it->angle = atan((data.dog[n_it->octave][n_it->z](n_it->x,n_it->y+1) - data.dog[n_it->octave][n_it->z](n_it->x,n_it->y-1)) /
            //				   (data.dog[n_it->octave][n_it->z](n_it->x+1,n_it->y) - data.dog[n_it->octave][n_it->z](n_it->x-1,n_it->y)));
        }
        // 3. nahazet do histogramu uhlu o 36 binech, tedy 1bin=10stupnu nebo PI/18rad
        //    -- magnituda binu se pocita jako suma pres body v binu: m(x,y)*Gauss(x,y,1.5*sigma)
        vector<double> hist(36, 0.0);
        for(vector<Keypoint>::iterator n_it = f_it->neighbourhood.begin(); n_it != f_it->neighbourhood.end(); ++n_it)
        {
            n_it->angle = 180 + (n_it->angle * 180.0 / Math::PI());	// rad2deg
            hist[int(n_it->angle) / 10] += n_it->magnitude * Math::Gaussian2D(n_it->x - f_it->x, n_it->y - f_it->y, 1.5*sigma);
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
    buildPyramid();
    getFeatureCandidates();
    getSubPixelLocations();
    removeUnstableFeatures();
    computeFeatureAttributes();

    for(Keypoint kp : _data.points)
    {
        buildDescriptor(kp, _data.dog, *d);
    }

    return d;
}

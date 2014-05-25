#include "keypoint.hh"
#include "sift_math.hh"

using namespace std;

void buildDescriptor(Keypoint& point, const CImageDoG &DoG, Descriptor &descriptors)
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
                point.neighbourhood.push_back(Keypoint(x, y, point.Bl, point.octave));
            }
        }
    }

    //  Считаем градиенты для окна
    for(Keypoint& kp : point.neighbourhood)
    {
        const CImage& img = DoG[kp.octave][kp.Bl];
        kp.magnitude = sqrt(pow(img(kp.X+1,kp.Y)   - img(kp.X-1,kp.Y),   2.0f)
                            + pow(img(kp.X,  kp.Y+1) - img(kp.X,  kp.Y-1), 2.0f));

        // в радианах
        kp.angle = atan2((img(kp.X,  kp.Y+1) - img(kp.X,  kp.Y-1)),
                         (img(kp.X+1,kp.Y)   - img(kp.X-1,kp.Y)));
    }

    for(Keypoint::Histogram::value_type& am_it : point.angmag)
    {
        Keypoint::D3Histogram hist(4, Keypoint::MatrixDouble());
        for(int i = 0; i < 4; i++)
        {
            hist[i].resize(4);
            for(int h = 0; h < 4; h++)
                hist[i][h].resize(8);
        }
        // 4x4x8 x y ,angle]
        for(Keypoint& neib : point.neighbourhood)
        {
            neib.angle = 180.0 + (neib.angle * 180.0 / Math::PI());
            neib.angle = am_it.first - neib.angle;

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

        descriptors.push_back(QList<double>());
        Descriptor::value_type& line = descriptors.back();

        for(int x = 0; x < hist.size(); x++)
            for(int y = 0; y < hist[x].size(); y++)
                for(int z = 0; z < hist[x][y].size(); z++)
                    line.push_back(hist[x][y][z]);

        auto accumSqr = [](double accum, double val) { return accum+val*val; };

        double norm = std::accumulate(line.begin(), line.end(), 0.0, accumSqr);
        std::for_each(line.begin(), line.end(), [norm](double& a){ a /= norm; if(a > 0.2) a = 0.2; });

        norm = std::accumulate(line.begin(), line.end(), 0.0, accumSqr);
        std::for_each(line.begin(), line.end(), [norm](double& a){ a /= norm; });
    }
}

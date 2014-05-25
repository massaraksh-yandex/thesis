#include "keypoint.hh"
#include "sift_math.hh"

using namespace std;

void buildDescriptor(Keypoint& point, const CImageDoG &DoG, Descriptor &descriptors)
{
    double sigma;
    int index;
    point.neighbourhood.clear();
    sigma = Math::sigma(point.octave, 0);

    // заполняет окно дескриптора
    for(int i = 0; i < 16; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            Keypoint kp(point.X-(16/2)+j, point.Y-(16/2)+i, point.Bl, point.octave);
            if(!((kp.X <= 0) || (kp.Y <= 0) ||
                (kp.X >= DoG[point.octave][point.Bl].width()-1) ||
                (kp.Y >= DoG[point.octave][point.Bl].height()-1)))
            {
                point.neighbourhood.push_back(kp);
            }
        }
    }

    //  Считаем градиенты для окна
    for(Keypoint& kp : point.neighbourhood)
    {
        kp.magnitude = sqrt(pow(DoG[kp.octave][kp.Bl](kp.X+1,kp.Y) - DoG[kp.octave][kp.Bl](kp.X-1,kp.Y), 2.0f)
                                + pow(DoG[kp.octave][kp.Bl](kp.X,kp.Y+1) - DoG[kp.octave][kp.Bl](kp.X,kp.Y-1), 2.0f));

        // в радианах
        kp.angle = atan2((DoG[kp.octave][kp.Bl](kp.X,kp.Y+1) - DoG[kp.octave][kp.Bl](kp.X,kp.Y-1)),
                            (DoG[kp.octave][kp.Bl](kp.X+1,kp.Y) - DoG[kp.octave][kp.Bl](kp.X-1,kp.Y)));
    }

    for(Keypoint::Histogram::value_type& am_it : point.angmag)//= point.angmag.begin(); am_it != point.angmag.end(); ++am_it)
    {
        // 3. sloucit body do okna 4x4 pres histogramy, ktere maji 8binu
        //    -- magnituda binu se pocita jako suma pres body v binu: m(x,y)*Gauss(x,y,sigma=window_size/2=16/2=8)
        //    -- uhly pocitat relativne k uhlu keypointu (pro kazdou orientaci jeden deskriptor)
        vector<vector<vector<double> > > hist(4, vector<vector<double> >(4, vector<double>(8, 0.0)));	// 4x4x8; [x,y,angle]
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

            double gauss = Math::Gaussian2D(neib.X - point.X, neib.Y - point.Y, 16/2);
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

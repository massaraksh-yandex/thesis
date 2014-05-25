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

    for(vector<pair<double,double> >::iterator am_it = point.angmag.begin(); am_it != point.angmag.end(); ++am_it)
    {
        // 3. sloucit body do okna 4x4 pres histogramy, ktere maji 8binu
        //    -- magnituda binu se pocita jako suma pres body v binu: m(x,y)*Gauss(x,y,sigma=window_size/2=16/2=8)
        //    -- uhly pocitat relativne k uhlu keypointu (pro kazdou orientaci jeden deskriptor)
        vector<vector<vector<double> > > hist(4, vector<vector<double> >(4, vector<double>(8, 0.0)));	// 4x4x8; [x,y,angle]
        for(Keypoint& n_it : point.neighbourhood)
        {
            n_it.angle = 180.0 + (n_it.angle * 180.0 / Math::PI());
            n_it.angle = am_it->first - n_it.angle;

            // углы должны быть от 0 до 360 градусов
            while(n_it.angle <    0.0) n_it.angle = 360.0 - n_it.angle;
            while(n_it.angle >= 360.0) n_it.angle = n_it.angle - 360.0;
            hist[(n_it.X-point.X+8)/4][(n_it.Y-point.Y+8)/4][int(n_it.angle)/45] += n_it.magnitude * Math::Gaussian2D(n_it.X - point.X, n_it.Y - point.Y, 16/2);
        }

        index = descriptors.size();
        descriptors.push_back(QList<double>());
        for(size_t xi = 0, xim = hist.size(); xi < xim; xi++)
            for(size_t yi = 0, yim = hist[xi].size(); yi < yim; yi++)
                for(size_t ai = 0, aim = hist[xi][yi].size(); ai < aim; ai++)
                    descriptors[index].push_back(hist[xi][yi][ai]);


        int size = descriptors[index].size();
//        double norm = 0.0;

        Descriptor::value_type& line = descriptors[index];
        auto accumSqr = [](double accum, double val) { return accum+val*val; };
        double norm = std::accumulate(line.begin(), line.end(), 0.0, accumSqr);
//        for(int i = 0; i < size; i++)
//            norm += descriptors[index][i] * descriptors[index][i];

        std::for_each(line.begin(), line.end(), [norm](double& a){ a /= norm; if(a > 0.2) a = 0.2; });
//        for(int i = 0; i < size; i++)
//        {
//            descriptors[index][i] /= norm;
//            if(descriptors[index][i] > 0.2)
//                descriptors[index][i] = 0.2;
//        }
//        norm = 0.0;
//        for(int i = 0; i < size; i++)
//            norm += descriptors[index][i] * descriptors[index][i];
        norm = std::accumulate(line.begin(), line.end(), 0.0, accumSqr);
//        for(int i = 0; i < size; i++)
//            descriptors[index][i] /= norm;
        std::for_each(line.begin(), line.end(), [norm](double& a){ a /= norm; });
    }
}

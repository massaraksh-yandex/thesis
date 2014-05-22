#include "keypoint.hh"
#include "sift_math.hh"

using namespace std;

void buildDescriptor(Keypoint& point, const CImageDoG &DoG, Descriptor &descriptors)
{
    double sigma;
    size_t index;
    point.neighbourhood.clear();
    sigma = Math::sigma(point.octave, 0);
    // 1. kolem keypointu okenko 16x16px
    for(int i = 0; i < 16; i++)
    {
        for(int j = 0; j < 16; j++)
        {
            std::vector<Keypoint>& nb = point.neighbourhood;
            nb.push_back(Keypoint());
            Keypoint kp(point.x-(16/2)+j, point.y-(16/2)+i, point.z, point.octave);
            if(!((kp.x <= 0) || (kp.y <= 0) ||
                (kp.x >= DoG[point.octave][point.z].width()-1) ||
                (kp.y >= DoG[point.octave][point.z].height()-1)))
            {
                nb.push_back(kp);
            }
        }
    }
    // 2. pro kazdy bod okenka spocitat magnitudu a uhel
    //    -- m(x,y)=sqrt(((L(x+1,y)-L(x-1,y))^2 + (L(x,y+1)-L(x,y-1))^2));
    //    -- angle(x,y)=atan((L(x,y+1)-L(x,y-1)) / (L(x+1,y)-L(x-1,y)));
    for(vector<Keypoint>::iterator n_it = point.neighbourhood.begin(); n_it != point.neighbourhood.end(); ++n_it)
    {
        n_it->magnitude = sqrt(pow(DoG[n_it->octave][n_it->z](n_it->x+1,n_it->y) - DoG[n_it->octave][n_it->z](n_it->x-1,n_it->y), 2.0f)
                                + pow(DoG[n_it->octave][n_it->z](n_it->x,n_it->y+1) - DoG[n_it->octave][n_it->z](n_it->x,n_it->y-1), 2.0f));
        // angle is in radians
        n_it->angle = atan2((DoG[n_it->octave][n_it->z](n_it->x,n_it->y+1) - DoG[n_it->octave][n_it->z](n_it->x,n_it->y-1)),
                            (DoG[n_it->octave][n_it->z](n_it->x+1,n_it->y) - DoG[n_it->octave][n_it->z](n_it->x-1,n_it->y)));
        //n_it->angle = atan((DoG[n_it->octave][n_it->z](n_it->x,n_it->y+1) - DoG[n_it->octave][n_it->z](n_it->x,n_it->y-1)) /
        //				   (DoG[n_it->octave][n_it->z](n_it->x+1,n_it->y) - DoG[n_it->octave][n_it->z](n_it->x-1,n_it->y)));
    }
    for(vector<pair<double,double> >::iterator am_it = point.angmag.begin(); am_it != point.angmag.end(); ++am_it)
    {
        // 3. sloucit body do okna 4x4 pres histogramy, ktere maji 8binu
        //    -- magnituda binu se pocita jako suma pres body v binu: m(x,y)*Gauss(x,y,sigma=window_size/2=16/2=8)
        //    -- uhly pocitat relativne k uhlu keypointu (pro kazdou orientaci jeden deskriptor)
        vector<vector<vector<double> > > hist(4, vector<vector<double> >(4, vector<double>(8, 0.0)));	// 4x4x8; [x,y,angle]
        for(vector<Keypoint>::iterator n_it = point.neighbourhood.begin(); n_it != point.neighbourhood.end(); ++n_it)
        {
            n_it->angle = 180.0 + (n_it->angle * 180.0 / Math::PI());	// rad2deg
            // -- rotacni invariance - uhly pocitat relativne k orientaci keypointu - vzdy musi byt v intervalu <0,360)
            n_it->angle = am_it->first - n_it->angle;
            while(n_it->angle <    0.0) n_it->angle = 360.0 - n_it->angle;
            while(n_it->angle >= 360.0) n_it->angle = n_it->angle - 360.0;
            //
            hist[(n_it->x-point.x+8)/4][(n_it->y-point.y+8)/4][int(n_it->angle)/45] += n_it->magnitude * Math::Gaussian2D(n_it->x - point.x, n_it->y - point.y, 16/2);	// opet by to melo bejt posunuty subpixelove o 0.5px, takze by se spravne melo pricist 7.5
        }
        // 4. ulozit 4x4x8 cisel do vektoru; lze exportovat treba do CSV,SQL,apod.
        index = descriptors.size();
        descriptors.push_back(QList<double>());
        for(size_t xi = 0, xim = hist.size(); xi < xim; xi++)
            for(size_t yi = 0, yim = hist[xi].size(); yi < yim; yi++)
                for(size_t ai = 0, aim = hist[xi][yi].size(); ai < aim; ai++)
                    descriptors[index].push_back(hist[xi][yi][ai]);
        // 5. normalizace magnitud deskriptoru
        double norm = 0.0;
        for(size_t di = 0, dim = descriptors[index].size(); di < dim; di++)	// spocitam normalizacni konstantu
            norm += descriptors[index][di] * descriptors[index][di];
        for(size_t di = 0, dim = descriptors[index].size(); di < dim; di++)	// normalizuju
        {
            descriptors[index][di] /= norm;
            if(descriptors[index][di] > 0.2)
                descriptors[index][di] = 0.2;
        }
        norm = 0.0;
        for(size_t di = 0, dim = descriptors[index].size(); di < dim; di++)	// znovu spocitam normalizacni konstantu
            norm += descriptors[index][di] * descriptors[index][di];
        for(size_t di = 0, dim = descriptors[index].size(); di < dim; di++)	// znovu normalizuju
            descriptors[index][di] /= norm;
    }
}

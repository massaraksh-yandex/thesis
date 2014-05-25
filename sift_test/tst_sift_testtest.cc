#include <QCoreApplication>
#include <QStringList>
#include <QTextStream>
#include <QString>
#include <QtTest>
#include <QFile>
#include <vector>
#include "sift.hh"
#include "../../sift/FeatureExtraction/SIFT.h"

using namespace std;
using namespace cimg_library;

bool compare(const SIFT::Feature& f, const Keypoint& k)
{/*
    double x, y, z;		// pixel position
    double dx, dy, dz;	// subpixel offsets
    int octave;			// octave index
    double magnitude;	// feature magnitude
    double angle;		// feature direction (angle)
    int direction;		// feature direction (index of histogram bin)*/

    if(f.x != k.X || f.y != k.Y || f.z != k.Bl)
        return false;
    if(f.dx != k.dx || f.dy != k.dy || f.dz != k.dz)
        return false;

    if(f.octave != k.octave)
        return false;

    return true;
}

class Sift_testTest : public QObject
{
    Q_OBJECT

    Sift* sift;
    QStringList list;
    QThread* _thread;

    void fn(int i)
    {
        vector<vector<CImg<float> > > pyramid;
        vector<vector<CImg<float> > > DoG;
        vector<SIFT::Feature> features;
        std::vector<std::vector<double> > SIFTDesc;

        QString name = list[i];

        cimg_library::CImg<unsigned char> img;
        sift->load(name);
        img.load(name.toStdString().c_str());

        sift->buildPyramidAndDoG();
        SIFT::step1_buildPyramid(img, pyramid);
        SIFT::step2_getDoG(pyramid, DoG);

        qDebug() << sift->data().dog.size() << DoG.size();
        QVERIFY(sift->data().dog.size() == DoG.size());

        int aa = SIFT::step3_getFeatureCandidates(DoG, features);
        int bb = sift->computeKeypoints();
        QVERIFY2(aa == bb, "getFeatureCandidates");


        QVERIFY2(SIFT::step4_getSubPixelLocations(DoG, features) ==
                 sift->clarifyKeypoints(), "getSubPixelLocations");

        QVERIFY(sift->data().points.size() == features.size());

        for(int yy = 0; yy <features.size(); yy++)
        {
           if(!compare(features[yy], sift->data().points[yy]))
           {
               int tt = 4;
               compare(features[yy], sift->data().points[yy]);
           }
        }

        aa = sift->filterKeypoints();
        bb = SIFT::step5_removeUnstableFeatures(DoG, features);

        QVERIFY2(aa == bb, "removeUnstableFeatures");

        SIFT::step6_computeFeatureAttributes(DoG, features);
        sift->finishKeypoints();
        QVERIFY(sift->data().points.size() == features.size());

        DescriptorPtr ptr(new Descriptor());
        auto dd = sift->data();
        for(int rr = 0; rr < dd.points.size(); rr++)
        {
            buildDescriptor(dd.points[rr], dd.dog, *ptr);
        }

        for(int rr = 0; rr < features.size(); rr++)
        {
            features[rr].getFeatureDescriptor(DoG, SIFTDesc);
        }

        qDebug() << "Число дескрипторов" << ptr->size();
        qDebug() << "Число дескрипторов SIFT" << SIFTDesc.size();

        QFile file(name + ".csv");
        QVERIFY2(file.open(QFile::ReadOnly | QFile::Text), "Не могу открыть файл");

        Descriptor& fromSift = *ptr;
        QTextStream stream(&file);

        int index = 0;
        while(!stream.atEnd())
        {
            auto lineStr1 = stream.readLine();
            if(lineStr1.isEmpty())
                break;
            auto lineStr = lineStr1.split(',');

            QCOMPARE(lineStr.size(), fromSift[index].size());

            QList<double> lineDouble;
            lineDouble.reserve(lineStr.size());
            for(QString& str : lineStr)
            {
                bool b;
                double dd = str.toDouble(&b);
                QVERIFY2(b, ("Плохое число " + str).toStdString().c_str());
                lineDouble.push_back(dd);
            }
            QVERIFY(lineDouble.size() == fromSift[index].size());
            for(int i = 0; i < lineDouble.size(); i++)
                QVERIFY( qAbs(fromSift[index][i] - lineDouble[i]) < 0.001);

            index++;
        }

        QVERIFY2(index == fromSift.size(), "Разное число дескрипторов");
    }

public:
    Sift_testTest();

private Q_SLOTS:
    void testCase0();
    void testCase1();
    void testCase2();
    void testCase3();
};

Sift_testTest::Sift_testTest()
{
    list << "0.jpg" << "1.jpg" << "2.jpg" << "3.jpg";
    sift = new Sift(this);
}

void Sift_testTest::testCase0()
{
    fn(0);
}

void Sift_testTest::testCase1()
{
    fn(1);
}

void Sift_testTest::testCase2()
{
    fn(2);
}

void Sift_testTest::testCase3()
{
    fn(3);
}
QTEST_MAIN(Sift_testTest)

#include "tst_sift_testtest.moc"

#ifndef CORE_HH
#define CORE_HH

#include <QLibrary>
#include <QString>
#include <QList>
#include <QFile>
#include <QVector>

#include "global_core.hh"
#include "logging.hh"
#include "noise.hh"
#include "sift.hh"

class Core : public QObject
{
    Q_OBJECT

    void keypointsFromFile(QString str, KeypointCoords &coords, Descriptor &desc, QString& error);

public:
    Core(QObject *parent) : QObject(parent)
    { }

public slots:
    void computeDescriptorsToFile(QString image, QString filename);
    void compareImages(QString im1, QString im2, int types);
    void testImages(QString dir, ImageNoises types);

signals:
    void log(Log::LogType type, int indent, QString message);
    void progress(int value, int maximum);
    void failed();

    void computingFinished(KeypointCoords, DescriptorPtr);
    void writingFinished();
    void testingFinished(TestingResults);
};

void gaussianImageNoise(CImage& img, double sigma);
void saltAndPepperNoise(CImage& img, double r);

#endif // CORE_HH

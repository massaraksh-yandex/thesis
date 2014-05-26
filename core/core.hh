#ifndef CORE_HH
#define CORE_HH

#include <QLibrary>
#include <QString>
#include <QList>
#include <QVector>

#include "global_core.hh"
#include "logging.hh"
#include "noise.hh"
#include "sift.hh"

typedef unsigned int DescriptorId;

class Core : public QObject
{
    Q_OBJECT

    DescriptorId generateId() const;

    QLibrary lib;
    QList<DescriptorPtr> data;

public:
    Core(QObject *parent) : QObject(parent)
    { }

public slots:
    void computeDescriptors(QString image);
    void writeDescriptor(DescriptorId id, QString name);
    void compareImages(DescriptorId im1, DescriptorId im2);
    void testImages(QString dir, ImageNoises types);

signals:
    void log(Log::LogType type, int indent, QString message);
    void progress(int value, int maximum);

    void computingFinished(DescriptorId);
    void writingFinished();
    void testingFinished(TestingResults);
};

void gaussianImageNoise(CImage& img, double sigma);
void saltAndPepperNoise(CImage& img, double r);

#endif // CORE_HH

#ifndef CORE_HH
#define CORE_HH

#include <QLibrary>
#include <QString>
#include <QList>
#include <QMap>
#include <QFile>
#include <QVector>

#include "global_core.hh"
#include "imageactions.h"
#include "logging.hh"
#include "compareimages.hh"

class QTextStream;
struct BuildDescriptorsData
{
    CImageUnsignedPtr image;
    QTextStream* stream;
};

struct TestImagesData
{
    ImageActionList actions;
    QString path;
    QStringList extencions;
};

class Core : public QObject
{
    Q_OBJECT

    void keypointsFromFile(QString str, KeypointCoords &coords, DescriptorArray &desc, QString& error);

    volatile bool _interrupt;

public:
    Core(QObject *parent) : QObject(parent), _interrupt(true)
    { }

public slots:
    void buildDescriptors(BuildDescriptorsData data);
    void compareImages(QString im1, QString im2);
    void testImages(QString dir, ImageNoises types);

    void interrupt();

signals:
    void log(Log::LogType type, int indent, QString message);
    void progress(int value, int maximum);

    void running(int);

    void compareImagesComplete(Map, KeypointList, KeypointList);
    void buildDescriptorsComplete();
    void testimagesComplete(TestingResultList);
};

#endif // CORE_HH

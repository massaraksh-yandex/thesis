#ifndef CORE_HH
#define CORE_HH

#include <QMetaType>
#include <QLibrary>
#include <QString>
#include <QList>
#include <QVector>

#include "../CImg.h"
#include "noise.hh"
#include "../sift/sift.hh"


typedef void (*BuildDescriptorsByName)(std::string *name, Descriptor* out);
typedef void (*BuildDescriptorsByImage)(CImage *name, Descriptor* out);

typedef unsigned int DescriptorId;
Q_DECLARE_METATYPE(DescriptorId)

namespace Log
{
enum LogType
{
    Message,
    Fail,
    Error
};
}
Q_DECLARE_METATYPE(Log::LogType)

class Core : public QObject
{
    Q_OBJECT

    DescriptorId generateId() const;

    QLibrary lib;
    QList<Descriptor> data;

public:
    Core(QObject *parent) : QObject(parent), lib("sift")
    {
    }

    static BuildDescriptorsByName computeByName;
    static BuildDescriptorsByImage computeByImage;
public slots:
    void load();

    void computeDescriptors(QString image);
    void writeDescriptor(DescriptorId id, QString name);
    double compareImages(DescriptorId im1, DescriptorId im2, bool _emit);
    void testImages(QString dir, Noise::Noises types);

signals:
    void log(Log::LogType type, QString message);
    void progress(int value, int maximum);

    void computingFinished(DescriptorId);
    void writingFinished();
    void testingFinished();
};

#endif // CORE_HH

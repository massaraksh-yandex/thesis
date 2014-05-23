#ifndef CORE_HH
#define CORE_HH

#include <QMetaType>
#include <QLibrary>
#include <QString>
#include <QList>
#include <QVector>

#include "global_core.hh"

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

struct TestingResult
{
    QString filename;
    QList<double> results;

    TestingResult() { }
    TestingResult(QString name, QList<double> res)
        : filename(name), results(res)
    { }
};
typedef QList<TestingResult> TestingResults;
Q_DECLARE_METATYPE(TestingResult)
Q_DECLARE_METATYPE(TestingResults)


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
    void log(Log::LogType type, int indent, QString message);
    void progress(int value, int maximum);

    void computingFinished(DescriptorId);
    void writingFinished();
    void testingFinished(TestingResults);
};

#endif // CORE_HH

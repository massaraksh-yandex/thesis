#ifndef LOGGING_HH
#define LOGGING_HH

#include <QPair>
#include <QList>

namespace Log
{
enum LogType
{
    Message,
    Fail,
    Error
};
}

enum ImageNoiseType
{
    GIN, // гауссов
    SAPIN // соль и перец
};

typedef QPair<ImageNoiseType, double> ImageNoisePair;
typedef QList<ImageNoisePair> ImageNoises;

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


#endif // LOGGING_HH

#ifndef LOGGING_HH
#define LOGGING_HH

#include <QMap>
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

typedef QList<QPair<int, int>> KeypointCoords;
typedef QMap<int, int> Map;

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

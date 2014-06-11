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

struct ImageTestResults
{
    double rate;
    double average;
    double sigma;
};

typedef QPair<ImageNoiseType, double> ImageNoisePair;
typedef QList<ImageNoisePair> ImageNoises;

typedef QList<QPair<int, int>> KeypointCoords;
typedef QMap<int, int> Map;

struct TestingResult
{
    QString filename;
    QList<ImageTestResults> results;

    TestingResult() { }
    TestingResult(QString name, QList<ImageTestResults> res)
        : filename(name), results(res)
    { }
};

typedef QList<ImageTestResults> TestingResultList;


#endif // LOGGING_HH

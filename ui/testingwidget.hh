#ifndef TESTINGWIDGET_HH
#define TESTINGWIDGET_HH

#include <QWidget>
#include <QFile>
#include "logging.hh"

namespace Ui {
class TestingWidget;
}

class TestingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TestingWidget(QWidget *parent = 0);
    ~TestingWidget();

    QList<QPair<ImageNoiseType, double>> data() { return _data; }
    QString path();

private slots:
    void openFolder();
    void addNoise();
    void removeNoise();
    void startPressed();
    void setOutputName();

public slots:
    void finishTesting(TestingResults tr);

signals:
    void log(Log::LogType type, int shift, QString str);
    void accepted();

private:
    Ui::TestingWidget *ui;
    QFile file;
    QStringList noises;
    QList<QPair<ImageNoiseType, double>> _data;

    void enableMainButton();
};

#endif // TESTINGWIDGET_HH

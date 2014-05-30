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

private slots:
    void openFolder();
    void addNoise();
    void removeNoise();
    void startPressed();
    void setOutputName();

public slots:
    void finishTesting(TestingResults tr);
    void block(int en);

signals:
    void log(Log::LogType type, int shift, QString str);
    void startTesting(QString, ImageNoises);

private:
    Ui::TestingWidget *ui;
    QFile file;
    QStringList noises;
    ImageNoises _data;

    void enableMainButton();
};

#endif // TESTINGWIDGET_HH

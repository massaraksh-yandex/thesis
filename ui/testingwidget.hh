#ifndef TESTINGWIDGET_HH
#define TESTINGWIDGET_HH

#include <QWidget>
#include "logging.hh"

namespace Ui {
class TestingWidget;
}

class TestingWidget : public QWidget
{
    Q_OBJECT

    QStringList noises;
    QList<QPair<ImageNoiseType, double>> _data;

public:
    explicit TestingWidget(QWidget *parent = 0);
    ~TestingWidget();

    QList<QPair<ImageNoiseType, double>> data() { return _data; }
    QString path();

private slots:
    void openFolder();
    void comboChanged(int);
    void addNoise();
    void startPressed();
signals:
    void log(Log::LogType type, int shift, QString str);
    void accepted();

private:
    Ui::TestingWidget *ui;
};

#endif // TESTINGWIDGET_HH

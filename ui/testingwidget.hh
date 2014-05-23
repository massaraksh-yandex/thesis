#ifndef TESTINGWIDGET_HH
#define TESTINGWIDGET_HH

#include <QWidget>
#include "logging.hh"
#include "noise.hh"

namespace Ui {
class TestingWidget;
}

class TestingWidget : public QWidget
{
    Q_OBJECT

    QStringList noises;

public:
    explicit TestingWidget(QWidget *parent = 0);
    ~TestingWidget();

private slots:
    void openFolder();
    void comboChanged(int);
    void addNoise();
    void startPressed();
signals:
    void log(Log::LogType type, int shift, QString str);
    void accepted(Noise::Noises);

private:
    Ui::TestingWidget *ui;
};

#endif // TESTINGWIDGET_HH

#ifndef NOISEWIDGET_HH
#define NOISEWIDGET_HH


#include <QDialog>
#include "logging.hh"

namespace Ui {
class NoiseWidget;
}

class NoiseWidget : public QDialog
{
    Q_OBJECT

public:
    explicit NoiseWidget(QWidget *parent = 0);
    ~NoiseWidget();

    ImageNoiseType noise();
    double value();

private slots:
    void spinValueChanged(double val);
    void sliderValueChanged(int val);
    void buttonPressed();
    void comboChanged(int);

signals:
    void cancelled();
    void accepted(QString, double);

private:
    Ui::NoiseWidget *ui;

    void updatePicture();
};

#endif // NOISEWIDGET_HH

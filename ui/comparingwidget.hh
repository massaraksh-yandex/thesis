#ifndef COMPARINGWIDGET_HH
#define COMPARINGWIDGET_HH

#include <QWidget>
#include "logging.hh"

namespace Ui {
class ComparingWidget;
}

class ComparingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ComparingWidget(QWidget *parent = 0);
    ~ComparingWidget();

public slots:
    void show(Map, KeypointCoords, KeypointCoords);

signals:
    void log(Log::LogType type, int shift, QString str);
    void compare(QString, QString, int);

private slots:
    void startPressed();
    void openPressed();

private:
    Ui::ComparingWidget *ui;
};

#endif // COMPARINGWIDGET_HH

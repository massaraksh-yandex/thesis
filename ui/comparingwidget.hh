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

signals:
    void log(Log::LogType type, int shift, QString str);

private:
    Ui::ComparingWidget *ui;
};

#endif // COMPARINGWIDGET_HH

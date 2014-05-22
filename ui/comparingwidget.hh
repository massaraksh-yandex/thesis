#ifndef COMPARINGWIDGET_HH
#define COMPARINGWIDGET_HH

#include <QWidget>

namespace Ui {
class ComparingWidget;
}

class ComparingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ComparingWidget(QWidget *parent = 0);
    ~ComparingWidget();

private:
    Ui::ComparingWidget *ui;
};

#endif // COMPARINGWIDGET_HH

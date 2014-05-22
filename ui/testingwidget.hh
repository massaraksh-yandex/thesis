#ifndef TESTINGWIDGET_HH
#define TESTINGWIDGET_HH

#include <QWidget>

namespace Ui {
class TestingWidget;
}

class TestingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TestingWidget(QWidget *parent = 0);
    ~TestingWidget();

private:
    Ui::TestingWidget *ui;
};

#endif // TESTINGWIDGET_HH

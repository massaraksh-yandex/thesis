#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QWidget>
#include <QVector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void radioButtonClicked();

private:
    Ui::MainWindow *ui;
    QWidget* messages;
    QVector<QWidget*> widgets;

    void setRightWidget(int index);
};

#endif // MAINWINDOW_HH

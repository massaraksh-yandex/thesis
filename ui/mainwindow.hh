#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QWidget>
#include <QVector>

#include "logging.hh"

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
    void toolBoxClicked(int);

signals:
    void log(Log::LogType type, int shift, QString str);

private:
    Ui::MainWindow *ui;
    QWidget* messages;
    QVector<QWidget*> widgets;
};

#endif // MAINWINDOW_HH

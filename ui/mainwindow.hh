#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QWidget>
#include <QVector>

#include "descriptorwidget.hh"
#include "comparingwidget.hh"
#include "testingwidget.hh"
#include "messageswidget.hh"

#include "logging.hh"

class Core;

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(Core* core, QWidget *parent = 0);
    ~MainWindow();

private slots:
    void toolBoxClicked(int);
    void interruptCore();

signals:
    void log(Log::LogType type, int shift, QString str);

private:
    Ui::MainWindow *ui;
    MessagesWidget* messages;
    DescriptorWidget* descriptorWidget;
    ComparingWidget* comparingWidget;
    TestingWidget* testingWidget;
    Core* _core;
};

#endif // MAINWINDOW_HH

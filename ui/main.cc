#include <QDebug>
#include <QApplication>
#include <QThread>

#include "mainwindow.hh"
#include "core.hh"
#include "noise.hh"


int main(int argc, char *argv[])
{
    qRegisterMetaType<ImageNoiseType>("ImageNoiseType");
    qRegisterMetaType<ImageNoisePair>("ImageNoisePair");
    qRegisterMetaType<ImageNoises>("ImageNoises");
    qRegisterMetaType<Log::LogType>("Log::LogType");
    QApplication a(argc, argv);

    Core* c = new Core(0);

    MainWindow* w = new MainWindow(c);

    QThread* thread = new QThread();
    c->moveToThread(thread);

    QObject::connect(w, SIGNAL(startTesting(QString,ImageNoises)),
                     c, SLOT(testImages(QString,ImageNoises)));

    w->show();
    thread->start();

    return a.exec();
}

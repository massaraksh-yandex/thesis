#include <QDebug>
#include <QApplication>
#include <QThread>

#include "mainwindow.hh"
#include "core.hh"
#include "logging.hh"


int main(int argc, char *argv[])
{
    qRegisterMetaType<ImageNoiseType>("ImageNoiseType");
    qRegisterMetaType<ImageNoisePair>("ImageNoisePair");
    qRegisterMetaType<ImageNoises>("ImageNoises");
    qRegisterMetaType<Log::LogType>("Log::LogType");
    qRegisterMetaType<TestingResult>("TestingResult");
    qRegisterMetaType<TestingResultList>("TestingResults");
    qRegisterMetaType<Keypoint::Cont>("Keypoint::Cont");
    qRegisterMetaType<KeypointCoords>("KeypointCoords");
    qRegisterMetaType<Map>("Map");

    QApplication a(argc, argv);

    Core* c = new Core(0);
    MainWindow* w = new MainWindow(c, 0);

    QThread* thread = new QThread();
    c->moveToThread(thread);

    w->show();
    thread->start();

    return a.exec();
}

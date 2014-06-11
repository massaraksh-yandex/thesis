#include <QMetaType>
#include <QThread>
#include "mainwindow.hh"
#include "ui_mainwindow.h"

#include "core.hh"

MainWindow::MainWindow(Core *core, QWidget *parent) :
    QWidget(parent), ui(new Ui::MainWindow),
    _core(core)
{
    ui->setupUi(this);

    messages = new MessagesWidget(this);
    layout()->addWidget(messages);

    descriptorWidget = new DescriptorWidget(ui->pageComputeDescriptor);
    comparingWidget = new ComparingWidget(ui->pageCompareImages);
    testingWidget = new TestingWidget(ui->pageTestSift);

    ui->pageComputeDescriptor->setLayout(new QVBoxLayout(ui->pageComputeDescriptor));
    ui->pageCompareImages->setLayout(new QVBoxLayout(ui->pageCompareImages));
    ui->pageTestSift->setLayout(new QVBoxLayout(ui->pageTestSift));

    ui->pageComputeDescriptor->layout()->addWidget(descriptorWidget);
    ui->pageCompareImages->layout()->addWidget(comparingWidget);
    ui->pageTestSift->layout()->addWidget(testingWidget);

    connect(core, SIGNAL(running(int)), testingWidget,    SLOT(block(int)));
    connect(core, SIGNAL(running(int)), descriptorWidget, SLOT(block(int)));
    connect(core, SIGNAL(running(int)), comparingWidget,  SLOT(block(int)));
    connect(core, SIGNAL(running(int)), messages,         SLOT(block(int)));

    connect(messages, SIGNAL(interrupt()), SLOT(interruptCore()));

    connect(testingWidget, SIGNAL(startTesting(QString,ImageNoises)),
            core, SLOT(testImages(QString,ImageNoises)));
    connect(descriptorWidget, SIGNAL(accepted(QString, QString)),
            core, SLOT(buildDescriptors(QString,QString)));
    connect(comparingWidget, SIGNAL(compare(QString,QString)),
            core, SLOT(compareImages(QString,QString)));

    connect(core, SIGNAL(compareImagesComplete(Map,KeypointCoords,KeypointCoords)),
            comparingWidget, SLOT(show(Map,KeypointCoords,KeypointCoords)));
    connect(core, SIGNAL(testimagesComplete(TestingResultList)),
            testingWidget, SLOT(finishTesting(TestingResultList)));

    connect(ui->toolBox, SIGNAL(currentChanged(int)), SLOT(toolBoxClicked(int)));

    connect(core, SIGNAL(progress(int,int)), messages, SLOT(progress(int,int)));
    connect(core, SIGNAL(log(Log::LogType,int,QString)), messages, SLOT(log(Log::LogType,int,QString)));
    connect(this, SIGNAL(log(Log::LogType,int,QString)), messages, SLOT(log(Log::LogType,int,QString)));
    connect(testingWidget, SIGNAL(log(Log::LogType,int,QString)), messages, SLOT(log(Log::LogType,int,QString)));
    resize(360, 480);
    ui->toolBox->setCurrentIndex(2);
}

MainWindow::~MainWindow()
{
    delete ui;
    _core->thread()->exit();
}

void MainWindow::toolBoxClicked(int i)
{
    int h = 250;
    switch (i) {
    case 0:
        h = 180;
        break;
    case 1:
        h = 220;
        break;
    case 2:
        h = 320;
        break;
    }
    resize(width(), h + messages->height() + 50);
}

void MainWindow::interruptCore()
{
    _core->interrupt();
}












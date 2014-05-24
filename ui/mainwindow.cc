#include "mainwindow.hh"
#include "ui_mainwindow.h"

#include "core.hh"

MainWindow::MainWindow(Core *core, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow), _busy(false)
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

    connect(testingWidget, SIGNAL(accepted()), SLOT(testAccepted()));

    connect(ui->toolBox, SIGNAL(currentChanged(int)), SLOT(toolBoxClicked(int)));
    toolBoxClicked(0);

    connect(core, SIGNAL(log(Log::LogType,int,QString)), messages, SLOT(log(Log::LogType,int,QString)));
    connect(core, SIGNAL(progress(int,int)), messages, SLOT(progress(int,int)));
    connect(this, SIGNAL(log(Log::LogType,int,QString)), messages, SLOT(log(Log::LogType,int,QString)));
    connect(testingWidget, SIGNAL(log(Log::LogType,int,QString)), messages, SLOT(log(Log::LogType,int,QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::toolBoxClicked(int i)
{
    int h;
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
    default:
        break;
    }
    resize(width(), h + messages->height() + 50);
}

void MainWindow::testAccepted()
{
    emit log(Log::Message, 0, "Тестирование начинается");

    emit startTesting(testingWidget->path(), testingWidget->data());
}












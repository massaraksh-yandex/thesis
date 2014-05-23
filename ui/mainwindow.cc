#include "mainwindow.hh"
#include "ui_mainwindow.h"

#include "descriptorwidget.hh"
#include "comparingwidget.hh"
#include "messageswidget.hh"
#include "testingwidget.hh"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    messages = new MessagesWidget(this);
    layout()->addWidget(messages);
    widgets.push_back(new DescriptorWidget(ui->pageComputeDescriptor));
    widgets.push_back(new ComparingWidget(ui->pageCompareImages));
    widgets.push_back(new TestingWidget(ui->pageTestSift));

    ui->pageComputeDescriptor->setLayout(new QVBoxLayout(ui->pageComputeDescriptor));
    ui->pageCompareImages->setLayout(new QVBoxLayout(ui->pageCompareImages));
    ui->pageTestSift->setLayout(new QVBoxLayout(ui->pageTestSift));

    ui->pageComputeDescriptor->layout()->addWidget(widgets[0]);
    ui->pageCompareImages->layout()->addWidget(widgets[1]);
    ui->pageTestSift->layout()->addWidget(widgets[2]);

    connect(ui->toolBox, SIGNAL(currentChanged(int)), SLOT(toolBoxClicked(int)));
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












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

    messages = new MessagesWidget(ui->rightWidget);
    widgets.push_back(new DescriptorWidget(ui->rightWidget));
    widgets.push_back(new ComparingWidget(ui->rightWidget));
    widgets.push_back(new TestingWidget(ui->rightWidget));
    for(int i = 0; i < widgets.size(); i++)
        widgets[i]->setVisible(false);

    auto lay = new QVBoxLayout(ui->rightWidget);
    lay->setMargin(0);
    lay->setSpacing(0);
    lay->addWidget(widgets[0]);
    lay->addWidget(messages);
    ui->rightWidget->setLayout(lay);


    connect(ui->countDescriptor, SIGNAL(clicked()), SLOT(radioButtonClicked()));
    connect(ui->compareImages, SIGNAL(clicked()), SLOT(radioButtonClicked()));
    connect(ui->testSift, SIGNAL(clicked()), SLOT(radioButtonClicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::radioButtonClicked()
{
    int index = 0;
    if(ui->countDescriptor->isChecked())
        index = 0;
    else if(ui->compareImages->isChecked())
        index = 1;
    else if(ui->testSift->isChecked())
        index = 2;

    setRightWidget(index);
}

void MainWindow::setRightWidget(int index)
{
    widgets[index]->setVisible(true);
    auto lay = (QVBoxLayout*)ui->rightWidget->layout();

    for(int i = 0; i < widgets.size(); i++)
    {
        if(lay->indexOf(widgets[i]) != -1)
        {
            lay->removeWidget(widgets[i]);
            widgets[i]->hide();
            break;
        }
    }

    lay->insertWidget(0, widgets[index]);
    lay->update();
}

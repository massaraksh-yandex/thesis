#include "testingwidget.hh"
#include "ui_testingwidget.h"

TestingWidget::TestingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestingWidget)
{
    ui->setupUi(this);
}

TestingWidget::~TestingWidget()
{
    delete ui;
}

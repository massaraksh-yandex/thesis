#include "descriptorwidget.hh"
#include "ui_descriptorwidget.h"

DescriptorWidget::DescriptorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DescriptorWidget)
{
    ui->setupUi(this);
}

DescriptorWidget::~DescriptorWidget()
{
    delete ui;
}

#include "descriptorwidget.hh"
#include "ui_descriptorwidget.h"

#include <QFileDialog>

DescriptorWidget::DescriptorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DescriptorWidget)
{
    ui->setupUi(this);
    connect(ui->buttonOpen, SIGNAL(clicked()), SLOT(openClicked()));
    connect(ui->buttonSave, SIGNAL(clicked()), SLOT(saveClicked()));
    connect(ui->buttonCompute, SIGNAL(clicked()), SLOT(computeClicked()));
}

DescriptorWidget::~DescriptorWidget()
{
    delete ui;
}

void DescriptorWidget::openClicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Открыть изображение", "", "Images (*.png *.jpg)");

    if(!path.isEmpty())
        ui->lineEdit->setText(path);
}

void DescriptorWidget::saveClicked()
{

}

void DescriptorWidget::computeClicked()
{

}

#include "descriptorwidget.hh"
#include "ui_descriptorwidget.h"

#include <QFileDialog>

DescriptorWidget::DescriptorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DescriptorWidget)
{
    ui->setupUi(this);
    connect(ui->buttonOpen, SIGNAL(clicked()), SLOT(openClicked()));
    connect(ui->buttonCompute, SIGNAL(clicked()), SLOT(computeClicked()));
}

DescriptorWidget::~DescriptorWidget()
{
    delete ui;
}

void DescriptorWidget::block(int block)
{
    bool en = bool(block);
    ui->buttonCompute->setEnabled(!en);
}

void DescriptorWidget::openClicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Открыть изображение", "", "Images (*.png *.jpg)");

    if(!path.isEmpty())
        ui->lineEdit->setText(path);
}

void DescriptorWidget::computeClicked()
{
    QString path = QFileDialog::getSaveFileName(this, "Сохранить дескрипторы", "", "*.descr");
    if(!path.isEmpty())
        emit accepted(ui->lineEdit->text(), path);
}

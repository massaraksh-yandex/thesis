#include "comparingwidget.hh"
#include "ui_comparingwidget.h"

ComparingWidget::ComparingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ComparingWidget)
{
    ui->setupUi(this);
}

ComparingWidget::~ComparingWidget()
{
    delete ui;
}

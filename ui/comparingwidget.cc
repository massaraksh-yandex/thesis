#include "comparingwidget.hh"
#include "ui_comparingwidget.h"
#include "showdiffwidget.hh"
#include <QFileDialog>

ComparingWidget::ComparingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ComparingWidget)
{
    ui->setupUi(this);

    connect(ui->pushOpen, SIGNAL(clicked()), SLOT(openPressed()));
    connect(ui->pushOpen2, SIGNAL(clicked()), SLOT(openPressed()));
    connect(ui->pushCompare, SIGNAL(clicked()), SLOT(startPressed()));
}

ComparingWidget::~ComparingWidget()
{
    delete ui;
}

void ComparingWidget::show(Map d, KeypointCoords k1, KeypointCoords k2)
{
    ShowDiffWidget* w = new ShowDiffWidget(k1, k2, d);
    w->open(ui->linePath->text(), ui->linePath2->text());

    w->exec();
}

void ComparingWidget::startPressed()
{
    emit compare(ui->linePath->text(), ui->linePath2->text(), 0);
}

void ComparingWidget::openPressed()
{
    QString str = QFileDialog::getOpenFileName(this, "Открыть изображение", "",
                                               "Images (*.png *.jpg)");

    if(str.isEmpty())
        return;

    if(sender() == ui->pushOpen)
        ui->linePath->setText(str);
    else
        ui->linePath2->setText(str);
}

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
    ShowDiffWidget* w = new ShowDiffWidget(k1, k2, d,
                                           ui->linePath->text(),
                                           ui->linePath2->text(), this);
    w->exec();
}

void ComparingWidget::block(int e)
{
    bool en = bool(e);
    ui->pushCompare->setEnabled(!en);
    ui->pushOpen->setEnabled(!en);
    ui->pushOpen2->setEnabled(!en);
}

void ComparingWidget::startPressed()
{
    emit compare(ui->linePath->text(), ui->linePath2->text(), /*computeType()*/0);
}

void ComparingWidget::openPressed()
{
    QString str = QFileDialog::getOpenFileName(this, "Открыть изображение", "",
                                               "Изображения (*.png *.jpg);; Дескрипторы SIFT(*.deskr)");

    if(str.isEmpty())
        return;

    if(sender() == ui->pushOpen)
        ui->linePath->setText(str);
    else
        ui->linePath2->setText(str);

    ui->pushCompare->setEnabled(!ui->linePath->text().isEmpty() &&
                                !ui->linePath->text().isEmpty());
}

int ComparingWidget::computeType()
{
    int type = QFileInfo(ui->linePath->text()).suffix() == "deskr";
    type += int(QFileInfo(ui->linePath->text()).suffix() == "deskr") * 2;
    return type;
}

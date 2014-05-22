#include "messageswidget.hh"
#include "ui_messageswidget.h"

MessagesWidget::MessagesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MessagesWidget)
{
    ui->setupUi(this);
}

MessagesWidget::~MessagesWidget()
{
    delete ui;
}

void MessagesWidget::log(QString str, int value, int maximum)
{
    QString newText = QString("%1\n%2").arg(ui->text->toPlainText()).arg(str);
    ui->text->setPlainText(newText);
    ui->progress->setValue(value);
    ui->progress->setMaximum(maximum);
}

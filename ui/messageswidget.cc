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

void MessagesWidget::log(Log::LogType type, int shift, QString str)
{
    QString oldText = ui->text->toPlainText();
    QString text = "\n";

    switch(type)
    {
        case Log::Message: ; break;
        case Log::Fail: text += "Проблема: "; break;
        case Log::Error: text += "Ошибка: "; break;
    }

    for(int i = 0; i < shift; i++) text += " ";
    text += str;
    ui->text->setPlainText(oldText + text);
}

void MessagesWidget::progress(int val, int max)
{
    ui->progress->setValue(val);
    ui->progress->setMaximum(max);
}


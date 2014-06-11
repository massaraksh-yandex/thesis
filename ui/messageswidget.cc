#include "messageswidget.hh"
#include "ui_messageswidget.h"
#include "core.hh"
#include <QThread>

MessagesWidget::MessagesWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::MessagesWidget),
    wasInterrupted(false)
{
    ui->setupUi(this);
    connect(ui->pushCancel, SIGNAL(clicked()), SLOT(interruptPushed()));
    connect(ui->pushClear, SIGNAL(clicked()), SLOT(clearPushed()));
    ui->progress->setTextVisible(false);
}

MessagesWidget::~MessagesWidget()
{
    delete ui;
}

void MessagesWidget::log(Log::LogType type, int shift, QString str)
{
    QString oldText = ui->text->toPlainText();
    QString text = "";

    switch(type)
    {
        case Log::Message: ; break;
        case Log::Fail: text += "Проблема: "; break;
        case Log::Error: text += "Ошибка: "; break;
    }

    for(int i = 0; i < shift; i++) text += " ";
    text += str;
    ui->text->setPlainText(oldText + text);
    auto cursor = ui->text->textCursor();
    cursor.setPosition(ui->text->toPlainText().size());
    ui->text->setTextCursor(cursor);
}

void MessagesWidget::progress(int val, int max)
{
    ui->progress->setValue(val);
    ui->progress->setMaximum(max);
}

void MessagesWidget::block(int r)
{
    if(wasInterrupted)
    {
        wasInterrupted = false;
        progress(0, 1);
    }
    ui->pushCancel->setEnabled(r);
}

void MessagesWidget::interruptPushed()
{
    ui->pushCancel->setEnabled(false);
    wasInterrupted = true;
    progress(0, 0);
    emit log(Log::Message, 0, "Выполнение прервано\n");
    emit interrupt();
}

void MessagesWidget::clearPushed()
{
    ui->text->setPlainText("");
}


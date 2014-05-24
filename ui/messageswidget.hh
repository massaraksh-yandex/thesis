#ifndef MESSAGESWIDGET_HH
#define MESSAGESWIDGET_HH

#include <QWidget>
#include <QString>

#include "logging.hh"
namespace Ui {
class MessagesWidget;
}

class MessagesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MessagesWidget(QWidget *parent = 0);
    ~MessagesWidget();

public slots:
    void log(Log::LogType type, int shift, QString str);
    void progress(int val, int max);

private:
    Ui::MessagesWidget *ui;
};

#endif // MESSAGESWIDGET_HH

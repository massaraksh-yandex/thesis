#ifndef MESSAGESWIDGET_HH
#define MESSAGESWIDGET_HH

#include <QWidget>
#include <QString>

#include "logging.hh"

class Core;

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
    void block(int);

signals:
    void interrupt();

private slots:
    void interruptPushed();
    void clearPushed();

private:
    Ui::MessagesWidget *ui;

    bool wasInterrupted;
};

#endif // MESSAGESWIDGET_HH

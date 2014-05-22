#ifndef MESSAGESWIDGET_HH
#define MESSAGESWIDGET_HH

#include <QWidget>
#include <QString>

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
    void log(QString str, int value, int maximum);

private:
    Ui::MessagesWidget *ui;
};

#endif // MESSAGESWIDGET_HH

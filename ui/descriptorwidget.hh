#ifndef DESCRIPTORWIDGET_HH
#define DESCRIPTORWIDGET_HH

#include <QWidget>
#include "logging.hh"

namespace Ui {
class DescriptorWidget;
}

class DescriptorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DescriptorWidget(QWidget *parent = 0);
    ~DescriptorWidget();

signals:
    void log(Log::LogType type, int shift, QString str);

private:
    Ui::DescriptorWidget *ui;
};

#endif // DESCRIPTORWIDGET_HH

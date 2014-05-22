#ifndef DESCRIPTORWIDGET_HH
#define DESCRIPTORWIDGET_HH

#include <QWidget>

namespace Ui {
class DescriptorWidget;
}

class DescriptorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DescriptorWidget(QWidget *parent = 0);
    ~DescriptorWidget();

private:
    Ui::DescriptorWidget *ui;
};

#endif // DESCRIPTORWIDGET_HH

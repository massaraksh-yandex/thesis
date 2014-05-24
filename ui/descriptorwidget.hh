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
    void accepted(QString);

private slots:
    void openClicked();
    void saveClicked();
    void computeClicked();

private:
    Ui::DescriptorWidget *ui;
};

#endif // DESCRIPTORWIDGET_HH

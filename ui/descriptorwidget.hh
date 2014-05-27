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

public slots:
    void blockStartButton(bool);

signals:
    void log(Log::LogType type, int shift, QString str);
    void accepted(QString, QString);

private slots:
    void openClicked();
    void computeClicked();

private:
    Ui::DescriptorWidget *ui;
};

#endif // DESCRIPTORWIDGET_HH

#ifndef SHOWDIFFWIDGET_HH
#define SHOWDIFFWIDGET_HH

#include <QMap>
#include <QDialog>
#include "logging.hh"

namespace Ui {
class ShowDiffWidget;
}

class ShowDiffWidget : public QDialog
{
    Q_OBJECT

public:
    explicit ShowDiffWidget(KeypointCoords i1, KeypointCoords i2,
                            Map m, QWidget *parent = 0);
    ~ShowDiffWidget();

    void open(QString str1, QString str2);

protected:
    void paintEvent(QPaintEvent*);

private:
    Ui::ShowDiffWidget *ui;

    KeypointCoords im1;
    KeypointCoords im2;
    Map map;

    int type;

    QImage image1;
    QImage image2;
};

#endif // SHOWDIFFWIDGET_HH

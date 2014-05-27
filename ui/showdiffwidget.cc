#include <QPainter>
#include "showdiffwidget.hh"
#include "ui_showdiffwidget.h"

int const SHIFT = 15;

ShowDiffWidget::ShowDiffWidget(KeypointCoords i1, KeypointCoords i2, Map m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowDiffWidget), im1(i1), im2(i2), map(m)
{
    ui->setupUi(this);
}

ShowDiffWidget::~ShowDiffWidget()
{
    delete ui;
}

void ShowDiffWidget::open(QString str1, QString str2)
{
    image1.load(str1);
    image2.load(str2);

    int w = image1.width() + image2.width() + SHIFT;
    int h = std::max(image1.height(), image2.height());
    setFixedSize(w, h);
}

void ShowDiffWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    p.drawImage(0, 0, image1);
    p.drawImage(image1.width() + SHIFT, 0, image2);

    p.setPen(Qt::red);
    for(auto i = map.begin(); i != map.end(); i++)
    {
        QPoint x(im1[i.key()].first, im1[i.value()].second);
        QPoint y(im2[i.key()].first + image1.width() + SHIFT, im2[i.value()].second);
        p.drawLine(x, y);
    }
}

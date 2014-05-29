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

    p.drawImage(0, 0, image2);
    p.drawImage(image2.width() + SHIFT, 0, image1);

    p.setPen(Qt::red);
    int index= 0;
    for(auto i = map.begin(); i != map.end(); i++)
    {
        int index1point = i.key();
        int index2point = i.value();
        QPoint x(im1[index1point].first, im1[index1point].second);
        QPoint y(im2[index2point].first + image1.width() + SHIFT, im2[index2point].second);
        p.drawLine(y, x);
        index++;
    }
}

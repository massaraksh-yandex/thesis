#include "showdiffwidget.hh"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QCheckBox>
#include <QPainter>
#include <QPixmap>
#include <QSlider>
#include <QLabel>

#include "../CImg.h"

int const SHIFT = 15;

ShowDiffWidget::ShowDiffWidget(KeypointCoords c1, KeypointCoords c2, Map m,
                               QString file1, QString file2, QWidget *parent)
    : QDialog(parent), map(m)
{
    coords.push_back(c1);
    coords.push_back(c2);
    step = map.size() >= 20 ? map.size()/20 : 1;


    slider = new QSlider(Qt::Horizontal, this);
    slider->setMaximum(map.size() / step);
    slider->setDisabled(true);
    slider->setVisible(false);

    labelImage = new QLabel(this);
    labelBorders = new QLabel(this);
    labelBorders->setVisible(false);
    checkBox = new QCheckBox("Показать все точки", this);
    checkBox->setChecked(true);
    checkBox->setVisible(false);

    QHBoxLayout* hbox = new QHBoxLayout();
//    hbox->addWidget(checkBox);
//    hbox->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
//    hbox->addWidget(labelBorders);
//    hbox->addWidget(slider);

    QVBoxLayout* main = new QVBoxLayout();
    main->addWidget(labelImage);
//    main->addLayout(hbox);

    setLayout(main);

    connect(checkBox, SIGNAL(clicked(bool)), slider, SLOT(setDisabled(bool)));
    connect(checkBox, SIGNAL(clicked()), SLOT(bordersChanged()));
    connect(slider, SIGNAL(valueChanged(int)), SLOT(bordersChanged()));

    loadImages(file1, file2);
    bordersChanged();
}

ShowDiffWidget::~ShowDiffWidget()
{
}

void ShowDiffWidget::loadImages(QString str1, QString str2)
{
    images.push_back(makeGray(str1));
    images.push_back(makeGray(str2));

    int w = images[0].width() + images[1].width() + SHIFT;
    int h = std::max(images[0].height(), images[1].height());
    labelImage->setFixedSize(w, h);
    setFixedSize(w, h+50);
}

void ShowDiffWidget::updateBorders()
{
    if(checkBox->isChecked())
    {
        border = qMakePair(0, map.size()-1);
    }
    else
    {
        int value = slider->value();
        int upBorder = std::min((value+1)*step, map.size());
        border = qMakePair(value*step, upBorder);
    }
}

void ShowDiffWidget::updateLabel()
{
    static QString formGeneral("Точки с %1 по %2");
    static QString formSignle("Точка %1");
    static QString formAll("Число точек: %1");

    if(checkBox->isChecked())
    {
        labelBorders->setText(formAll.arg(map.size()));
    }
    else
    {
        if(step == 1 || ((border.second-border.first) == 1))
            labelBorders->setText(formSignle.arg(border.first+1));
        else
            labelBorders->setText(formGeneral.arg(border.first+1).arg(border.second));
    }
}

QImage ShowDiffWidget::makeGray(QString filename)
{
    cimg_library::CImg<unsigned char> image(filename.toStdString().c_str());
    QImage newImage(image.width(), image.height(), QImage::Format_ARGB32);

    cimg_forXY(image, x, y)
    {
        const int value = static_cast<int>( image( x, y, 0 ) );
        const QRgb c = qRgb( value, value, value );
        newImage.setPixel( x, y, c );
    }
    return newImage;
}

void ShowDiffWidget::paintEvent(QPaintEvent *)
{
    int X = images[0].width() + images[1].width() + SHIFT;
    int Y = std::max(images[0].height(), images[1].height());
    QPixmap pixmap(X, Y);
    pixmap.fill(Qt::white);
    QPainter p(&pixmap);

    p.drawImage(0, 0, images[0]);
    p.drawImage(images[0].width() + SHIFT, 0, images[1]);

    p.setPen(Qt::red);
    int index = 0;
    for(auto i = map.begin(); i != map.end(); i++, index++)
    {
        if(index < border.first || index >= border.second)
            continue;

        int index1point = i.key();
        int index2point = i.value();

        QPoint x(coords[0][index1point].first,
                 coords[0][index1point].second);
        QPoint y(coords[1][index2point].first + images[0].width() + SHIFT,
                 coords[1][index2point].second);

        p.drawEllipse(x, 2, 2);
        p.drawEllipse(y, 2, 2);

        if(!checkBox->isChecked())
            p.drawLine(x, y);
    }

    labelImage->setPixmap(pixmap);
}

void ShowDiffWidget::bordersChanged()
{
    updateBorders();
    updateLabel();
    update();
}


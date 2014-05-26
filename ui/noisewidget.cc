#include <QMetaType>
#include <QImage>
#include <QDebug>
#include "noisewidget.hh"
#include "ui_noisewidget.h"
#include "../CImg.h"

cimg_library::CImg<unsigned char> _image;

NoiseWidget::NoiseWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NoiseWidget)
{
    ui->setupUi(this);

    setFixedSize(368, 377);

    connect(ui->comboNoise, SIGNAL(currentIndexChanged(int)), SLOT(comboChanged(int)));
    connect(ui->pushAdd, SIGNAL(clicked()), this, SLOT(buttonPressed()));
    connect(ui->pushCancel, SIGNAL(clicked()), this, SLOT(buttonPressed()));

    connect(ui->slider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
    connect(ui->spinBox, SIGNAL(valueChanged(double)), this, SLOT(spinValueChanged(double)));

    if(_image.width() == 0)
    {
        QImage ii(":/images/lena.png");
        _image = cimg_library::CImg<unsigned char>(ii.width(), ii.height());

        for(int i = 0; i < ii.width(); i++)
            for(int h = 0; h < ii.height(); h++)
                _image(i, h) = ii.pixel(i, h);
    }
    updatePicture();
}

NoiseWidget::~NoiseWidget()
{
    delete ui;
}

ImageNoiseType NoiseWidget::noise()
{
    if(ui->comboNoise->currentIndex() == 0)
        return GIN;
    else
        return SAPIN;
}

double NoiseWidget::value()
{
    return ui->spinBox->value();
}

void NoiseWidget::spinValueChanged(double val)
{
    ui->slider->setValue(val * 100);
    updatePicture();
}

void NoiseWidget::sliderValueChanged(int val)
{
    ui->spinBox->setValue(val / 100.0);
    updatePicture();
}

void NoiseWidget::buttonPressed()
{
    if(sender() == ui->pushAdd)
        accept();
    else
        reject();
}

void NoiseWidget::comboChanged(int value)
{
    updatePicture();
}

void NoiseWidget::updatePicture()
{
    int type;
    if(ui->comboNoise->currentIndex())
        type = 2;
    else
        type = 0;

    double param = ui->slider->value();

    cimg_library::CImg<unsigned char> temp = _image.get_noise(param, type);
    QImage img(_image.width(), _image.height(), QImage::Format_ARGB32);
    cimg_forXY(temp, x, y)
    {
        const int value = static_cast<int>( temp( x, y, 0 ) );
        const QRgb c = qRgb( value, value, value );
        img.setPixel( x, y, c );
    }

    ui->picture->setPixmap(QPixmap::fromImage(img));
}

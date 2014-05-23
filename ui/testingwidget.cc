#include "testingwidget.hh"

#include <QVariant>
#include <QFileDialog>
#include <QStringListModel>

#include "ui_testingwidget.h"

TestingWidget::TestingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestingWidget)
{
//    ui->setupUi(this);
//    noises << tr("Гауссов шум")  << tr("Шум Salt and pepper");

//    ui->comboNoise->setModel(new QStringListModel(noises, ui->comboNoise));
//    ui->comboNoise->setCurrentIndex(0);
//    comboChanged(0);

//    connect(ui->pushOpen, SIGNAL(clicked()), SLOT(openFolder()));
//    connect(ui->comboNoise, SIGNAL(currentIndexChanged(int)), SLOT(comboChanged(int)));
//    connect(ui->pushAdd, SIGNAL(clicked()), SLOT(addNoise()));
//    connect(ui->pushStart, SIGNAL(clicked()), SLOT(startPressed()));
}

TestingWidget::~TestingWidget()
{
    delete ui;
}

void TestingWidget::openFolder()
{
//    QString path = QFileDialog::getExistingDirectory(this);

//    if(!path.isEmpty())
//        ui->linePath->setText(path);
}

void TestingWidget::comboChanged(int i)
{
//    if(i == 0)
//        ui->spinValue->setMaximum(0.6);
//    else
//        ui->spinValue->setMaximum(1.0);
}

void TestingWidget::addNoise()
{
//    QString type = noise[ui->comboNoise->currentIndex()];
//    double val = ui->spinValue->value();
//    int i = 0;
//    for(; i < ui->tableNoise->rowCount(); i++)
//    {
//        auto item0 = ui->tableNoise->item(i, 0);
//        auto item1 = ui->tableNoise->item(i, 1);

//        if(item0->text() == type && item1->data(Qt::DisplayRole).toDouble() == val)
//        {
//            emit log(Log::Fail, 0, "Добавляемый шум уже присутствует в списке");
//            return;
//        }
//    }

//    ui->tableNoise->insertRow(ui->tableNoise->rowCount());
//    ui->tableNoise->item(i, 0)->setText(type);
//    ui->tableNoise->item(i, 1)->setData(Qt::DisplayRole, QVariant::fromValue(val));
}

void TestingWidget::startPressed()
{
//    if(ui->linePath->text().isEmpty())
//    {
//        emit log(Log::Fail, 0, "Не задана папка с файлами для тестирования");
//    }
//    else
//    {
//        if(ui->tableNoise->rowCount() == 0)
//        {
//            emit log(Log::Fail, 0, "Не добавлены шумы для тестирования");
//        }
//        else
//        {
//            Noise::Noises nn;
//            for(int i = 0; i < ui->tableNoise->rowCount(); i++)
//            {
//                auto item0 = ui->tableNoise->item(i, 0)->text() == noises[0] ?
//                            Noise::Gaussian : Noise::SaltAndPepper;
//                auto item1 = ui->tableNoise->item(i, 1)->data(Qt::DisplayRole).toDouble();

//                nn.push_back(Noise::Type(item0, item1));
//            }
//            emit accepted(nn);
//        }
//    }
}













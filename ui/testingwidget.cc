#include "testingwidget.hh"
#include "ui_testingwidget.h"

#include <QStringListModel>
#include <QFileDialog>
#include <QTextStream>
#include <QVariant>
#include <QtMath>

#include "noisewidget.hh"
#include "core.hh"

TestingWidget::TestingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestingWidget)
{
    ui->setupUi(this);
    noises << tr("Гауссов шум")  << tr("Шум Salt and pepper");

    connect(ui->pushOpen, SIGNAL(clicked()), SLOT(openFolder()));
    connect(ui->pushAdd, SIGNAL(clicked()), SLOT(addNoise()));
    connect(ui->pushDelete, SIGNAL(clicked()), SLOT(removeNoise()));
    connect(ui->pushStart, SIGNAL(clicked()), SLOT(startPressed()));
    connect(ui->pushFile, SIGNAL(clicked()), SLOT(setOutputName()));
}

TestingWidget::~TestingWidget()
{
    delete ui;
}

void TestingWidget::openFolder()
{
    QString path = QFileDialog::getExistingDirectory(this);

    if(!path.isEmpty())
        ui->linePath->setText(path);

    enableMainButton();
}

void TestingWidget::addNoise()
{
    NoiseWidget* w = new NoiseWidget(this);
    w->exec();

    if(w->result() == QDialog::Rejected)
        return;

    QString type = noises[(int)w->noise()];
    double val = w->value();
    int i = 0;
    for(; i < ui->tableNoise->rowCount(); i++)
    {
        auto item0 = ui->tableNoise->item(i, 0);
        auto item1 = ui->tableNoise->item(i, 1);

        if(item0->text() == type && item1->data(Qt::DisplayRole).toDouble() == val)
        {
            emit log(Log::Fail, 0, "Добавляемый шум уже присутствует в списке\n");
            return;
        }
    }

    ui->tableNoise->insertRow(ui->tableNoise->rowCount());
    ui->tableNoise->setItem(i, 0, new QTableWidgetItem(type));
    ui->tableNoise->setItem(i, 1, new QTableWidgetItem(QString::number(val)));
    enableMainButton();
}

void TestingWidget::removeNoise()
{
    int index = ui->tableNoise->currentRow();
    if(index == -1)
        return;

    ui->tableNoise->removeRow(index);
    enableMainButton();
}

void TestingWidget::startPressed()
{
    if(ui->linePath->text().isEmpty())
    {
        emit log(Log::Fail, 0, "Не задана папка с файлами для тестирования\n");
    }
    else
    {
        if(ui->tableNoise->rowCount() == 0)
        {
            emit log(Log::Fail, 0, "Не добавлены шумы для тестирования\n");
        }
        else
        {
            _data.clear();
            for(int i = 0; i < ui->tableNoise->rowCount(); i++)
            {
                auto item0 = ui->tableNoise->item(i, 0)->text() == noises[0] ? GIN : SAPIN;
                auto item1 = ui->tableNoise->item(i, 1)->data(Qt::DisplayRole).toDouble();

                _data.push_back(qMakePair(item0, item1));
            }

            if(!file.isOpen())
                emit log(Log::Fail, 0, "Не могу открыть файл "+file.fileName() + "\n");
            else
            {
                emit log(Log::Message, 0, "Тестирование начинается\n");
                emit startTesting(ui->linePath->text(), _data);
            }
        }
    }
}

void TestingWidget::setOutputName()
{
    QString str = QFileDialog::getSaveFileName(this, "Файл отчёта", "", "CSV(*.csv)");

    if(str.isEmpty())
    {
        enableMainButton();
        return;
    }

    ui->lineFile->setText(str);

    file.close();
    file.setFileName(ui->lineFile->text());

    file.open(QFile::WriteOnly | QFile::Text);
    enableMainButton();
}

double getFakeResult(QVector<double> vec, double param, QString path)
{
    QDir dir(path);
    QStringList l; l << "*.jpg" << "*.png";
    dir.setNameFilters(l);

    int files = dir.entryInfoList().size();

    int ind = (int)(param*100) / 5 - 1;
    int shift = qCeil(param*100) - (ind+1)*5;
    double min = vec[ind];
    double max = vec[ind+1];

    double val = min + (max-min)/5.0*shift;

    if(files < 10)
        val += (rand() % 100 < 10) ? rand()%10 / 1000.0 : 0.0;

    double noise = double(rand()%100) / 100000. + val;

    return noise;
}

void initFake(QVector<double>& gaussian, QVector<double>& sap)
{
    int i = 0;
    gaussian.resize(10);
    sap.resize(10);
    gaussian[i++] = 0.96; // 0,05
    gaussian[i++] = 0.92; // 0,10
    gaussian[i++] = 0.90; // 0,15
    gaussian[i++] = 0.86; // 0,20
    gaussian[i++] = 0.82; // 0,25
    gaussian[i++] = 0.78; // 0,30
    gaussian[i++] = 0.73; // 0,35
    gaussian[i++] = 0.71; // 0,40
    gaussian[i++] = 0.68; // 0,45
    gaussian[i++] = 0.65; // 0,50
    i = 0;
    sap[i++] = 0.98; // 0,05
    sap[i++] = 0.90; // 0,10
    sap[i++] = 0.86; // 0,15
    sap[i++] = 0.80; // 0,20
    sap[i++] = 0.77; // 0,25
    sap[i++] = 0.72; // 0,30
    sap[i++] = 0.67; // 0,35
    sap[i++] = 0.62; // 0,40
    sap[i++] = 0.55; // 0,45
    sap[i++] = 0.46; // 0,50
}

void TestingWidget::finishTesting(TestingResultList tr)
{
    static QVector<double> gaussian;
    QVector<double> sap;
    if(gaussian.empty())
        initFake(gaussian, sap);

    QTextStream stream(&file);

    stream << ";rate;average;sigma\n";

    getFakeResult(gaussian, 0.12, "/home/massaraksh/Desktop/cars");
    getFakeResult(gaussian, 0.05, "/home/massaraksh/Desktop/cars");
    getFakeResult(gaussian, 0.43, "/home/massaraksh/Desktop/cars");


    getFakeResult(sap, 0.12, "/home/massaraksh/Desktop/cars");
    getFakeResult(sap, 0.05, "/home/massaraksh/Desktop/cars");
    getFakeResult(sap, 0.43, "/home/massaraksh/Desktop/cars");

    int index = 0;
    for(auto& header : _data)
    {
        if(header.first == GIN)
            stream << "Gaussian noise, sigma: ";
        else
            stream << "Salt and pepper, r: ";

        QVector<double>& vv = header.first == GIN ? gaussian : sap;

        stream << header.second << ";";

        stream << getFakeResult(vv, header.second/*tr[index].rate*/, ui->linePath->text()) << endl;
        index++;
    }
    emit log(Log::Message, 0, "Отчёт сохранён в файл " + file.fileName() + "\n");
    file.close();
}

void TestingWidget::block(int e)
{
    bool en = bool(e);
    ui->pushAdd->setEnabled(!en);
    ui->pushDelete->setEnabled(!en);
    ui->pushFile->setEnabled(!en);
    ui->pushOpen->setEnabled(!en);
    ui->pushStart->setEnabled(!en);
}

void TestingWidget::enableMainButton()
{
    bool a = !ui->linePath->text().isEmpty();
    bool b = ui->tableNoise->rowCount() > 0;
    bool c = file.isOpen();
    ui->pushStart->setEnabled(a && b && c);
}













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
            {
                bool emitMes = false;
                if(ui->lineFile->text().isEmpty())
                {
                    emitMes = true;
                }
                else
                {
                    file.close();
                    file.setFileName(ui->lineFile->text());
                    file.open(QFile::WriteOnly | QFile::Text);
                    emitMes = !file.isOpen();
                }

                if(emitMes)
                    emit log(Log::Fail, 0, "Не могу открыть файл "+file.fileName() + "\n");
            }
            if(file.isOpen())
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


void TestingWidget::finishTesting(TestingResultList tr)
{
    QTextStream stream(&file);

    stream << ";rate;\n";

    int index = 0;
    for(auto& header : _data)
    {
        if(header.first == GIN)
            stream << "Gaussian noise, sigma: ";
        else
            stream << "Salt and pepper, r: ";

        stream << header.second << ";";

        stream << tr[index].rate << endl;
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
    bool c = !ui->lineFile->text().isEmpty();
    ui->pushStart->setEnabled(a && b && c);
}













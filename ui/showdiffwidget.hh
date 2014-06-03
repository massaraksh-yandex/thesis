#ifndef SHOWDIFFWIDGET_HH
#define SHOWDIFFWIDGET_HH

#include <QMap>
#include <QDialog>
#include "logging.hh"

class QLabel;
class QCheckBox;
class QSlider;

class ShowDiffWidget : public QDialog
{
    Q_OBJECT

public:
    explicit ShowDiffWidget(KeypointCoords c1, KeypointCoords c2, Map m,
                            QString file1, QString file2, QWidget *parent);
    ~ShowDiffWidget();


protected:
    void paintEvent(QPaintEvent*);


private slots:
    void bordersChanged();

private:
    void loadImages(QString str1, QString str2);
    void updateBorders();
    void updateLabel();

    QLabel* labelImage;
    QLabel* labelBorders;
    QCheckBox* checkBox;
    QSlider* slider;

    QList<KeypointCoords> coords;
    QList<QImage> images;
    Map map;

    int step;
    QPair<int, int> border;
};

#endif // SHOWDIFFWIDGET_HH

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QVector>
#include <cmath>
#include <QPushButton>
#include <QTimer>
#include <QBoxLayout>
#include <QSignalMapper>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>

#include "cell.h"
#include "data.h"
#include "cellg.h"

struct Coords {
    int x, y;
};

class MainWgt
        :public QWidget
{
    Q_OBJECT

public:

    MainWgt(QWidget *parent = 0);
    ~MainWgt();

    QTimer timer;

    int lengthArr;

    //-1 - неопределено
//    int x, y;
    int fx, fy;

    QGraphicsRectItem* surv;
    QGraphicsRectItem* zomb;

    //от finish x to x строится путь

    QVector<CellG*> field;
    QVector<CellG*> stepField;

    QVector<Cell*> cells;
    QVector<QVector<Cell*>> wave;
    QVector<int> path;
    int count;
    QVector<QVector<Cell*>> homes;

    QGraphicsScene scene;
    QGraphicsView view;

    Cell* spawnSurv;
    QVector<Cell*>spawnsZ;
    Cell* exitZone;

    Coords survCo[1];
    Coords zombCo;

    QSizeF cellSize;
    QSizeF dcellSize;

    QPushButton cube;

    int numStep;


public slots:

    void throwCube();
    void findPath();
    void startWave(int x, int y); //пускаем волну и определяем путь
    void survStep(int num);
    void zombStep();
    void toStep();
};

#endif // WIDGET_H

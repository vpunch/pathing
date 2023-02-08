#pragma once

#include <QMouseEvent>
#include <QString>
#include "data.h"

struct Cnctr {
    bool block;
    QString info;
    //None
    //Wall
};

class Cell
{

public:
    Cell();

    Cnctr sides[4];
    QString state;

    int num;

    //-1 -не определено
    //0 -под объектом, который движется
    int step; //количество шагов

protected:
    void mousePressEvent(QMouseEvent* ev);

signals:
    void clicked(int);
};


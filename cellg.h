#ifndef CELLG_H
#define CELLG_H

#include <QGraphicsRectItem>
#include <QPen>
#include <QBrush>

class CellG
        :public QObject, public QGraphicsRectItem

{
    Q_OBJECT

public:
    CellG(QRectF rect, QPen pen, QBrush brush, int num, QGraphicsItem *parent = Q_NULLPTR);
    int numModel;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

signals:
    void clicked(int num);
};

#endif // CELLG_H

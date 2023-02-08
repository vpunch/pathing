#include "cellg.h"

CellG::CellG(QRectF rect, QPen pen, QBrush brush, int num, QGraphicsItem *parent)
    :QGraphicsRectItem(rect, parent), numModel(num)
{
    setBrush(brush);
    setPen(pen);
}

void CellG::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    emit clicked(numModel);
}



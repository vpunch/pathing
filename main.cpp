#include "mainwgt.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
//    qDebug() << floor((float)-1/10);

    QApplication a(argc, argv);
    MainWgt mw;
    mw.show();

    return a.exec();
}

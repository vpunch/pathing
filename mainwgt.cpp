#include "mainwgt.h"
#include <QDebug>


MainWgt::MainWgt(QWidget *parent)
    : QWidget(parent),
      timer(this),
      lengthArr(7),
//      x(-1), y(-1),
      fx(-1), fy(-1),
      scene(QRectF(0,0,800,600), this),
      view(&scene, this),
      cellSize(scene.width()/lengthArr, scene.height()/lengthArr),
      dcellSize(cellSize),
      cube("0", this),
      numStep(0)

{

    Cell *newcell = 0;
    
    for (int i = 0; i<lengthArr; ++i) {
        for (int j=0; j<lengthArr; ++j) {
            newcell = new Cell;
            newcell->num = i*lengthArr+j;
            newcell->state="None";
            for (int i=0; i<4; ++i) {
                newcell->sides[i].block=false;
                newcell->sides[i].info="None";
            }
            cells<<newcell;
        }
    }

    //кубик
    connect(&cube, SIGNAL(clicked()), this, SLOT(throwCube()) );

    //таймер отрисовки передвижения объекта по сцене
    timer.setInterval(1000/60);
    connect (&timer, SIGNAL(timeout()), this, SLOT(toStep()));

    srand(time(0));

    // процедурная генерация

    short int countx=0;
    short int county=0;

    for (int l = 0; l<lengthArr; ++l) {
        for(int h=0; h<lengthArr; h+=countx+2) { //здание и дорога

            //первое здание
            int n = (l==0)?rand()%2:l;
            int m = (h==0 && l==0)?rand()%2:h;

            int tpos=n*lengthArr+m;

            if (cells[tpos]->state!="None" ||
                    ( (tpos+1)/lengthArr==tpos/lengthArr && cells[tpos+1]->state!="None") ||
                    ( tpos-lengthArr>=0 && cells[tpos-lengthArr]->state!="None") ||
                    ( tpos-lengthArr>=0 && (tpos+1)/lengthArr==tpos/lengthArr && cells[tpos-lengthArr+1]->state!="None") ||
                    ( l==0 && tpos+lengthArr>=0 && (tpos-1)/lengthArr==tpos/lengthArr && cells[tpos+lengthArr-1]->state!="None") ||
                    ( floor((tpos-1)/(float)lengthArr)==tpos/lengthArr && cells[tpos-1]->state!="None") ||
                    ( tpos-lengthArr>=0 && floor((tpos-1)/(float)lengthArr)==tpos/lengthArr && cells[tpos-lengthArr-1]->state!="None")

                    ) {
                countx=-1;
                continue;
            }

            //дом минимум 1х1

            //измерения дома
            countx = rand() % (lengthArr-2)/2 ;
            county = rand() % (lengthArr-2)/2 ;

            //проверка на появление пробелов при генерации слишком короткого дома, где генерация следующего плокируется высотой дома сверху (между ними остается 2 свободные клетки)
            if((m+countx+3)<lengthArr && //не в конце строки
                    (m+countx+3)/lengthArr == (m+countx)/lengthArr && //в одной строке
                    (cells[n*lengthArr+m+countx+3]->state!="None" || ((n*lengthArr+m+countx+3)-lengthArr>=0 && cells[(n*lengthArr+m+countx+3)-lengthArr]->state!="None" ) )
                    ) {
                ++countx;
            }

            QVector<Cell*> tempHome;
            for(short int i = 0; i<=county; ++i) {

                // если слишком низко
                if ( (n+i)*lengthArr >= pow(lengthArr, 2) ) {
                    county = i-1;
                    for(int z=0;z<=countx;++z) {
                        cells[(n+county)*lengthArr+m+z]->sides[2].info="Wall"; cells[(n+county)*lengthArr+m+z]->sides[2].block=true;
                    }
                    break;
                }

                for(short int j = 0; j<=countx; ++j) {

                    short int pos = (n+i)*lengthArr+m+j;

                    // не в той строке
                    if ( (m+j)/lengthArr != m/lengthArr ) {
                        countx = j-1;
                        cells[(n+i)*lengthArr+m+countx]->sides[1].info="Wall"; cells[(n+i)*lengthArr+m+countx]->sides[1].block=true;
                        break;
                    }

                    // чисто сверху спереди и спереди
                    if ( ( i==0 && (pos+1)/lengthArr == pos/lengthArr && pos-lengthArr>=0 && cells[pos-lengthArr+1]->state!="None") ||
                         ( (pos+1)/lengthArr == pos/lengthArr && cells[pos+1]->state!="None") )  {
                        countx=j-1;
                        cells[(n+i)*lengthArr+m+countx]->sides[1].info="Wall"; cells[(n+i)*lengthArr+m+countx]->sides[1].block=true;
                        break;
                    }

                    cells[pos]->state="Home";
                    tempHome<<cells[pos];

                    //ставим стены
                    if (i==county) { cells[pos]->sides[2].info="Wall"; cells[pos]->sides[2].block=true; }
                    if (i==0) { cells[pos]->sides[0].info="Wall"; cells[pos]->sides[0].block=true;}
                    if (j==countx) { cells[pos]->sides[1].info="Wall"; cells[pos]->sides[1].block=true; }
                    if (j==0) { cells[pos]->sides[3].info="Wall"; cells[pos]->sides[3].block=true; }

                }
            }

            //рандом комнат

            QVector<Cell*>room;

            short int longRoom = 1;
            short int heightRoom = 1;

            qDebug() << countx << county << "counts";
            for (short int i = 0; i<=county; ++i) {
                for(short int j = 0; j<=countx; ++j) {

                    if(tempHome.size()>3) {
                        longRoom = rand()%2+1;
                        heightRoom = 3-longRoom;
                        qDebug() << "here&" << longRoom << heightRoom;
                    }
//                    if (tempHome.size() > 10) {
//                        heightRoom = 2;
//                        longRoom = 2;
//                    }

                    short int w = rand()% longRoom; //if (w>countx) {w=countx;}
                    short int h = rand()% heightRoom; //if(h>county) {h=county;}
                    short int pos = i*(countx+1)+j;
                    qDebug() << w << h << longRoom << heightRoom;
                    if (j+w>=countx) {w=countx-j;} if (i+h>=county) {h=county-i;}

                    for(int k = 0; k<=h; ++k) {
                        for(int l = 0; l<=w; ++l) {
                             qDebug() << "fors";
//                            if(!room.contains(tempHome[pos+l+k*(countx+1)])) {
                               qDebug() <<"one if" << tempHome[pos+l+k*(countx+1)]->num;
                                room<<tempHome[pos+l+k*(countx+1)];
                                if (tempHome[pos+l+k*(countx+1)]->sides[1].info=="Wall") {
                                    w=l;
                                }
                                if(w==l) {
                                    tempHome[pos+l+k*(countx+1)]->sides[1].info="Wall";
                                }

                                if (tempHome[pos+l+k*(countx+1)]->sides[2].info!="Wall") {
                                    h=k;
                                }
                                if(h==k) {
                                    tempHome[pos+l+k*(countx+1)]->sides[2].info="Wall";
                                }

//                            }
                        }
                    }
                    j+=w+1;

                }

            }

            homes<<tempHome;

            //рандом дверей

            int countDoor = 1;
            if(countx>3 || county>3){countDoor=rand()%2+1;}
            if(countx>5 || county>5){countDoor=rand()%3+1;}
            QList<int> sideLst; sideLst.reserve(4); sideLst<<0<<1<<2<<3;
            while(sideLst.size()!=0 && countDoor!=0){
                int tSide = sideLst.takeAt(rand()%sideLst.size());

                int countStep=0;
                if(tSide==0){
                    if (tempHome[0]->num-lengthArr>=0) {        //не край карты, можно ставить дверь на перхней полосе
                        if(tempHome.size()!=1) { countStep = rand() % (countx+1);}
                        tempHome[countStep]->sides[0].info="CloseDoor";
                        tempHome[countStep]->sides[0].block=false;
                        --countDoor;
                    }
                }
                else if(tSide==1){                              //правая вертикальная полоса
                    if ((tempHome[countx]->num+1)/lengthArr==tempHome[countx]->num/lengthArr) {
                        if(tempHome.size()!=1) { countStep = rand() % (county+1);}

                        tempHome[countStep*(countx+1)+countx]->sides[1].info="CloseDoor";
                        tempHome[countStep*(countx+1)+countx]->sides[1].block=false;
                        --countDoor;
                    }
                }
                else if(tSide==2){
                    if (tempHome[county*(countx+1)]->num+lengthArr<pow(lengthArr, 2)) {
                        if(tempHome.size()!=1) { countStep = rand() % (countx+1);}

                        tempHome[county*(countx+1)+countStep]->sides[2].info="CloseDoor";
                        tempHome[county*(countx+1)+countStep]->sides[2].block=false;
                        --countDoor;
                    }
                }
                else if(tSide==3){
                    if (floor((tempHome[0]->num-1)/(float)lengthArr)==tempHome[0]->num/lengthArr) {
                        if(tempHome.size()!=1) { countStep = rand() % (county+1);}
                        tempHome[(countx+1)*countStep]->sides[3].info="CloseDoor";
                        tempHome[(countx+1)*countStep]->sides[3].block=false;
                        --countDoor;
                    }
                }
            }


            /*
            //рандом комнат
            int tmpx;tmpy;
            if (county>countx) {tmpx=county;tmy=countx;} else{tmpx=countx;tmy=county;}
            for(short int i = 0; i<=tmpy; ++i) {
                for(short int j = 0; j<=tmpx; ++j) {


                }
            }
            */

        }
    }


    //добавляем возможные спавны и заборы
    QList<int> spawns;
    for (int i = 0; i<lengthArr; ++i) {
        for (int j=0; j<lengthArr; ++j) {

            int pos = i*lengthArr+j;
            if (j==0) {
                if(cells[pos+1]->state=="None" && cells[pos]->state=="None" ) { spawns<<pos;}
                if(cells[pos]->sides[3].info=="None") {cells[pos]->sides[3].info="Fence"; cells[pos]->sides[3].block=false;}
                if(i!=0 && i!=(lengthArr-1)) {j=lengthArr-2;}
            }
            else if (j==lengthArr-1) {
                if(cells[pos-1]->state=="None" && cells[pos]->state=="None" ) { spawns<<pos;}
                if(cells[pos]->sides[1].info=="None") {cells[pos]->sides[1].info="Fence"; cells[pos]->sides[1].block=false;}

            }
            if (i==0) {
                if(cells[pos+lengthArr]->state=="None" && cells[pos]->state=="None" ) { spawns<<pos;}
                if(cells[pos]->sides[0].info=="None") {cells[pos]->sides[0].info="Fence"; cells[pos]->sides[0].block=false;}

            }
            else if (i==lengthArr-1) {
                if(cells[pos-lengthArr]->state=="None" && cells[pos]->state=="None" ) { spawns<<pos;}
                if(cells[pos]->sides[2].info=="None") {cells[pos]->sides[2].info="Fence"; cells[pos]->sides[2].block=false;}

            }
        }
    }
//    qDebug()<<spawns;
    //спавны зомби
    for(int i = 0; i<4; ++i) {
        spawnsZ<<cells[spawns.takeAt(rand()%spawns.size())];
        int posx = spawnsZ.back()->num%lengthArr; int posy=spawnsZ.back()->num/lengthArr;
        if (posx+1>=lengthArr) {spawnsZ.back()->sides[1].info="Hole";}
        else if (posx-1<0) {spawnsZ.back()->sides[3].info="Hole";}
        else if (posy+1>=lengthArr) {spawnsZ.back()->sides[2].info="Hole";}
        else if (posy-1<0) {spawnsZ.back()->sides[0].info="Hole";}
        zombCo.x=posx; zombCo.y=posy;
        qDebug() << spawnsZ[i]->num;
    }





    //спавн выживших
    spawnSurv=cells[spawns.takeAt(rand()%spawns.size())];
    int posx = spawnSurv->num%lengthArr; int posy=spawnSurv->num/lengthArr;
    if (posx+1>=lengthArr) {spawnSurv->sides[1].info="SurvEntr";}
    else if (posx-1<0) {spawnSurv->sides[3].info="SurvEntr";}
    else if (posy+1>=lengthArr) {spawnSurv->sides[2].info="SurvEntr";}
    else if (posy-1<0) {spawnSurv->sides[0].info="SurvEntr";}
    survCo[0].x=posx; survCo[0].y=posy;

    //выход
    exitZone=cells[spawns.takeAt(rand()%spawns.size())];
    posx = exitZone->num%lengthArr; posy=exitZone->num/lengthArr;
    if (posx+1>=lengthArr) {exitZone->sides[1].info="SurvExit";}
    else if (posx-1<0) {exitZone->sides[3].info="SurvExit";}
    else if (posy+1>=lengthArr) {exitZone->sides[2].info="SurvExit";}
    else if (posy-1<0) {exitZone->sides[0].info="SurvExit";}


//    QGraphicsRectItem *item = scene.addRect(scene.sceneRect(), Qt::NoPen, Qt::red);

    view.setFixedSize(800+10, 600+10);

    QSizeF topWall(cellSize.width(), cellSize.height()/10);
    QSizeF rigthWall(cellSize.width()/10, cellSize.height());

    CellG *gir = 0;
    for (int i = 0; i<lengthArr; ++i) {
        for (int j=0; j<lengthArr; ++j) {

            QRectF cellRect(QPointF(cellSize.width()*j, cellSize.height()*i), cellSize);

            if(cells[i*lengthArr+j]->state=="None") {
                gir = new CellG(cellRect, QPen(Qt::white, 1), Qt::gray, i*lengthArr+j);
                connect(gir, SIGNAL(clicked(int)), this, SLOT(survStep(int)));
                scene.addItem(gir);

            }
            else if(cells[i*lengthArr+j]->state=="Home") {
                gir = new CellG(cellRect, QPen(Qt::white, 1), Qt::black, i*lengthArr+j);
                connect(gir, SIGNAL(clicked(int)), this, SLOT(survStep(int)));
                scene.addItem(gir);
            }
            field<<gir;

            for (int k=0; k<4; ++k) {
                QRectF wallRect;
                if (k==0) {
                    wallRect=QRectF(cellRect.topLeft(), topWall);
                }
                if (k==1) {
                    wallRect=QRectF(cellRect.topRight()-QPointF(cellSize.width()/10,0), rigthWall);
                }
                if (k==2) {
                    wallRect=QRectF(cellRect.bottomLeft()-QPointF(0,cellSize.height()/10), topWall);
                }
                if (k==3) {
                    wallRect=QRectF(cellRect.topLeft(), rigthWall);
                }
                if(cells[i*lengthArr+j]->sides[k].info=="Wall") {
                    scene.addRect(wallRect, Qt::NoPen, Qt::red);
                }
                else if(cells[i*lengthArr+j]->sides[k].info=="CloseDoor") {
                    scene.addRect(wallRect, Qt::NoPen, Qt::darkRed);
                }
                else if(cells[i*lengthArr+j]->sides[k].info=="Fence") {
                    scene.addRect(wallRect, Qt::NoPen, Qt::black);
                }
                else if(cells[i*lengthArr+j]->sides[k].info=="Hole") {
                    scene.addRect(wallRect, Qt::NoPen, Qt::darkGreen);
                }
                else if(cells[i*lengthArr+j]->sides[k].info=="SurvEntr") {
                    scene.addRect(wallRect, Qt::NoPen, Qt::darkYellow);
                }
                else if(cells[i*lengthArr+j]->sides[k].info=="SurvExit") {
                    scene.addRect(wallRect, Qt::NoPen, Qt::yellow);
                }

            }
        }
    }


    surv = scene.addRect(QRectF(QPointF(cellSize.width()*survCo[0].x, cellSize.height()*survCo[0].y), cellSize), Qt::NoPen, Qt::green);
    zomb = scene.addRect(QRectF(QPointF(cellSize.width()*zombCo.x, cellSize.height()*zombCo.y), cellSize), Qt::NoPen, Qt::darkGreen);

}

void MainWgt::findPath() { //для зомби




    //----ищем путь

    int pos = fy*lengthArr+fx;

    path<<pos;

    bool pathInvalid=false;
    while( (pathInvalid==false) && (cells[pos]->step!=0) ) {
        if ( (floor((pos-1)/(float)lengthArr)==pos/lengthArr) &&
             (cells[pos]->step == cells[pos-1]->step+1) &&
             (cells[pos]->sides[3].block==false) &&
             (cells[pos-1]->sides[1].block==false) ) {
            pos-=1;path<<pos;
        }
        else if ( ((pos+1)/lengthArr==pos/lengthArr) &&
                  (cells[pos]->step == cells[pos+1]->step+1) &&
                  (cells[pos]->sides[1].block==false) &&
                  (cells[pos+1]->sides[3].block==false) ) {
            pos+=1;path<<pos;
        }
        else if ( (pos-lengthArr>=0) &&
                  (cells[pos]->step == cells[pos-lengthArr]->step+1) &&
                  (cells[pos]->sides[0].block==false) &&
                  (cells[pos-lengthArr]->sides[2].block==false) ) {
            pos-=lengthArr;path<<pos;
        }
        else if ( (pos+lengthArr<pow(lengthArr, 2) ) &&
                  (cells[pos]->step == cells[pos+lengthArr]->step+1) &&
                  (cells[pos]->sides[2].block==false) &&
                  (cells[pos+lengthArr]->sides[0].block==false) ) {
            pos+=lengthArr;path<<pos;
        }
        else {pathInvalid=true;}
    }

    if (pathInvalid) {
        path << -1; //путь не найден
    }
    count=path.size()-1-1;
    qDebug() << path;
}

void MainWgt::throwCube() {

    numStep = rand() % 12 +1;
    cube.setText(QString::number(numStep));
    startWave(survCo[0].x, survCo[0].y);

    for (auto it = stepField.begin(); it!=stepField.end(); ++it) {
        int i=(*it)->numModel/lengthArr; int j=(*it)->numModel%lengthArr;
        if(cells[i*lengthArr+j]->state=="None") {
            (*it)->setBrush(Qt::gray);
        }
        else if(cells[i*lengthArr+j]->state=="Home") {
            (*it)->setBrush(Qt::black);
        }
    }
    stepField.clear();

    for(int i = 0; i<numStep+1; ++i) {
        if (i>=wave.size()) {break;}
        for(int j = 0; j<wave[i].size(); ++j) {
            field[wave[i][j]->num]->setBrush(Qt::white);
            stepField<<field[wave[i][j]->num];
        }
    }

    if (numStep!=0) {
        zombStep();
    }

}

void MainWgt::toStep(){

    int speed = 5;

    if (cells[path[count+1]]->num - cells[path[count]]->num == 1) {
        if (dcellSize.width()-speed>=0) {
            if (_part == Surv) {
                surv->moveBy(-speed,0);
            }
            else {
                zomb->moveBy(-speed,0);
            }
        }
        else {
            if (_part == Surv) {
                surv->moveBy(-dcellSize.width(), 0);
            }
            else {
                zomb->moveBy(-dcellSize.width(), 0);
            }
        }
        dcellSize-=QSizeF(speed,0);
    }
    else if (cells[path[count+1]]->num - cells[path[count]]->num == -1) {
        if (dcellSize.width()-speed>=0){
            if (_part == Surv) {
                surv->moveBy(speed,0);
            }
            else {
                zomb->moveBy(speed,0);
            }
        }
        else {
            if (_part == Surv) {
                surv->moveBy(dcellSize.width(), 0);
            }
            else {
                zomb->moveBy(dcellSize.width(), 0);
            }
        }
        dcellSize-=QSizeF(speed,0);
    }
    else if (cells[path[count+1]]->num - cells[path[count]]->num == lengthArr) {
        if (dcellSize.height()-speed>=0)
            if (_part == Surv) {
                surv->moveBy(0,-speed);
            }
            else {
                zomb->moveBy(0,-speed);
            }
        else {
            if (_part == Surv) {
                surv->moveBy(0, -dcellSize.height());
            }
            else {
                zomb->moveBy(0, -dcellSize.height());
            }
        }
        dcellSize-=QSizeF(0,speed);
    }
    else if (cells[path[count+1]]->num - cells[path[count]]->num == -lengthArr) {
        if (dcellSize.height()-speed>=0)
            if (_part == Surv) {
                surv->moveBy(0,speed);
            }
            else {
                zomb->moveBy(0,speed);
            }
        else {
            if (_part == Surv) {
                surv->moveBy(0, dcellSize.height());
            }
            else {
                zomb->moveBy(0, dcellSize.height());
            }
        }
        dcellSize-=QSize(0,speed);
    }

    if(dcellSize.width()<=0 || dcellSize.height()<=0) {
        dcellSize=cellSize;
        count--;
        if (_part==Zomb) {
            timer.stop();
            zombCo.x=path[count+1]%lengthArr;
            zombCo.y=path[count+1]/lengthArr;
            _part=Surv;
        }
    }


    if (count<0) {
        timer.stop();
        if (_part == Surv) {
            survCo[0].x=path[count+1]%lengthArr;
            survCo[0].y=path[count+1]/lengthArr;

            for (auto it = stepField.begin(); it!=stepField.end(); ++it) {
                int i=(*it)->numModel/lengthArr; int j=(*it)->numModel%lengthArr;
                if(cells[i*lengthArr+j]->state=="None") {
                    (*it)->setBrush(Qt::gray);
                }
                else if(cells[i*lengthArr+j]->state=="Home") {
                    (*it)->setBrush(Qt::black);
                }
            }
            stepField.clear();

            if(numStep!=0) {
                startWave(survCo[0].x, survCo[0].y);
                for(int i = 0; i<numStep+1; ++i) {
                    for(int j = 0; j<wave[i].size(); ++j) {
                        field[wave[i][j]->num]->setBrush(Qt::white);
                        stepField<<field[wave[i][j]->num];
                    }
                }
            }
            else {_part=Zomb; zombStep(); }
        }
    }

}



void MainWgt::startWave(int x, int y) {

    if (!wave.isEmpty()) {
        QVector<QVector<Cell*>>::iterator it = wave.begin();
        for (;it!=wave.end();++it) {
            QVector<Cell*>::iterator it2 = (*it).begin();
            for (;it2!=it->end(); ++it2) {
                (*it2)->step=-1;
            }
            (*it).clear();
        }
        wave.clear();
    }

    if (!path.isEmpty()) {path.clear();}

    //-----------Пускаем волну

    cells[y*lengthArr+x]->step=0;

    wave << (QVector<Cell*>() << cells[y*lengthArr+x]);

    for ( int i =0; i<wave.size(); ++i) {          //волна
        QVector<Cell*> temp;
//        qDebug() << "wave" << i;
        for (int j = 0; j<wave[i].size(); ++j) {    //варианты

            int pos = wave[i][j]->num;
            if ( ( (pos+1)/lengthArr == pos/lengthArr) &&
                 ( cells[pos+1]->step==-1 ) &&
                 ( cells[pos+1]->sides[3].block==false ) &&
                 ( cells[pos]->sides[1].block==false ) ) {
                cells[pos+1]->step=i+1;
                temp<<cells[pos+1];
            }
            if ( (floor((pos-1)/(float)lengthArr) == pos/lengthArr) &&
                 ( cells[pos-1]->step==-1 ) &&
                 ( cells[pos-1]->sides[1].block==false ) &&
                 ( cells[pos]->sides[3].block==false )) {
                cells[pos-1]->step=i+1;
                temp<<cells[pos-1];
            }

            if ( (pos+lengthArr < pow(lengthArr,2)) &&
                 ( cells[pos+lengthArr]->step==-1 ) &&
                 ( cells[pos]->sides[2].block==false) &&
                 ( cells[pos+lengthArr]->sides[0].block==false ))  {
                cells[pos+lengthArr]->step=i+1;
                temp<<cells[pos+lengthArr];
            }

            if ( (pos-lengthArr >= 0) &&
                 ( cells[pos-lengthArr]->step==-1 ) &&
                 ( cells[pos]->sides[0].block==false ) &&
                 ( cells[pos-lengthArr]->sides[2].block==false ) ) {
                cells[pos-lengthArr]->step=i+1;
                temp<<cells[pos-lengthArr];
            }

        }
        if (!temp.isEmpty()) {
            wave<<temp;
        }

    }



}

void MainWgt::survStep(int num) {

    if (field[num]->brush()!=Qt::white) {return;}

    if(!timer.isActive()) {

        fy=num/lengthArr; fx=num%lengthArr;

        if (survCo[0].y*lengthArr+survCo[0].x == fy*lengthArr+fx) {
            qDebug() << "i2";
            return ;
        }

        startWave(survCo[0].x, survCo[0].y);
        findPath();

        if (path.contains(-1)) {
            qDebug() << "i1";
            return;
        }

        timer.start();
    }

    numStep-=cells[num]->step; cube.setText(QString::number(numStep));

}

void MainWgt::zombStep() {
    _part=Zomb;

    //видимость
    QVector<int> visible;
    for(int forw = -1; forw<2; forw+=2) {
        for(int vert = 0; vert<2; vert+=1) {
            for(int k = 0; k<lengthArr; ++k) {
                int pos = (zombCo.y+vert*forw*k)*lengthArr+zombCo.x+(1-vert)*forw*k;
                int spos = zombCo.y*lengthArr+zombCo.x;
                if( ( (vert==1) && (forw==-1) && pos >= 0 )||
                       ( (vert==1) && (forw==1) && pos<pow(lengthArr, 2) )||
                       ( (vert==0) && (forw==1) && pos/lengthArr==spos/lengthArr )||
                       ( (vert==0) && (forw==-1) && floor(pos/(float)lengthArr)==spos/lengthArr ) )
                {
                    visible<<pos;
                    if (forw==-1 && vert==0 && (cells[pos]->sides[3].block==true ||
                                                cells[pos]->sides[3].info=="CloseDoor" ||
                                                (
                                                floor((pos-1)/(float)lengthArr)==pos/lengthArr &&
                                                      (
                                                      (cells[pos-1]->sides[1].block==true) ||
                                                      (cells[pos-1]->sides[1].info=="CloseDoor")
                                                      )
                                                ) ) )  {break;}
                    else if (forw==-1 && vert==1 && (cells[pos]->sides[0].block==true ||
                                                     cells[pos]->sides[0].info=="CloseDoor" ||
                                                     (
                                                     pos-lengthArr>=0 &&
                                                           (
                                                           (cells[pos-lengthArr]->sides[2].block==true) ||
                                                           (cells[pos-lengthArr]->sides[2].info=="CloseDoor")
                                                           )
                                                     ) ) )  {break;}
                    else if (forw==1 && vert==0 && (cells[pos]->sides[1].block==true ||
                                                    cells[pos]->sides[1].info=="CloseDoor" ||
                                                    (
                                                    (pos+1)/lengthArr==pos/lengthArr &&
                                                          (
                                                          (cells[pos+1]->sides[3].block==true) ||
                                                          (cells[pos+1]->sides[3].info=="CloseDoor")
                                                          )
                                                    ) ) )  {break;}
                    else if (forw==1 && vert==1 && (cells[pos]->sides[2].block==true ||
                                                     cells[pos]->sides[2].info=="CloseDoor" ||
                                                     (
                                                     pos+lengthArr<pow(lengthArr,2) &&
                                                           (
                                                           (cells[pos+lengthArr]->sides[0].block==true) ||
                                                           (cells[pos+lengthArr]->sides[0].info=="CloseDoor")
                                                           )
                                                     ) ) )  {break;}
                    if (cells[pos]->state=="Home") {break;}
                }
            }
        }
    }
    qDebug() << visible;
    //видит выживших
    if (visible.contains(survCo[0].y*lengthArr+survCo[0].x)) {
        fx=survCo[0].x;
        fy=survCo[0].y;
    }
    else
    {
        //шум
        fx=survCo[0].x;
        fy=survCo[0].y;
    }

    if (zombCo.y*lengthArr+zombCo.x == fy*lengthArr+fx) {
        qDebug() << "i2";
        return ;
    }

    startWave(zombCo.x, zombCo.y);
    findPath();

    if (path.contains(-1)) {
        qDebug() << "i1";
        return;
    }


    timer.start();
}



MainWgt::~MainWgt()
{

}

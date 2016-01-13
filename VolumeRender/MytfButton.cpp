#include "MytfButton.h"
#include <QApplication>
#include <QMimeData>
#include <QDrag>

MyTFButton::MyTFButton(int ind, QWidget *parent):
    QPushButton(parent)
{
    setFixedSize(20,10);
    setAcceptDrops(true);
    moveHMin = 5;
    moveHMax = 265;
    moveVMin = 0;
    moveVMax = 0;

    btnQColor = Qt::white;
    color = Vec3f::zero();
    alpha = 0.0;
    this->setStyleSheet("border:1px solid black");
    this->index = ind;
}

void MyTFButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
         startPos = event->pos();
    QPushButton::mousePressEvent(event);
}
void MyTFButton::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() ) {
        QPoint movDis = event->pos() - startPos;
        if (movDis.manhattanLength() >= QApplication::startDragDistance())
        {
            int moveX = this->x() + movDis.x();
            if(moveX >= moveHMin && moveX <= moveHMax)
            {
                move(this->x() + movDis.x(), this->y());
            }
            //emit moved();
        }
    }
   QPushButton::mouseMoveEvent(event);
}

void MyTFButton::SetColor(QColor qcolor)
{
    btnQColor = qcolor;
    color.x = (float)qcolor.red()/255;
    color.y = (float)qcolor.green()/255;
    color.z = (float)qcolor.blue()/255;
    this->setStyleSheet(QString("background-color:%1").arg(qcolor.name()));
}

void MyTFButton::setMoveRange(int left, int right, int top, int bottom)
{
    moveHMin = left;
    moveHMax = right;
    moveVMin = top;
    moveVMax = bottom;
}



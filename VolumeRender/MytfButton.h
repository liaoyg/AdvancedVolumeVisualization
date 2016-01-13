#ifndef MYTFBUTTON_H
#define MYTFBUTTON_H

#include <QPushButton>
#include <QMouseEvent>
#include "Mat.h"

class MyTFButton : public QPushButton
{
public:
    MyTFButton(int ind, QWidget *parent = 0);
    void SetColor(QColor qcolor);
    Vec3f getColor() { return color; }
    void SetAlpha(float a) { alpha = a; }
    float GetAlpha() { return alpha; }
    void setMoveRange(int left, int right, int top, int bottom);

    QColor btnQColor;
    int index;

private:
    int moveHMin;
    int moveHMax;
    int moveVMin;
    int moveVMax;
    QPoint startPos;
    Vec3f color;
    float alpha;


protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

signals:
    void moved();
public slots:
    void getColor(Vec3f col);

};

#endif // MYTFBUTTON_H

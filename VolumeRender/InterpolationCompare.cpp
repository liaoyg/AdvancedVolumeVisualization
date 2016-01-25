#include "InterpolationCompare.h"
#include <QPainter>
#include <qmath.h>

InterpolationCompare::InterpolationCompare(QWidget *parent) :
    QWidget(parent)
{
    ColorDiffRes = new QPixmap(256,256);
    compareBtn = new QPushButton("Color Diff", this);
    compareBtn->setGeometry(200,0,40,20);
    compareBtn->setFixedSize(120,33);
    ResDisplay = new QLabel(this);

    connect(compareBtn, SIGNAL(clicked()), this, SLOT(RequestGLImage()));
}

void InterpolationCompare::paintEvent(QPaintEvent *)
{
//    QPainter mapPainter(this);
//    mapPainter.drawPixmap(0,0,256,256,*ColorDiffRes);
//    if(!ColorRes.isNull())
//    {
//        QPainter resPainter(this);
//        resPainter.drawPixmap(0,0,256,556,ColorRes);
//    }
}

void InterpolationCompare::updateColorDiff()
{
    QPixmap ColorRes;
    if(comparedImage.size() == 2)
    {
        QImage &imgA = comparedImage[0];
        QImage &imgB = comparedImage[1];
        if(!imgA.isNull()&&!imgB.isNull()&&imgA.size() == imgB.size())
        {
            QImage Res(imgA.size(),QImage::Format_RGB32);
            for ( int row = 0; row < imgA.width(); ++row )
                for ( int col = 0; col < imgA.height(); ++col )
                {
                    QColor colorA(imgA.pixel(row, col));
                    QColor colorB(imgB.pixel(row, col));

                    qreal diff = qSqrt(qPow(colorA.red()-colorB.red(),2) + qPow(colorA.green()-colorB.green(),2) +qPow(colorA.blue()-colorB.blue(),2));
                    int diffint = int(diff/qSqrt(3));
                    Res.setPixel(row,col,QColor(diffint,255-diffint,255).rgb());
                    Res.setPixel(row,col,QColor(qAbs(colorA.red()-colorB.red()),qAbs(colorA.green()-colorB.green()),qAbs(colorA.blue()-colorB.blue())).rgb());
                }
            if(!Res.isNull())
            {
                int h = this->height();
                QPixmap resPixel = QPixmap::fromImage(Res);
                ResDisplay->setPixmap(resPixel);
                ResDisplay->setFixedSize(QSize(192,302));
                ResDisplay->show();
            }
        }
    }
    comparedImage.clear();

}

void InterpolationCompare::RequestGLImage()
{
    emit SendGLimageReq();
}

void InterpolationCompare::GetGLImage(QImage img)
{
    comparedImage.push_back(img);
    if(comparedImage.size() >= 2)
        updateColorDiff();
}

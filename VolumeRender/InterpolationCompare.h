#ifndef INTERPOLATIONCOMPARE_H
#define INTERPOLATIONCOMPARE_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include "Global.h"

class InterpolationCompare : public QWidget
{
    Q_OBJECT
public:
    explicit InterpolationCompare(QWidget *parent = 0);

protected:
    virtual void paintEvent(QPaintEvent *);
    void updateColorDiff();

private:
    QPushButton *compareBtn;
    QPixmap *ColorDiffRes;
    QLabel* ResDisplay;

    std::vector<QImage> comparedImage;


signals:
    void SendGLimageReq();

public slots:
    void RequestGLImage();
    void GetGLImage(QImage);

};

#endif // INTERPOLATIONCOMPARE_H

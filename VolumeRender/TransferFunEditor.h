#ifndef TRANSFERFUNEDITOR_H
#define TRANSFERFUNEDITOR_H

#include <QWidget>
#include "MytfButton.h"
#include <QPixmap>
#include <QColorDialog>
#include <QCheckBox>

class TransferFunEditor : public QWidget
{
    Q_OBJECT
public:
    explicit TransferFunEditor(QWidget *parent = 0);

private:
    std::vector<Vec4f> transferFuncPointData;

    MyTFButton *colorBtn[6];
    QColorDialog *colorSelect;
    MyTFButton *currentBtn;
    QPushButton *applyBtn;
    QCheckBox *isPreInt;
//    QRadioButton *PreIntFlag;
    int tfImageWidth;
    int tfImageHeight;

    int leftmargin;
    int topmargin;
    QPixmap *transferFuncMap;

    bool setAlpha;
    int lastSetPosition;

    virtual void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    bool IsMouseOnMap(QPoint pos);
    void applyTfPointData();

signals:
    void sendTFMap(QPixmap);

public slots:
    void ColorChange();
    void OpenColorSelect();
    void setColor(QColor color);
    void finishEdit();

};

#endif // TRANSFERFUNEDITOR_H

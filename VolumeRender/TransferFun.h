#ifndef TRANSFERFUN_H
#define TRANSFERFUN_H

#include <QWidget>
#include <QLabel>
#include "MytfButton.h"
#include <QColorDialog>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QLineEdit>
#include "Global.h"
#include "TransferFunEditor.h"

class TransferFun : public QWidget
{
    Q_OBJECT
public:
    explicit TransferFun(QWidget *parent = 0);

private:
    QLabel *nameTF;
    QPushButton *applyBtn;
    TransferFunEditor *tfEditor;
    QPixmap *TFMap;

protected:
    virtual void paintEvent(QPaintEvent *);
signals:
    void TFDateApply();

public slots:
    void ColorChange();
    void OpenTransferFuncEditor();
    void updateTFMap(QPixmap tfmap);

};

#endif // TRANSFERFUN_H

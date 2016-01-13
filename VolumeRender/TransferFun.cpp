#include "TransferFun.h"
#include <QLayout>
#include <QPainter>
#include <QColorDialog>
#include <QSlider>
#include <QMouseEvent>

TransferFun::TransferFun(QWidget *parent) :
    QWidget(parent)
{
    tfEditor = new TransferFunEditor;
    tfEditor->hide();

    TFMap = new QPixmap(250,90);

//    for(int i = 0; i< 6; i++)
//    {
//        tfBtn[i] = new MyTFButton(i,this);
//        tfBtn[i]->setGeometry(5 + i*52,177,0,0);
//        tfBtn[i]->SetAlpha(0.1*i);
//    }
//    {
//        //initial transfer function color
//        tfBtn[0]->SetColor(Qt::black);
//        tfBtn[1]->SetColor(Qt::red);
//        tfBtn[2]->SetColor(Qt::green);
//        tfBtn[3]->SetColor(Qt::white);
//        tfBtn[4]->SetColor(Qt::yellow);
//        tfBtn[5]->SetColor(Qt::blue);
//    }
//    currentBtn = tfBtn[0];

    QLabel *tFname = new QLabel(QWidget::tr("Transfer Function:"), this);
    applyBtn = new QPushButton("Apply", this);
    applyBtn->setGeometry(200,0,40,20);
    applyBtn->setFixedSize(80,33);
    QHBoxLayout *buttonlayout = new QHBoxLayout;
    buttonlayout->addWidget(tFname);
    buttonlayout->addWidget(applyBtn);

    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->addLayout(buttonlayout);
    mainlayout->addSpacing(150);
//    mainlayout->addLayout(btmlayout);

    mainlayout->setMargin(3);
    this->setLayout(mainlayout);

    connect(applyBtn, SIGNAL(clicked()), this, SLOT(OpenTransferFuncEditor()));
    connect(tfEditor, SIGNAL(sendTFMap(QPixmap)), this, SLOT(updateTFMap(QPixmap)));

    //initial TFMap
    QPainter painter(TFMap);
    QLinearGradient linearGradient1(0,0,260,0);
    for(int i = 0; i< 6; i++)
    {
        linearGradient1.setColorAt(0.2*i,Qt::white);
    }
    painter.setBrush(linearGradient1);
    painter.drawRect(0,0,260,130);

}

void TransferFun::paintEvent(QPaintEvent *)
{
    QPainter mapPainter(this);
    mapPainter.drawPixmap(10,40,300,130,*TFMap);
}

void TransferFun::ColorChange()
{
    update();
    emit TFDateApply();
}

void TransferFun::OpenTransferFuncEditor()
{
    if(tfEditor->isHidden())
        tfEditor->show();
}

void TransferFun::updateTFMap(QPixmap tfmap)
{
    *TFMap = tfmap;
    update();
    emit TFDateApply();
}

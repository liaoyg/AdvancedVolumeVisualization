#include "TransferFunEditor.h"
#include "Global.h"
#include <QPainter>
#include <QLayout>

TransferFunEditor::TransferFunEditor(QWidget *parent) :
    QWidget(parent)
{
    setFixedSize(1100,480);

    tfImageWidth = 1024;
    tfImageHeight = 400;
    leftmargin = (width()-tfImageWidth)/2;
    topmargin = 20;
    setAlpha = false;
    lastSetPosition = 0;

    colorSelect = new QColorDialog;
    currentBtn = NULL;
    applyBtn = new QPushButton("Apply", this);
    //applyBtn->setGeometry(1000,440,40,20);
    applyBtn->setFixedSize(80,33);

    isPreInt = new QCheckBox("Pre-Integration", this);
    QHBoxLayout *btlayout = new QHBoxLayout;
    //btlayout->setGeometry(new QRect(0,440,1024,20));
    btlayout->setSpacing(10);
    btlayout->addWidget(isPreInt);
    btlayout->addWidget(applyBtn);
    btlayout->setAlignment(Qt::AlignBottom);

    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->addSpacing(440);
    mainlayout->addStretch(1);
    mainlayout->addLayout(btlayout);
//    mainlayout->setAlignment(Qt::AlignBottom);
    mainlayout->setMargin(3);
    this->setLayout(mainlayout);


    transferFuncMap = new QPixmap(tfImageWidth,tfImageHeight);

    for(int i = 0; i< 6; i++)
    {
        colorBtn[i] = new MyTFButton(i,this);
        colorBtn[i]->setGeometry(leftmargin + i*tfImageWidth/5-10,topmargin+tfImageHeight,0,0);
        colorBtn[i]->SetAlpha(0.1*i);
        colorBtn[i]->setMoveRange(leftmargin,(width()+tfImageWidth)/2,0,0);
    }
    {
        //initial transfer function color
        colorBtn[0]->SetColor(Qt::black);
        colorBtn[1]->SetColor(Qt::red);
        colorBtn[2]->SetColor(Qt::green);
        colorBtn[3]->SetColor(Qt::white);
        colorBtn[4]->SetColor(Qt::yellow);
        colorBtn[5]->SetColor(Qt::blue);
    }

    for(int i = 0; i< 512; i++)
    {
        transferFuncPointData.push_back(Vec4f(0.0, 0.002*i, 0.0, 0.0));
        //transferFuncPointData.
    }
    applyTfPointData();

    for(int i = 0; i< 6; i++)
    {
        connect(colorBtn[i], SIGNAL(pressed()), this, SLOT(ColorChange()));
        connect(colorBtn[i], SIGNAL(released()), this, SLOT(ColorChange()));
        //connect(colorBtn[i], SIGNAL(moved()), this, SLOT(ColorChange()));
        connect(colorBtn[i], SIGNAL(clicked()), this, SLOT(OpenColorSelect()));
    }
    connect(colorSelect, SIGNAL(colorSelected(QColor)), this, SLOT(setColor(QColor)));
    connect(colorSelect, SIGNAL(currentColorChanged(QColor)), this, SLOT(setColor(QColor)));
    connect(applyBtn, SIGNAL(clicked()), this, SLOT(finishEdit()));
}

void TransferFunEditor::paintEvent(QPaintEvent *)
{
    QPainter painter(transferFuncMap);
    QLinearGradient linearGradient1(0,0,tfImageWidth,0);
    for(int i = 0; i< 6; i++)
    {
        linearGradient1.setColorAt(float((colorBtn[i]->x()+10.0 - leftmargin)/tfImageWidth),colorBtn[i]->btnQColor);
    }
    painter.setBrush(linearGradient1);
    painter.drawRect(0,0,tfImageWidth,tfImageHeight);
    painter.setBrush(QBrush(Qt::gray, Qt::SolidPattern));
    painter.drawRect(0,380,tfImageWidth,20);

    //alpha curve
    //QPainter alphaPainter(this);
    painter.setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap));
    painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
    QPoint start(0,tfImageHeight-transferFuncPointData[0].w*400);
    for(int i = 1; i<transferFuncPointData.size();i++)
    {
        QPoint end(i*tfImageWidth/512,tfImageHeight-transferFuncPointData[i].w*380-20);
        painter.drawLine(start, end);
        start = end;
    }
    QPainter tfpainter(this);
    tfpainter.drawPixmap((width()-tfImageWidth)/2,20,tfImageWidth,tfImageHeight, *transferFuncMap);
}

void TransferFunEditor::ColorChange()
{
    applyTfPointData();
    update();
}
void TransferFunEditor::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        lastSetPosition = (event->pos().x()-leftmargin)*transferFuncPointData.size()/tfImageWidth;
    QWidget::mousePressEvent(event);
}
void TransferFunEditor::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons()  & Qt::LeftButton) {
        QPoint mousePos = event->pos();
        if(IsMouseOnMap(mousePos))
        {
            float alphavalue = float((400 - mousePos.y())/380.0);
            int position = (mousePos.x()-leftmargin)*transferFuncPointData.size()/tfImageWidth;
            for(int i = lastSetPosition + 1; i<= position; i++)
            {
                alphavalue = (alphavalue < 0) ? 0: alphavalue;
                transferFuncPointData[i].w = alphavalue;
            }
            lastSetPosition = position;
            update();
        }
    }
    QWidget::mouseMoveEvent(event);
}

bool TransferFunEditor::IsMouseOnMap(QPoint pos)
{
    if(pos.x() >= leftmargin && pos.x() <= tfImageWidth + leftmargin)
        if(pos.y() >= topmargin && pos.y() <= tfImageHeight + topmargin)
            return true;
    return false;
}

void TransferFunEditor::applyTfPointData()
{
    QImage tfColorImage = transferFuncMap->toImage();
    for(int i = 0; i< transferFuncPointData.size();i++)
    {
        QRgb colorRGB = tfColorImage.pixel(i*tfImageWidth/transferFuncPointData.size(),100);
        transferFuncPointData[i].x = float(qRed(colorRGB))/255.0;
        transferFuncPointData[i].y = float(qGreen(colorRGB))/255.0;
        transferFuncPointData[i].z = float(qBlue(colorRGB))/255.0;
    }
}

void TransferFunEditor::OpenColorSelect()
{
    if(colorSelect&&colorSelect->isHidden())
    {
        colorSelect->show();
    }
    currentBtn = (MyTFButton*)sender();
}

void TransferFunEditor::setColor(QColor color)
{
    currentBtn->SetColor(color);
    QPalette pal = currentBtn->palette();
    pal.setColor(QPalette::Button,QColor(color.red(),color.green(),color.blue()));
    currentBtn->setPalette(pal);
    update();
}

void TransferFunEditor::finishEdit()
{
    applyTfPointData();
    this->hide();
    // send to global value
    gb_PreIntFlag = isPreInt->isChecked();
    tfdata.clear();
    for(int i = 0; i< transferFuncPointData.size();i++)
    {
        tfdata.push_back(transferFuncPointData[i]);
    }
    emit sendTFMap(*transferFuncMap);
}

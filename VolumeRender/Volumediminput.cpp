#include "Volumediminput.h"


VolumeDimInput::VolumeDimInput(QWidget *parent) :
    QWidget(parent)
{
    //Initial
    clipData = Vec3f(120.0,45.0,0.2);

    //setMaximumSize(200, 100);
    //QHBoxLayout *InputLayout = new QHBoxLayout;
    QGridLayout *InputLayout = new QGridLayout;
    InputLayout->setColumnStretch(0,10);
    InputLayout->setColumnStretch(1,1);
    InputLayout->setColumnStretch(2,1);

    //InputLayout->setMargin(15);
    InputLayout->setColumnMinimumWidth(1, 40);
    InputLayout->setColumnMinimumWidth(2, 40);

    QLabel *dimLbl = new QLabel(QWidget::tr("Clipping Parameters:"), this);

    azimuthSlider = new QSlider(Qt::Horizontal);
    azimuthSlider->setMinimum(0);
    azimuthSlider->setMaximum(360);
    azimuthSlider->setValue(100);
    azimuthEditor = new QLineEdit;
    azimuthEditor->setText("100");
    azimuthEditor->setEnabled(false);
    azimuthEditor->setAlignment(Qt::AlignHCenter);
    //azimuthBtn = new QPushButton("OK", this);

    elevationSlider = new QSlider(Qt::Horizontal);
    elevationSlider->setMinimum(-90);
    elevationSlider->setMaximum(90);
    elevationSlider->setValue(45);
    elevationEditor = new QLineEdit;
    elevationEditor->setText("45");
    elevationEditor->setEnabled(false);
    elevationEditor->setAlignment(Qt::AlignHCenter);
    //elevationBtn = new QPushButton("OK", this);

    clipPlaneDepthSlider = new QSlider(Qt::Horizontal);
    clipPlaneDepthSlider->setMinimum(0);
    clipPlaneDepthSlider->setMaximum(100);
    clipPlaneDepthSlider->setValue(20);
    clipPlaneDepthEditor = new QLineEdit;
    clipPlaneDepthEditor->setText("0.2");
    clipPlaneDepthEditor->setEnabled(false);
    clipPlaneDepthEditor->setAlignment(Qt::AlignHCenter);
    //clipPlaneDepthBtn = new QPushButton("OK", this);

    btnClipflag = new QRadioButton("Clipping", this);
    QPushButton *btnApply = new QPushButton("Apply", this);
    btnApply->setGeometry(200,160,40,20);
    btnApply->setFixedSize(80,33);

    InputLayout->addWidget(azimuthSlider,1,0,1,2);
    InputLayout->addWidget(azimuthEditor,1,2);
    InputLayout->addWidget(elevationSlider,2,0,1,2);
    InputLayout->addWidget(elevationEditor,2,2);
    InputLayout->addWidget(clipPlaneDepthSlider,3,0,1,2);
    InputLayout->addWidget(clipPlaneDepthEditor,3,2);

    QHBoxLayout *btmlayout = new QHBoxLayout;
    btmlayout->addWidget(btnClipflag);
    btmlayout->addWidget(btnApply);

    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->setMargin(3);
    mainlayout->addWidget(dimLbl);
    mainlayout->addLayout(InputLayout);
    mainlayout->addLayout(btmlayout);
    this->setLayout(mainlayout);

    connect(azimuthSlider, SIGNAL(valueChanged(int)), this, SLOT(SetSliderValueToEditor()));
    connect(elevationSlider, SIGNAL(valueChanged(int)), this, SLOT(SetSliderValueToEditor()));
    connect(clipPlaneDepthSlider, SIGNAL(valueChanged(int)), this, SLOT(SetSliderValueToEditor()));

    connect(btnApply, SIGNAL(clicked()), this, SLOT(GetClipflag()));
}

void VolumeDimInput::GetClipflag()
{
    gb_Clipflag = btnClipflag->isChecked();
    emit SendClipData(clipData);
}

void VolumeDimInput::SetSliderValueToEditor()
{
    int pos = azimuthSlider->value();
    QString str = QString("%1").arg(pos);
    azimuthEditor->setText(str);
    pos = elevationSlider->value();
    str = QString("%1").arg(pos);
    elevationEditor->setText(str);
    float fpos = clipPlaneDepthSlider->value()/100.0;
    str = QString::number(fpos);
    clipPlaneDepthEditor->setText(str);

    clipData.x = azimuthEditor->text().toFloat();
    clipData.y = elevationEditor->text().toFloat();
    clipData.z = clipPlaneDepthEditor->text().toFloat();
    emit SendClipData(clipData);
}

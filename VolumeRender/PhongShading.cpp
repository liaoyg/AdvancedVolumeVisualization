#include "PhongShading.h"
#include <QLayout>

PhongShading::PhongShading(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *InputLayout = new QGridLayout;
    InputLayout->setColumnStretch(0,4);
    InputLayout->setColumnStretch(1,4);
    InputLayout->setColumnStretch(2,2);

    AmbientCoELabel = new QLabel(QWidget::tr("Ambient Coefficient:"), this);
    AmbientCoESlider = new QSlider(Qt::Horizontal);
    AmbientCoESlider->setMinimum(0);
    AmbientCoESlider->setMaximum(100);
    AmbientCoESlider->setValue(30);
    AmbientCoEEditor = new QLineEdit;
    AmbientCoEEditor->setText("0.3");
    AmbientCoEEditor->setEnabled(false);
    AmbientCoEEditor->setAlignment(Qt::AlignHCenter);

    InputLayout->addWidget(AmbientCoELabel,0,1);
    InputLayout->addWidget(AmbientCoESlider,1,0,1,2);
    InputLayout->addWidget(AmbientCoEEditor,1,2);

    DiffuseCLabel = new QLabel(QWidget::tr("Diffuse:"), this);
    DiffuseCSlider = new QSlider(Qt::Horizontal);
    DiffuseCSlider->setMinimum(0);
    DiffuseCSlider->setMaximum(100);
    DiffuseCSlider->setValue(50);
    DiffuseCEditor = new QLineEdit;
    DiffuseCEditor->setText("0.5");
    DiffuseCEditor->setEnabled(false);
    DiffuseCEditor->setAlignment(Qt::AlignHCenter);

    InputLayout->addWidget(DiffuseCLabel,2,1);
    InputLayout->addWidget(DiffuseCSlider,3,0,1,2);
    InputLayout->addWidget(DiffuseCEditor,3,2);

    SpecularCLabel = new QLabel(QWidget::tr("Specular:"), this);
    SpecularCSlider = new QSlider(Qt::Horizontal);
    SpecularCSlider->setMinimum(0);
    SpecularCSlider->setMaximum(100);
    SpecularCSlider->setValue(50);
    SpecularCEditor = new QLineEdit;
    SpecularCEditor->setText("0.5");
    SpecularCEditor->setEnabled(false);
    SpecularCEditor->setAlignment(Qt::AlignHCenter);

    InputLayout->addWidget(SpecularCLabel,4,1);
    InputLayout->addWidget(SpecularCSlider,5,0,1,2);
    InputLayout->addWidget(SpecularCEditor,5,2);

    ShinessLabel = new QLabel(QWidget::tr("Shiness:"), this);
    ShinessSlider = new QSlider(Qt::Horizontal);
    ShinessSlider->setMinimum(0);
    ShinessSlider->setMaximum(100);
    ShinessSlider->setValue(80);
    ShinessEditor = new QLineEdit;
    ShinessEditor->setText("80");
    ShinessEditor->setEnabled(false);
    ShinessEditor->setAlignment(Qt::AlignHCenter);

    InputLayout->addWidget(ShinessLabel,6,1);
    InputLayout->addWidget(ShinessSlider,7,0,1,2);
    InputLayout->addWidget(ShinessEditor,7,2);

    PhongSwith = new QCheckBox("Phong Shading", this);
    AmbientOSwith = new QCheckBox("Ambient Occlusion", this);
    QHBoxLayout *switchlayout = new QHBoxLayout;
    switchlayout->addWidget(PhongSwith);
    switchlayout->addWidget(AmbientOSwith);
    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->addLayout(switchlayout);
    mainlayout->addLayout(InputLayout);

    this->setLayout(mainlayout);

    connect(AmbientCoESlider, SIGNAL(valueChanged(int)), this, SLOT(SetSliderValueToEditor()));
    connect(DiffuseCSlider, SIGNAL(valueChanged(int)), this, SLOT(SetSliderValueToEditor()));
    connect(SpecularCSlider, SIGNAL(valueChanged(int)), this, SLOT(SetSliderValueToEditor()));
    connect(ShinessSlider, SIGNAL(valueChanged(int)), this, SLOT(SetSliderValueToEditor()));
    connect(PhongSwith, SIGNAL(clicked()), this, SLOT(SetSliderValueToEditor()));
    connect(AmbientOSwith, SIGNAL(clicked()), this, SLOT(SetSliderValueToEditor()));
}

void PhongShading::SetSliderValueToEditor()
{
    float fpos = AmbientCoESlider->value()/100.0;
    QString str = QString::number(fpos);
    AmbientCoEEditor->setText(str);
    fpos = DiffuseCSlider->value()/100.0;
    str = QString::number(fpos);
    DiffuseCEditor->setText(str);
    fpos = SpecularCSlider->value()/100.0;
    str = QString::number(fpos);
    SpecularCEditor->setText(str);
    fpos = ShinessSlider->value();
    str = QString::number(fpos);
    ShinessEditor->setText(str);

    gb_AmbientCoE = AmbientCoEEditor->text().toFloat();
    gb_DiffuseC = DiffuseCEditor->text().toFloat();
    gb_SpecularC = SpecularCEditor->text().toFloat();
    gb_Shiness = ShinessEditor->text().toFloat();
    gb_AOFlag = AmbientOSwith->isChecked();
    if(PhongSwith->isChecked())
        emit PhongShadingChange(3);
    else
        emit PhongShadingChange(2); //2 is close
}

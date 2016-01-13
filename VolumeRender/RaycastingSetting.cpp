#include "RaycastingSetting.h"
#include <QLayout>
#include <QValidator>

RaycastingSetting::RaycastingSetting(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *mainlayout = new QGridLayout;
    mainlayout->setColumnStretch(0,3);
    mainlayout->setColumnStretch(1,2);
    mainlayout->setColumnStretch(2,2);

    sampleStep = new QLabel(QWidget::tr("Sampling Step size:"), this);
    sampleStepValue = new QLineEdit;
    sampleStepValue->setValidator(new QDoubleValidator(-1.00,1.00,6,this));
    sampleApplyBtn = new QPushButton("Apply", this);

    opacityCorect = new QLabel(QWidget::tr("Opacity Correction:"), this);
    opacityCorectValue = new QLineEdit;
    opacityCorectValue->setValidator(new QDoubleValidator(-1.00,1.00,6,this));
    opacityApplyBtn = new QPushButton("Apply", this);

    mainlayout->addWidget(sampleStep,0,0);
    mainlayout->addWidget(sampleStepValue,0,1);
    mainlayout->addWidget(sampleApplyBtn,0,2);
    mainlayout->addWidget(opacityCorect,1,0);
    mainlayout->addWidget(opacityCorectValue,1,1);
    mainlayout->addWidget(opacityApplyBtn,1,2);

    this->setLayout(mainlayout);

    connect(sampleApplyBtn, SIGNAL(clicked()), this, SLOT(SendSampleStep()));
    connect(opacityApplyBtn, SIGNAL(clicked()), this, SLOT(SendOpacityCorrection()));

}

void RaycastingSetting::SendSampleStep()
{
    emit ApplySampleStep(sampleStepValue->text().toFloat());
}

void RaycastingSetting::SendOpacityCorrection()
{
    emit ApplyOpacityCorrection(opacityCorectValue->text().toFloat());
}

#include "VolumeVisEditor.h"
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>

VolumeVisEditor::VolumeVisEditor(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *editorLayout = new QVBoxLayout(this);

    vDim = new VolumeDimInput(this);
    Tf = new TransferFun(this);
    dataSelect = new DatasetSelect(this);
    rcSetting = new RaycastingSetting(this);
//    lightScroll = new QScrollArea(this);
//    lgtSettting = new LightSetting(this);
//    lgtSettting->setMaximumWidth(310);
//    lgtSettting->setMinimumHeight(200);
//    lgtSettting->setMaximumHeight(600);
//    lightScroll->setWidget(lgtSettting);
//    lightScroll->resize(lgtSettting->width(),lgtSettting->height());
    pShading = new PhongShading(this);
    basicSetting = new QTabWidget(this);
    IntepComp = new InterpolationCompare(this);

    basicSetting->addTab(vDim,"Clipping");
//    basicSetting->addTab(lightScroll,"Lighting");
    basicSetting->addTab(pShading, "Phong Shading");
    basicSetting->addTab(IntepComp, "ResultCompare");

    editorLayout->addWidget(dataSelect,1);
    editorLayout->addWidget(Tf,1);
    editorLayout->addWidget(basicSetting,1);
    //editorLayout->addWidget(lgtSettting,1);
    editorLayout->addWidget(rcSetting,2);

    editorLayout->setMargin(5);
    //this->setStyleSheet("QWidget{border:2px solid red}");
    this->setLayout(editorLayout);
    editorLayout->setAlignment(vDim,Qt::AlignTop);

    connect(vDim, SIGNAL(SendClipData(Vec3f)), this, SLOT(GetDim(Vec3f)));
    connect(Tf, SIGNAL(TFDateApply()), this, SLOT(SendTFToGL()));
    connect(dataSelect, SIGNAL(DatasetSelected(QString,Vec3i)), this, SLOT(SendDatasetToGL(QString,Vec3i)));
    connect(rcSetting, SIGNAL(ApplySampleStep(float)), this, SIGNAL(ApplySStepToGL(float)));
    connect(rcSetting, SIGNAL(ApplyOpacityCorrection(float)), this, SIGNAL(ApplyOCorrectionToGL(float)));
//    connect(lgtSettting, SIGNAL(ApplyLightSetting(int)), this, SIGNAL(ApplyLightToGL(int)));
    connect(pShading, SIGNAL(PhongShadingChange(int)), this, SIGNAL(ApplyPhongToGL(int)));
    connect(IntepComp, SIGNAL(SendGLimageReq()), this, SIGNAL(ApplyGLImageReq()));
    connect(this, SIGNAL(SendGLImageToCompare(QImage)), IntepComp, SLOT(GetGLImage(QImage)));

    emit ApplyClipflagToGl(Vec3i::zero());
    //vDim.show();

}

void VolumeVisEditor::GetDim(Vec3f clip)
{
    emit ApplyClipflagToGl(clip);
}

void VolumeVisEditor::SendTFToGL()
{
//    tfdata.clear();
//    for(int i = 0; i<6; i++)
//    {
//        tfdata.push_back(Tf->GetTfPonit(i));
//    }
    emit ApplyTF();
}

void VolumeVisEditor::SendDatasetToGL(QString str,Vec3i dim)
{
    emit ApplyDatasetToGL(str,dim);
}

void VolumeVisEditor::GetGLImage(QImage img)
{
    emit SendGLImageToCompare(img);
}

#include "GLWidget.h"
#include "Global.h"
#include "VolumeVisEditor.h"
#include "TransferFunEditor.h"
#include "TransferFun.h"
#include <QApplication>
#include <QWidget>

std::vector<Vec4f> tfdata;
bool gb_Clipflag;
float azimuth = 123;
float elevation = -44;
float clipPlaneDepth = 0.2;
float deltaStep = 0.05;
float opacityCorrection = 0.02;

Vec3f gb_ambient;
Vec3f gb_lightcolor;
Vec3f gb_PosOrDirect;
float gb_Shiness;
float gb_LightStrength;
float gb_CosAttent;
float gb_LineAttent;
float gb_QuadAttent;

float gb_AmbientCoE;
float gb_DiffuseC;
float gb_SpecularC;
//float gb_Shiness;

bool gb_AOFlag;
bool gb_PreIntFlag;

void ConnectFun(GLWidget &w, VolumeVisEditor &vEditor )
{
    QObject::connect(&vEditor, SIGNAL(ApplyClipflagToGl(Vec3f)), &w, SLOT(GetClipflag(Vec3f)));
    QObject::connect(&vEditor, SIGNAL(ApplyTF()), &w, SLOT(UpdateTransferFun()));
    QObject::connect(&vEditor,SIGNAL(ApplyDatasetToGL(QString,Vec3i)), &w, SLOT(OpenNewDataset(QString,Vec3i)));
    QObject::connect(&vEditor,SIGNAL(ApplySStepToGL(float)), &w, SLOT(UpdateStepSize(float)));
    QObject::connect(&vEditor,SIGNAL(ApplyOCorrectionToGL(float)), &w, SLOT(UpdateOpacityCorretion(float)));
    QObject::connect(&vEditor,SIGNAL(ApplyLightToGL(int)), &w, SLOT(UpdateLight(int)));
    QObject::connect(&vEditor,SIGNAL(ApplyPhongToGL(int)), &w, SLOT(UpdateLight(int)));
    QObject::connect(&vEditor,SIGNAL(ApplyGLImageReq()), &w, SLOT(ExtractImage()));
    QObject::connect(&w,SIGNAL(SendGLImage(QImage)), &vEditor, SLOT(GetGLImage(QImage)));

    return;
}

int main(int argc, char *argv[])
{

    QGLFormat glFormat = QGLFormat::defaultFormat();
    glFormat.setVersion(3, 3);
    glFormat.setProfile(QGLFormat::CoreProfile);
    glFormat.setSampleBuffers(true);
    glFormat.setSamples(4);
    QGLFormat::setDefaultFormat(glFormat);

    //initial global
    tfdata.push_back(Vec4f(1.0f, 0.0f, 0.0f, 0.0f));
    tfdata.push_back(Vec4f(1.0f, 1.0f, 0.0f, 0.1f));
    tfdata.push_back(Vec4f(0.0f, 1.0f, 0.0f, 0.2f));
    tfdata.push_back(Vec4f(0.0f, 1.0f, 1.0f, 0.3f));
    tfdata.push_back(Vec4f(0.0f, 0.0f, 1.0f, 0.4f));
    tfdata.push_back(Vec4f(1.0f, 0.0f, 1.0f, 0.5f));

    gb_Clipflag = false;
    gb_PreIntFlag = false;
    gb_AOFlag = false;
    azimuth = 123;
    elevation = -44;
    clipPlaneDepth = 0.2;

    deltaStep = 0.05;
    opacityCorrection = 0.05;

    gb_ambient = Vec3f(1.0,1.0,1.0);
    gb_lightcolor = Vec3f(1.0,0.0,0.0);
    gb_PosOrDirect = Vec3f(1.0,0.0,0.0);
    gb_Shiness = 0.5;
    gb_LightStrength = 0.5;
    gb_CosAttent = 0.5;
    gb_LineAttent = 0.3;
    gb_QuadAttent = 0.2;

    gb_AmbientCoE = 0.5;
    gb_DiffuseC = 0.5;
    gb_SpecularC = 0.5;
    gb_Shiness = 0.5;

    QApplication a(argc, argv);
    QDialog *mainWindow = new QDialog;
    mainWindow->resize(1440,900);

    GLWidget w(GLWidget::RenderMode::TRICUBIC);
    GLWidget c(GLWidget::RenderMode::AUTOLINER);
    VolumeVisEditor vEditor(mainWindow);

//    TransferFunEditor tfe;
//    tfe.show();

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(&w,4);
    mainLayout->addWidget(&c,4);
    mainLayout->addWidget(&vEditor,2);
    mainLayout->setMargin(5);
    mainWindow->setLayout(mainLayout);

    a.setActiveWindow(mainWindow);
    mainWindow->show();

//    QObject::connect(&vEditor, SIGNAL(ApplyClipflagToGl(Vec3f)), &w, SLOT(GetClipflag(Vec3f)));
//    QObject::connect(&vEditor, SIGNAL(ApplyTF()), &w, SLOT(UpdateTransferFun()));
//    QObject::connect(&vEditor,SIGNAL(ApplyDatasetToGL(QString,Vec3i)), &w, SLOT(OpenNewDataset(QString,Vec3i)));
//    QObject::connect(&vEditor,SIGNAL(ApplySStepToGL(float)), &w, SLOT(UpdateStepSize(float)));
//    QObject::connect(&vEditor,SIGNAL(ApplyOCorrectionToGL(float)), &w, SLOT(UpdateOpacityCorretion(float)));
//    QObject::connect(&vEditor,SIGNAL(ApplyLightToGL(int)), &w, SLOT(UpdateLight(int)));
//    QObject::connect(&vEditor,SIGNAL(ApplyPhongToGL(int)), &w, SLOT(UpdateLight(int)));
    ConnectFun(w, vEditor);
    ConnectFun(c, vEditor);
    return a.exec();
}

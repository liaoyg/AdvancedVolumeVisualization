#ifndef VOLUMEVISEDITOR_H
#define VOLUMEVISEDITOR_H

#include <QWidget>
#include <QTabWidget>
#include <QScrollArea>
#include "Volumediminput.h"
#include "TransferFun.h"
#include "DatasetSelect.h"
#include "RaycastingSetting.h"
#include "LightSetting.h"
#include "Global.h"
#include "PhongShading.h"

class VolumeVisEditor : public QWidget
{
    Q_OBJECT
public:
    explicit VolumeVisEditor(QWidget *parent = 0);

private:
    VolumeDimInput *vDim;
    TransferFun *Tf;
    DatasetSelect *dataSelect;
    RaycastingSetting *rcSetting;
    LightSetting *lgtSettting;
    QTabWidget *basicSetting;
    QScrollArea *lightScroll;
    PhongShading *pShading;

signals:
    void ApplyTF();
    void ApplyClipflagToGl(Vec3f);
    void ApplyDatasetToGL(QString,Vec3i);
    void ApplySStepToGL(float);
    void ApplyOCorrectionToGL(float);
    void ApplyLightToGL(int type);
    void ApplyPhongToGL(int type);

public slots:
    void GetDim(Vec3f);
    void SendTFToGL();
    void SendDatasetToGL(QString,Vec3i);

};

#endif // VOLUMEVISEDITOR_H

#ifndef VOLUMEDIMINPUT_H
#define VOLUMEDIMINPUT_H

#include <QApplication.h>
#include <QWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include "Mat.h"
#include "Global.h"


class VolumeDimInput : public QWidget
{
    Q_OBJECT
public:
    explicit VolumeDimInput(QWidget *parent = 0);

private:
    Vec3f clipData;

    QSlider *azimuthSlider;
    QLineEdit *azimuthEditor;

    QSlider *elevationSlider;
    QLineEdit *elevationEditor;

    QSlider *clipPlaneDepthSlider;
    QLineEdit *clipPlaneDepthEditor;

    QRadioButton *btnClipflag;

signals:
    void SendClipData(Vec3f);
public slots:
    void GetClipflag();
    void SetSliderValueToEditor();

};

#endif // VOLUMEDIMINPUT_H

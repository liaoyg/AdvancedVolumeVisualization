#ifndef PHONGSHADING_H
#define PHONGSHADING_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QCheckBox>
#include "Global.h"

class PhongShading : public QWidget
{
    Q_OBJECT
public:
    explicit PhongShading(QWidget *parent = 0);

private:
    QCheckBox *PhongSwith;
    QCheckBox *AmbientOSwith;
    QSlider *AmbientCoESlider;
    QLabel *AmbientCoELabel;
    QLineEdit *AmbientCoEEditor;

    QSlider *DiffuseCSlider;
    QLabel *DiffuseCLabel;
    QLineEdit *DiffuseCEditor;

    QSlider *SpecularCSlider;
    QLabel *SpecularCLabel;
    QLineEdit *SpecularCEditor;

    QSlider *ShinessSlider;
    QLabel *ShinessLabel;
    QLineEdit *ShinessEditor;


signals:
    void PhongShadingChange(int type);

public slots:
    void SetSliderValueToEditor();

};

#endif // PHONGSHADING_H

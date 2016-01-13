#ifndef RAYCASTINGSETTING_H
#define RAYCASTINGSETTING_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

class RaycastingSetting : public QWidget
{
    Q_OBJECT
public:
    explicit RaycastingSetting(QWidget *parent = 0);

private:
    QLabel *sampleStep;
    QLineEdit *sampleStepValue;
    QPushButton *sampleApplyBtn;
    QLabel *opacityCorect;
    QLineEdit *opacityCorectValue;
    QPushButton *opacityApplyBtn;

signals:
    void ApplySampleStep(float);
    void ApplyOpacityCorrection(float);

public slots:
    void SendSampleStep();
    void SendOpacityCorrection();

};

#endif // RAYCASTINGSETTING_H

#ifndef LIGHTSETTING_H
#define LIGHTSETTING_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>
#include "Mat.h"
#include "Global.h"

class LightSetting : public QWidget
{
    Q_OBJECT
public:
    explicit LightSetting(QWidget *parent = 0);

private:
    QTableWidget *lightTable;
    QWidget *point;
    QWidget *other;

    QLabel *lightTypeName;
    QComboBox *lightType;
    QPushButton *applyBtn;

    QLabel *AmbientName;
    QLineEdit *AmbientEditX;
    QLineEdit *AmbientEditY;
    QLineEdit *AmbientEditZ;
    QLabel *ColorName;
    QLineEdit *ColorEditX;
    QLineEdit *ColorEditY;
    QLineEdit *ColorEditZ;
    QLabel *DirectionName;
    QLineEdit *DirectionEditX;
    QLineEdit *DirectionEditY;
    QLineEdit *DirectionEditZ;

    QLabel *PositionName;

    QLabel *ShinessName;
    QLineEdit *ShinessValue;
    QLabel *StrengthName;
    QLineEdit *StrengthValue;
    /*
    QLabel *CosattenName;
    QLineEdit *CosattenValue;
    QLabel *LineattenName;
    QLineEdit *LineattenValue;
    QLabel *QuadattenName;
    QLineEdit *QuadattenValue;
*/


signals:
    void ApplyLightSetting(int);
    void SizeChange();

public slots:
    void setSetting(int index);
    void SendLightSetting();

};

#endif // LIGHTSETTING_H

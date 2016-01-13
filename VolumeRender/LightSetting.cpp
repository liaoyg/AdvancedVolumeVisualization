#include "LightSetting.h"
#include <QLayout>
#include <QStringList>
#include <QScrollArea>

LightSetting::LightSetting(QWidget *parent) :
    QWidget(parent)
{
    lightType = new QComboBox();
    lightType->addItem(QWidget::tr("Direction Light"));
    lightType->addItem(QWidget::tr("Point Light"));
    lightType->addItem(QWidget::tr("No light"));

    //tablewodget
    lightTable = new QTableWidget(3,4);
    lightTable->setWindowTitle("Light Setting");
    lightTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    lightTable->setColumnWidth(0,90);
    lightTable->setColumnWidth(1,55);
    lightTable->setColumnWidth(2,55);
    lightTable->setColumnWidth(3,55);
    lightTable->setFixedHeight(116);
    QStringList header;
    header<<"type"<<"X"<<"Y"<<"Z";
    lightTable->setHorizontalHeaderLabels(header);

    AmbientName = new QLabel(QWidget::tr("Ambient:"), lightTable);
    AmbientEditX = new QLineEdit(lightTable);
    AmbientEditX->setValidator(new QDoubleValidator(0.00,1.00,3,this));
    AmbientEditX->setText("1.0");
    AmbientEditY = new QLineEdit(lightTable);
    AmbientEditY->setValidator(new QDoubleValidator(0.00,1.00,3,this));
    AmbientEditY->setText("1.0");
    AmbientEditZ = new QLineEdit(lightTable);
    AmbientEditZ->setValidator(new QDoubleValidator(0.00,1.00,3,this));
    AmbientEditZ->setText("1.0");
    lightTable->setCellWidget(0,0,AmbientName);
    lightTable->setCellWidget(0,1,AmbientEditX);
    lightTable->setCellWidget(0,2,AmbientEditY);
    lightTable->setCellWidget(0,3,AmbientEditZ);

    ColorName = new QLabel(QWidget::tr("LightColor:"), lightTable);
    ColorEditX = new QLineEdit(lightTable);
    ColorEditX->setValidator(new QDoubleValidator(0.00,1.00,3,this));
    ColorEditX->setText("1.0");
    ColorEditY = new QLineEdit(lightTable);
    ColorEditY->setValidator(new QDoubleValidator(0.00,1.00,3,this));
    ColorEditY->setText("0.0");
    ColorEditZ = new QLineEdit(lightTable);
    ColorEditZ->setValidator(new QDoubleValidator(0.00,1.00,3,this));
    ColorEditZ->setText("0.0");
    lightTable->setCellWidget(1,0,ColorName);
    lightTable->setCellWidget(1,1,ColorEditX);
    lightTable->setCellWidget(1,2,ColorEditY);
    lightTable->setCellWidget(1,3,ColorEditZ);

    DirectionName = new QLabel(QWidget::tr("Direction:"), lightTable);
    DirectionEditX = new QLineEdit(lightTable);
    DirectionEditX->setValidator(new QDoubleValidator(0.00,1.00,3,this));
    DirectionEditX->setText("1.0");
    DirectionEditY = new QLineEdit(lightTable);
    DirectionEditY->setValidator(new QDoubleValidator(0.00,1.00,3,this));
    DirectionEditY->setText("0.0");
    DirectionEditZ = new QLineEdit(lightTable);
    ColorEditZ->setValidator(new QDoubleValidator(0.00,1.00,3,this));
    DirectionEditZ->setText("1.0");
    lightTable->setCellWidget(2,0,DirectionName);
    lightTable->setCellWidget(2,1,DirectionEditX);
    lightTable->setCellWidget(2,2,DirectionEditY);
    lightTable->setCellWidget(2,3,DirectionEditZ);

    other = new QWidget;
    QGridLayout *othlayout = new QGridLayout;
    othlayout->setColumnStretch(0,1);
    othlayout->setColumnStretch(1,1);
    othlayout->setColumnStretch(2,1);
    othlayout->setColumnStretch(3,1);
    ShinessName = new QLabel(QWidget::tr("Shiness:"), other);
    ShinessValue = new QLineEdit;
    ShinessValue->setValidator(new QDoubleValidator(0.00,1.00,3,this));
    ShinessValue->setText("0.5");
    StrengthName = new QLabel(QWidget::tr("Strength:"), other);
    StrengthValue = new QLineEdit;
    StrengthValue->setValidator(new QDoubleValidator(0.00,1.00,3,this));
    StrengthValue->setText("0.5");
    othlayout->addWidget(ShinessName,0,0);
    othlayout->addWidget(ShinessValue,0,1);
    othlayout->addWidget(StrengthName,0,2);
    othlayout->addWidget(StrengthValue,0,3);
    other->setLayout(othlayout);

    applyBtn = new QPushButton(QWidget::tr("Apply"));
    applyBtn->setFixedSize(80,33);
    QHBoxLayout *btmlayout = new QHBoxLayout;
    btmlayout->addStretch();
    btmlayout->addWidget(applyBtn);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(lightType);
    layout->addWidget(lightTable);
    layout->addWidget(other);
    //layout->addWidget(point);
    layout->addLayout(btmlayout);

    //point->setHidden(true);
    this->setLayout(layout);


    connect(lightType, SIGNAL(currentIndexChanged(int)), this, SLOT(setSetting(int)));
    connect(applyBtn, SIGNAL(clicked()), this, SLOT(SendLightSetting()));


}

void LightSetting::setSetting(int index)
{
    if(index == 0)
    {
        DirectionName->setText("Direction:");
        other->show();
        lightTable->show();
    }
    else if(index == 1)
    {
        DirectionName->setText("Position:");
        other->show();
        lightTable->show();
    }
    else if(index == 2)
    {
        other->setHidden(true);
        lightTable->setHidden(true);
    }
    emit SizeChange();
}

void LightSetting::SendLightSetting()
{
    gb_ambient = Vec3f(AmbientEditX->text().toFloat(),AmbientEditY->text().toFloat(),AmbientEditZ->text().toFloat());
    gb_lightcolor = Vec3f(ColorEditX->text().toFloat(),ColorEditY->text().toFloat(),ColorEditZ->text().toFloat());
    gb_PosOrDirect = Vec3f(DirectionEditX->text().toFloat(),DirectionEditY->text().toFloat(),DirectionEditZ->text().toFloat());
    gb_Shiness = ShinessValue->text().toFloat();
    gb_LightStrength = StrengthValue->text().toFloat();

    //gb_CosAttent = CosattenValue->text().toFloat();
    //gb_LineAttent = LineattenValue->text().toFloat();
    //gb_QuadAttent = QuadattenValue->text().toFloat();

    int lighttype = lightType->currentIndex();
    emit ApplyLightSetting(lighttype);
}

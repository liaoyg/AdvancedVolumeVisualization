#include "DatasetSelect.h"
#include <QLayout>
#include <QLineEdit>

DatasetSelect::DatasetSelect(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *vDimLayout = new QGridLayout;
    vDimLayout->setColumnStretch(0, 1);
    vDimLayout->setColumnStretch(1, 2);
    vDimLayout->setColumnStretch(2, 1);
    vDimLayout->setColumnStretch(3, 2);
    vDimLayout->setColumnStretch(4, 1);
    vDimLayout->setColumnStretch(5, 2);

    QLabel *vDimX = new QLabel(QWidget::tr("Dim.x:"));
    xEditor = new QLineEdit;
    xEditor->setValidator(new QIntValidator(0, 2048, this));
    QLabel *vDimY = new QLabel(QWidget::tr("Dim.y:"));
    yEditor = new QLineEdit;
    yEditor->setValidator(new QIntValidator(0, 2048, this));
    QLabel *vDimZ = new QLabel(QWidget::tr("Dim.z:"));
    zEditor = new QLineEdit;
    zEditor->setValidator(new QIntValidator(0, 2048, this));

    vDimLayout->addWidget(vDimX,0,0);
    vDimLayout->addWidget(xEditor,0,1);
    vDimLayout->addWidget(vDimY,0,2);
    vDimLayout->addWidget(yEditor,0,3);
    vDimLayout->addWidget(vDimZ,0,4);
    vDimLayout->addWidget(zEditor,0,5);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setColumnStretch(0, 1);
    gridLayout->setColumnStretch(1, 3);
    gridLayout->setColumnStretch(2, 1);

    gridLayout->setMargin(10);

    datasetSelect = new QLabel(QWidget::tr("Dataset:"));
    datasetFile = new QComboBox();

    datasetFile->addItem(QWidget::tr("aneurism.raw"));
    datasetFile->addItem(QWidget::tr("bonsai.raw"));
    datasetFile->addItem(QWidget::tr("BostonTeapot.raw"));
    datasetFile->addItem(QWidget::tr("SampleVolume.raw"));
    datasetFile->addItem(QWidget::tr("skull.raw"));
    //datasetFile->insertItem(2, tr("Insert item"));
    //datasetFile->insertSeparator(2);
    applyBtn = new QPushButton(QWidget::tr("Select"));
    applyBtn->setFixedSize(80,33);

    gridLayout->addWidget(datasetSelect, 0, 0);
    gridLayout->addWidget(datasetFile, 0, 1);
    gridLayout->addWidget(applyBtn);
    //bomLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(vDimLayout);
    mainLayout->addLayout(gridLayout);
    mainLayout->setMargin(3);
    this->setLayout(mainLayout);

    //Set aneurism default volume dimension
    xEditor->setText("256");
    yEditor->setText("256");
    zEditor->setText("256");

    connect(applyBtn, SIGNAL(clicked()), this, SLOT(GetDatasetName()));
    connect(datasetFile, SIGNAL(currentIndexChanged(int)), this, SLOT(setDefaultDim(int)));
}

void DatasetSelect::GetDatasetName()
{
    QString str;
    str = datasetFile->currentText();
    Vec3i vDim;
    vDim.x = xEditor->text().toInt();
    vDim.y = yEditor->text().toInt();
    vDim.z = zEditor->text().toInt();
    emit DatasetSelected(str, vDim);
}

void DatasetSelect::setDefaultDim(int index)
{
    switch(index)
    {
        case 0:
        {
            xEditor->setText("256");
            yEditor->setText("256");
            zEditor->setText("256");
            break;
        }
        case 1:
        {
            xEditor->setText("256");
            yEditor->setText("256");
            zEditor->setText("256");
            break;
        }
        case 2:
        {
            xEditor->setText("256");
            yEditor->setText("256");
            zEditor->setText("176");
            break;
        }
        case 3:
        {
            xEditor->setText("32");
            yEditor->setText("32");
            zEditor->setText("32");
            break;
        }
        case 4:
        {
            xEditor->setText("256");
            yEditor->setText("256");
            zEditor->setText("256");
            break;
        }
        default:
        {
            xEditor->setText("256");
            yEditor->setText("256");
            zEditor->setText("256");
            break;
        }
    }
}

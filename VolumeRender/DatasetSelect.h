#ifndef DATASETSELECT_H
#define DATASETSELECT_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include "Mat.h"

class DatasetSelect : public QWidget
{
    Q_OBJECT
public:
    explicit DatasetSelect(QWidget *parent = 0);

private:
    QLabel *datasetSelect;
    QComboBox *datasetFile;
    QPushButton *applyBtn;

    QLineEdit *xEditor;
    QLineEdit *yEditor;
    QLineEdit *zEditor;

signals:
    void DatasetSelected(QString,Vec3i);

public slots:
    void GetDatasetName();
    void setDefaultDim(int index);

};

#endif // DATASETSELECT_H

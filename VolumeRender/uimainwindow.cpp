#include "uimainwindow.h"
#include "ui_mainwindow.h"

UIMainWindow::UIMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UIMainWindow)
{
    ui->setupUi(this);
}

UIMainWindow::~UIMainWindow()
{
    delete ui;
}

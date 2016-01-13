#ifndef UIMAINWINDOW_H
#define UIMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class UIMainWindow;
}

class UIMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit UIMainWindow(QWidget *parent = 0);
    ~UIMainWindow();

private:
    Ui::UIMainWindow *ui;
};

#endif // UIMAINWINDOW_H

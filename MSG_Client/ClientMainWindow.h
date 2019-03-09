#ifndef CLIENTMAINWINDOW_H
#define CLIENTMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class TClientMainWindow;
}

class TClientMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TClientMainWindow(QWidget *parent = 0);
    ~TClientMainWindow();

private:
    Ui::TClientMainWindow *ui;

};

#endif // CLIENTMAINWINDOW_H

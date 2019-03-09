#ifndef SERVERMAINWINDOW_H
#define SERVERMAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

namespace Ui {
class TServerMainWindow;
}

class TServerMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TServerMainWindow(QWidget *parent = 0);
    ~TServerMainWindow();

private:
    Ui::TServerMainWindow *ui;

    QLabel fServerHostLable;
    QLabel fServerPortLable;

    void init();
    void Link();

public slots:

private slots:
    void slot_UpdateColumnsSize();
};

#endif // SERVERMAINWINDOW_H

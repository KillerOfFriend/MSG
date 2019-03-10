#ifndef SERVERMAINWINDOW_H
#define SERVERMAINWINDOW_H

#include <memory>

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

    std::unique_ptr<QLabel> fServerHostLable = nullptr;
    std::unique_ptr<QLabel> fServerPortLable = nullptr;

    void init();
    void Link();

public slots:

private slots:
    void slot_UpdateColumnsSize();
};

#endif // SERVERMAINWINDOW_H

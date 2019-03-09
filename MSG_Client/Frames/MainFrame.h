#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QWidget>

namespace Ui {
class TfmeMainFrame;
}

class TfmeMainFrame : public QWidget
{
    Q_OBJECT

public:
    explicit TfmeMainFrame(QWidget *parent = 0);
    ~TfmeMainFrame();

private:
    Ui::TfmeMainFrame *ui;
};

#endif // MAINFRAME_H

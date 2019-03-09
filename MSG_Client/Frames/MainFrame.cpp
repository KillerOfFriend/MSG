#include "MainFrame.h"
#include "ui_MainFrame.h"

TfmeMainFrame::TfmeMainFrame(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TfmeMainFrame)
{
    ui->setupUi(this);
}

TfmeMainFrame::~TfmeMainFrame()
{
    delete ui;
}

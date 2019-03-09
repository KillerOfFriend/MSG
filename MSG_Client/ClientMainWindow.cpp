#include "ClientMainWindow.h"
#include "ui_ClientMainWindow.h"

#include <QGridLayout>

#include "Classes/DataModule/DataModule.h"

//-----------------------------------------------------------------------------
TClientMainWindow::TClientMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TClientMainWindow)
{
    ui->setupUi(this);

    this->setCentralWidget(TDM::Instance().FrameController()->MainFrame().get());
}
//-----------------------------------------------------------------------------
TClientMainWindow::~TClientMainWindow()
{
    delete ui;
}
//-----------------------------------------------------------------------------

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
    this->centralWidget()->setParent(nullptr); // Задаём предка (Это запретит форме удаление фиджета из умного указателя, что привело бы к ошибке)
    delete ui;
}
//-----------------------------------------------------------------------------

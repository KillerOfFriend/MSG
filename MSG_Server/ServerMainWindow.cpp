#include "ServerMainWindow.h"
#include "ui_ServerMainWindow.h"

#include <QDateTime>

#include "Classes/DataModule/DataModule.h"
#include "Classes/Settings/Settings.h"

//-----------------------------------------------------------------------------
TServerMainWindow::TServerMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TServerMainWindow)
{
    ui->setupUi(this);

    init();
    Link();
}
//-----------------------------------------------------------------------------
TServerMainWindow::~TServerMainWindow()
{
    delete ui;
}
//-----------------------------------------------------------------------------
void TServerMainWindow::init()
{
    TDM &DM = TDM::Instance();

    fServerHostLable.reset(new QLabel("Host IP: " + DM.Server()->serverAddress().toString(), this));
    fServerPortLable.reset(new QLabel("Port: " + QString::number(DM.Server()->serverPort()), this));

    ui->statusBar->addWidget(fServerHostLable.get());
    ui->statusBar->addWidget(fServerPortLable.get());

    ui->LogTableView->setModel(DM.LogModel().get());
    ui->LogTableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->UsersTableView->setModel(DM.Server()->clientsModel());

    DM.LogModel()->slot_NewMessage(QHostAddress(), "=== Сервер запущен ===");
    slot_UpdateColumnsSize();
}
//-----------------------------------------------------------------------------
void TServerMainWindow::Link()
{
    connect(ui->LogTableView->model(), &QAbstractItemModel::rowsInserted, ui->LogTableView, &QTableView::scrollToBottom);
    connect(ui->LogTableView->model(), &QAbstractItemModel::rowsInserted, this, &TServerMainWindow::slot_UpdateColumnsSize);
    connect(ui->UsersTableView->model(), &QAbstractItemModel::rowsInserted, this, &TServerMainWindow::slot_UpdateColumnsSize);
}
//-----------------------------------------------------------------------------
void TServerMainWindow::slot_UpdateColumnsSize()
{
    for (quint32 Index = TServerLogModel::firstColumnIndex; Index <= TServerLogModel::lastColumnIndex; ++Index)
        ui->LogTableView->horizontalHeader()->setSectionResizeMode(Index, QHeaderView::ResizeToContents);

    for (quint32 Index = TConnectedUsersModel::firstColumnIndex; Index <= TConnectedUsersModel::lastColumnIndex; ++Index)
        ui->UsersTableView->horizontalHeader()->setSectionResizeMode(Index, QHeaderView::ResizeToContents);
}
//-----------------------------------------------------------------------------

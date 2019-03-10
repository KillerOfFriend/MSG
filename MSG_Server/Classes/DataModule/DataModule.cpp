#include "DataModule.h"

#include <QDebug>

//-----------------------------------------------------------------------------
TDM::TDM(QObject *inParent) : QObject(inParent)
{
    init();
}
//-----------------------------------------------------------------------------
TDM::~TDM()
{

}
//-----------------------------------------------------------------------------
TDM& TDM::Instance()
{
    static TDM DM;
    return DM;
}
//-----------------------------------------------------------------------------
std::shared_ptr<TMSGServer> TDM::Server() // Метод вернёт указатель на объект-сервер
{ return fServer; }
//-----------------------------------------------------------------------------
std::shared_ptr<TServerLogModel> TDM::LogModel() // Метод вернёт указатель на модель логов
{ return fLogModel; }
//-----------------------------------------------------------------------------
void TDM::init() // Мотод инициализирует классы модуля данных
{
    fServer.reset(new TMSGServer(this));
    fLogModel.reset(new TServerLogModel(this));

    connect(fServer.get(), &TMSGServer::sig_LogMessage, fLogModel.get(), &TServerLogModel::slot_NewMessage);
}
//-----------------------------------------------------------------------------

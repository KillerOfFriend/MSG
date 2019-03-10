#include "DataModule.h"
#include <QFile>
#include <QDebug>
#include <QSettings>

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
    static TDM Inst;
    return Inst;
}
//-----------------------------------------------------------------------------
std::shared_ptr<TMSGClient> TDM::Client()
{ return fClient; }
//-----------------------------------------------------------------------------
std::shared_ptr<TUserAccount> TDM::UserAccount()
{ return fUserAccount; }
//-----------------------------------------------------------------------------
std::shared_ptr<TFrameController> TDM::FrameController()
{ return fFrameController; }
//-----------------------------------------------------------------------------
void TDM::init() /// Мотод инициализирует классы модуля данных
{
    fClient = std::make_shared<TMSGClient>(this);
    fUserAccount = std::make_shared<TUserAccount>(this);
    fFrameController = std::make_shared<TFrameController>(this);

    connect(fClient.get()->ComandExecutor.get(), &TComandExecutor::sig_SetUserInfo, fUserAccount.get(), &TUserAccount::slot_SetUserInfo); // Передача информации о пользователе
    connect(fClient.get()->ComandExecutor.get(), &TComandExecutor::sig_GetContactsResult, fUserAccount.get(), &TUserAccount::slot_SetContacts); // Передача контактов пользователя
}
//-----------------------------------------------------------------------------

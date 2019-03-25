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
std::shared_ptr<Users::TUserAccount> TDM::UserAccount()
{ return fUserAccount; }
//-----------------------------------------------------------------------------
std::shared_ptr<TFrameController> TDM::FrameController()
{ return fFrameController; }
//-----------------------------------------------------------------------------
std::shared_ptr<TModels> TDM::Models()
{ return fModels; }
//-----------------------------------------------------------------------------
void TDM::init() // Мотод инициализирует классы модуля данных
{
    fClient = std::make_shared<TMSGClient>(this);
    //fUserAccount = std::make_shared<Users::TUserAccount>(this);
    fFrameController = std::make_shared<TFrameController>(this);
    fModels = std::make_shared<TModels>(this);

    connect(fClient.get(), &TMSGClient::sig_LogMessage, fModels->LogModel().get(), &TClientLogModel::slot_NewMessage); // Логирование

//    connect(fClient.get(), &TMSGClient::sig_SetUserInfo, fUserAccount.get(), &Users::TUserAccount::slot_SetUserInfo); // Передача информации о пользователе
//    connect(fClient.get(), &TMSGClient::sig_SetContacts, fUserAccount.get(), &Users::TUserAccount::slot_SetContacts); // Передача информации о контактах пользователя
//    connect(fClient.get(), &TMSGClient::sig_SetChats, fUserAccount.get(), &Users::TUserAccount::slot_SetChats); // Передача информации о беседах

    connect(fClient.get(), &TMSGClient::sig_ContactChangeStatus, fUserAccount.get(), &Users::TUserAccount::slot_ContactChangeStatus); // Передача изменнённого статуса контакта
    connect(fClient.get(), &TMSGClient::sig_GetUserTypesResult, fModels.get(), &TModels::slot_InitUserTypes); // Передача списка типов пользователей

    connect(fClient.get(), &TMSGClient::sig_InviteToChatResult, fUserAccount.get(), &Users::TUserAccount::slot_AddChat); // Добавление новой беседы
}
//-----------------------------------------------------------------------------
void TDM::slot_SetUserAccount(Users::TUserAccount &inUserAccount)
{
    fUserAccount = std::make_shared<Users::TUserAccount>(inUserAccount);
}
//-----------------------------------------------------------------------------

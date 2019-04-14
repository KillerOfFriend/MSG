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
//std::shared_ptr<Core::TUserAccount> TDM::UserAccount()
//{ return fUserAccount; }
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
    connect(fClient.get(), &TMSGClient::sig_GetUserTypesResult, fModels.get(), &TModels::slot_InitUserTypes); // Передача списка типов пользователей


//    // Создаём "Анонимный" аккаунт
//    Core::TUserAccount NewAccount(this);
//    Core::UserInfo_Ptr AnonimusInfo = std::make_shared<Core::TUserInfo>(this);
//    // Задаём параметры анонимного не авторизированног опльзователя
//    AnonimusInfo->setUserType(0);
//    AnonimusInfo->setUserUuid(QUuid());
//    AnonimusInfo->setUserLogin("Anonimus");
//    AnonimusInfo->setUserName("Anonimus");

//    if (fClient->clientSocket())
//        NewAccount.setSocket(fClient->clientSocket().get()); // Задаём сокет
//    slot_SetUserAccount(NewAccount); // Задаём анонимный аккаунт
}
//-----------------------------------------------------------------------------
//void TDM::slot_SetUserAccount(Core::TUserAccount &inUserAccount)
//{
//    fUserAccount = std::make_shared<Core::TUserAccount>(inUserAccount); // Переназначаем аккаунт
//    // Линкуем к нему сигналы
//    connect(fClient.get(), &TMSGClient::sig_ContactChangeStatus, fUserAccount.get(), &Core::TUserAccount::slot_ContactChangeStatus); // Передача изменнённого статуса контакта
//    connect(fClient.get(), &TMSGClient::sig_InviteToChatResult, fUserAccount.get(), &Core::TUserAccount::slot_AddChat); // Добавление новой беседы
//    connect(fUserAccount.get(), &Core::TUserAccount::sig_ComandReadyToExecute, fClient.get(), &TMSGClient::executCommand); // Запуск выполнения полученой команды
//}
//-----------------------------------------------------------------------------

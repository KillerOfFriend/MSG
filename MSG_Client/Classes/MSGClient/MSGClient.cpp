#include "MSGClient.h"

#include <QFile>
#include <QBuffer>
#include <QSettings>
#include <QByteArray>
#include <QDataStream>
#include <QHostAddress>
#include <QApplication>

#include <QCryptographicHash>

#include "Classes/DataModule/DataModule.h"
#include "Classes/Settings/Settings.h"
#include "Classes/ChatMessage/ChatMessage.h"
#include "Classes/DataPacker/DataPacker.h"

#include "comandes.h"
#include "resultcodes.h"

//-----------------------------------------------------------------------------
TMSGClient::TMSGClient(QObject *inParent) : Core::TCommandSystem(inParent)
{
    init();
    Link();
}
//-----------------------------------------------------------------------------
TMSGClient::~TMSGClient()
{
    if (isConnected())
    {
        fClient->disconnectFromHost();
        fClient->waitForDisconnected();
    }
}
//-----------------------------------------------------------------------------
void TMSGClient::init()
{
    fClient = std::make_shared<QTcpSocket>(new QTcpSocket(this));

    // Создаём "Анонимный" аккаунт
    Core::TUserAccount NewAccount(this);
    Core::UserInfo_Ptr AnonimusInfo = std::make_shared<Core::TUserInfo>(this);
    // Задаём параметры анонимного не авторизированног опльзователя
    AnonimusInfo->setUserType(0);
    AnonimusInfo->setUserUuid(QUuid());
    AnonimusInfo->setUserLogin("Anonimus");
    AnonimusInfo->setUserName("Anonimus");

    if (fClient)
        NewAccount.setSocket(fClient.get()); // Задаём сокет
    slot_SetUserAccount(NewAccount); // Задаём анонимный аккаунт
}
//-----------------------------------------------------------------------------
void TMSGClient::Link()
{
    connect(fClient.get(), &QTcpSocket::readyRead, this, &TMSGClient::slot_ReadyRead);

    connect(fClient.get(), &QTcpSocket::hostFound, this, &TMSGClient::slot_hostFound);
    connect(fClient.get(), &QTcpSocket::connected, this, &TMSGClient::slot_connected);
    connect(fClient.get(), &QTcpSocket::disconnected, this, &TMSGClient::slot_disconnected);
}
//-----------------------------------------------------------------------------
bool TMSGClient::connectToServer()
{
    if (!isConnected())
    {
        fClient->connectToHost(TSettings::Instance().ServerAddress(), TSettings::Instance().ServerPort());

        if (!fClient->waitForConnected() || !isConnected())
            qDebug() << "[ОШИБКА] Не удалось подключиться к серверу!";
        else
            qDebug() << "[УСПЕХ] Подключение к серверу прошло успешно.";
    }
    else
        qDebug() << "[ИНФО] Соединение уже установлено.";

    return isConnected();
}
//-----------------------------------------------------------------------------
void TMSGClient::disconnect()
{
    fClient->close();
}
//-----------------------------------------------------------------------------
bool TMSGClient::isConnected()
{ return fClient->state() == QAbstractSocket::ConnectedState; }
//-----------------------------------------------------------------------------
std::shared_ptr<QTcpSocket> TMSGClient::clientSocket() // Метод вернёт сокет клиента
{ return fClient; }
//-----------------------------------------------------------------------------
std::shared_ptr<Core::TUserAccount> TMSGClient::userAccount()
{ return fUserAccount; }
//-----------------------------------------------------------------------------
bool TMSGClient::createUser(QString inLogin, QString inPassword, QString inName, bool inIsMale) /// Метод отправит команду на создание пользователья
{
    bool Result = true;

    if(!isConnected())
        Result = false;
    else
    {
        QByteArray SendingData;
        QDataStream Stream(&SendingData, QIODevice::WriteOnly);
        Core::TDataPacker DataPacker(this);

        QByteArray PasswordHash = QCryptographicHash::hash(inPassword.toUtf8(), QCryptographicHash::Md5);
//        Stream << Commands::CreateUser << inLogin.toLower().toUtf8() << PasswordHash << inName.toUtf8() << inIsMale;
        Result = DataPacker.makePackage(Commands::CreateUser, Stream, inLogin.toLower().toUtf8(), PasswordHash, inName.toUtf8(), inIsMale);

        if (Result) // Если данные корректно запакованы
            fClient->write(SendingData); // Пишем их в поток
        //Result = fClient->waitForBytesWritten();
    }

    return Result;
}
//-----------------------------------------------------------------------------
bool TMSGClient::authorization(QString inLogin, QString inPassword) // Метод отправит номанду на авторизацию пользователя
{
    bool Result = true;

    if(!isConnected())
        Result = false;
    else
    {
        QByteArray SendingData;
        QDataStream Stream(&SendingData, QIODevice::WriteOnly);
        Core::TDataPacker DataPacker(this);

        QByteArray PasswordHash = QCryptographicHash::hash(inPassword.toUtf8(), QCryptographicHash::Md5);
        //Stream << Commands::Authorization << inLogin.toLower().toUtf8() << PasswordHash;
        Result = DataPacker.makePackage(Commands::Authorization, Stream, inLogin.toLower().toUtf8(), PasswordHash);

        if (Result) // Если данные корректно запакованы
            fClient->write(SendingData); // Пишем их в поток
        //Result = fClient->waitForBytesWritten();
    }

    return Result;
}
//-----------------------------------------------------------------------------
bool TMSGClient::getUserTypes() // Метод отправит запрос получение списка типов пользователей
{
    bool Result = true;

    if(!isConnected())
        Result = false;
    else
    {
        QByteArray SendingData;
        QDataStream Stream(&SendingData, QIODevice::WriteOnly);
        Core::TDataPacker DataPacker(this);

        //Stream << Commands::GetUserTypes;
        Result = DataPacker.makePackage(Commands::GetUserTypes, Stream, 0);

        if (Result) // Если данные корректно запакованы
            fClient->write(SendingData); // Пишем их в поток
    }

    return Result;
}
//-----------------------------------------------------------------------------
bool TMSGClient::findUsers(QString inUserName) // Метод отправит команду на поиск пользователей
{
    bool Result = true;

    if(!isConnected())
        Result = false;
    else
    {
        sig_LogMessage("Отправлен запрос на поиск пользователей по фильтру (" + inUserName + ")");

        QByteArray SendingData;
        QDataStream Stream(&SendingData, QIODevice::WriteOnly);
        Core::TDataPacker DataPacker(this);

        //Stream << Commands::FindUsers << inUserName.toLower().toUtf8();
        Result = DataPacker.makePackage(Commands::FindUsers, Stream, inUserName.toLower().toUtf8());

        if (Result) // Если данные корректно запакованы
            fClient->write(SendingData); // Пишем их в поток
    }

    return Result;
}
//-----------------------------------------------------------------------------
bool TMSGClient::addContact(QUuid inSelfUuid, QUuid inContactUuid) // Метод отправит команду на добавление контакта
{
    bool Result = true;

    if(!isConnected())
        Result = false;
    else
    {
        sig_LogMessage("Отправлен запрос на добавление пользоватея в контакты");

        QByteArray SendingData;
        QDataStream Stream(&SendingData, QIODevice::WriteOnly);
        Core::TDataPacker DataPacker(this);

        //Stream << Commands::AddContact << inSelfUuid << inContactUuid;
        Result = DataPacker.makePackage(Commands::AddContact, Stream, inSelfUuid, inContactUuid);

        if (Result) // Если данные корректно запакованы
            fClient->write(SendingData); // Пишем их в поток
    }

    return Result;
}
//-----------------------------------------------------------------------------
bool TMSGClient::deleteContact(QUuid inSelfUuid, QUuid inContactUuid) // Метод отправит команду на удаление контакта
{
    bool Result = true;

    if(!isConnected())
        Result = false;
    else
    {
        sig_LogMessage("Отправлен запрос на удаление пользоватея из контактов");

        QByteArray SendingData;
        QDataStream Stream(&SendingData, QIODevice::WriteOnly);
        Core::TDataPacker DataPacker(this);

        //Stream << Commands::DeleteContact << inSelfUuid << inContactUuid;
        Result = DataPacker.makePackage(Commands::DeleteContact, Stream, inSelfUuid, inContactUuid);

        if (Result) // Если данные корректно запакованы
            fClient->write(SendingData); // Пишем их в поток
    }

    return Result;
}
//-----------------------------------------------------------------------------
bool TMSGClient::createChat(Core::TChatInfo &inChatInfo) // Метод отправит команду на создание беседы
{
    bool Result = true;

    if(!isConnected())
        Result = false;
    else
    {
        sig_LogMessage("Отправлен запрос на создание беседы");

        QByteArray SendingData;
        QDataStream Stream(&SendingData, QIODevice::WriteOnly);
        Core::TDataPacker DataPacker(this);

        //Stream << Commands::CreateChat << inChatInfo;
        Result = DataPacker.makePackage(Commands::CreateChat, Stream, inChatInfo);

        if (Result) // Если данные корректно запакованы
            fClient->write(SendingData); // Пишем их в поток
    }

    return Result;
}
//-----------------------------------------------------------------------------
bool TMSGClient::leaveFromChat(const QUuid inChatUuid) // Метод удалит беседу (По факту, удалит из беседы пользователя)
{
    bool Result = true;

    if(!isConnected())
        Result = false;
    else
    {
        sig_LogMessage("Отправлен запрос на выход из беседы");

        QByteArray SendingData;
        QDataStream Stream(&SendingData, QIODevice::WriteOnly);
        Core::TDataPacker DataPacker(this);

        //Stream << Commands::ILeaveFromChat << inChatUuid << TDM::Instance().UserAccount()->userInfo()->userUuid(); // Шлём ID беседы и свой Uuid
        Result = DataPacker.makePackage(Commands::ILeaveFromChat, Stream, inChatUuid, fUserAccount->userInfo()->userUuid());

        if (Result) // Если данные корректно запакованы
            fClient->write(SendingData); // Пишем их в поток
    }

    return Result;
}
//-----------------------------------------------------------------------------
bool TMSGClient::sendMessage(QUuid inChatUuid, Core::TChatMessage &inMessage) // Метод отправит сообщение
{
    bool Result = true;

    if(!isConnected())
        Result = false;
    else
    {
        sig_LogMessage("Сообщение отправлено");

//        QByteArray SendingData;
//        QDataStream Stream(&SendingData, QIODevice::WriteOnly);
//        Stream << Commands::SendMessage << inChatUuid << inMessage; // Шлём ID беседы и сообщение

//        fClient->write(SendingData);
//        Result = fClient->waitForBytesWritten();

        QByteArray SendingData;
        QDataStream Stream(&SendingData, QIODevice::WriteOnly);
        Core::TDataPacker DataPacker(this);

        //Stream << Commands::SendMessage << inChatUuid << inMessage; // Шлём ID беседы и сообщение
        Result = DataPacker.makePackage(Commands::SendMessage, Stream,  inChatUuid, inMessage);

        if (Result) // Если данные корректно запакованы
            fClient->write(SendingData); // Пишем их в поток
    }

    return Result;
}
//-----------------------------------------------------------------------------
// Slots
//-----------------------------------------------------------------------------
void TMSGClient::slot_ReadyRead()
{
    //executCommand(fClient.get());

    if (fClient && fUserAccount)
        fUserAccount->readData();
}
//-----------------------------------------------------------------------------
void TMSGClient::slot_hostFound()
{ qDebug() << "slot_hostFound"; }
//-----------------------------------------------------------------------------
void TMSGClient::slot_connected()
{ qDebug() << "slot_hostFound"; }
//-----------------------------------------------------------------------------
void TMSGClient::slot_disconnected()
{
    qDebug() << "slot_disconnected";
    sig_LogMessage("Соеденение разорвано");
}
//-----------------------------------------------------------------------------
void TMSGClient::slot_SetUserAccount(Core::TUserAccount &inUserAccount)
{
    fUserAccount = std::make_shared<Core::TUserAccount>(inUserAccount); // Переназначаем аккаунт
    // Линкуем к нему сигналы
    connect(this, &TMSGClient::sig_ContactChangeStatus, fUserAccount.get(), &Core::TUserAccount::slot_ContactChangeStatus); // Передача изменнённого статуса контакта
    connect(this, &TMSGClient::sig_InviteToChatResult, fUserAccount.get(), &Core::TUserAccount::slot_AddChat); // Добавление новой беседы
    connect(fUserAccount.get(), &Core::TUserAccount::sig_ComandReadyToExecute, this, &TMSGClient::executCommand); // Запуск выполнения полученой команды
}
//-----------------------------------------------------------------------------

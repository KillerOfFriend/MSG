#include "MSGClient.h"

#include <QFile>
#include <QBuffer>
#include <QSettings>
#include <QByteArray>
#include <QDataStream>
#include <QHostAddress>
#include <QApplication>

#include <QCryptographicHash>

#include "Classes/Settings/Settings.h"
#include "comandes.h"
#include "resultcodes.h"

//-----------------------------------------------------------------------------
TMSGClient::TMSGClient(QObject *inParent) : QObject(inParent)
{
    init();
    Link();
}
//-----------------------------------------------------------------------------
TMSGClient::~TMSGClient()
{
    if (isConnected())
        fClient->disconnectFromHost();
}
//-----------------------------------------------------------------------------
void TMSGClient::init()
{
    fClient.reset(new QTcpSocket(this));
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
bool TMSGClient::createUser(QString inLogin, QString inPassword, QString inName, bool inIsMale) /// Метод отправит команду на создание пользователья
{
    bool Result = true;

    if(!isConnected())
        Result = false;
    else
    {
        QByteArray SendingData;
        QDataStream Stream(&SendingData, QIODevice::WriteOnly);

        QByteArray PasswordHash = QCryptographicHash::hash(inPassword.toUtf8(), QCryptographicHash::Md5);
        Stream << Commands::CreateUser << inLogin.toLower().toUtf8() << PasswordHash << inName.toUtf8() << inIsMale;

        fClient->write(SendingData);
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

        QByteArray PasswordHash = QCryptographicHash::hash(inPassword.toUtf8(), QCryptographicHash::Md5);
        Stream << Commands::Authorization << inLogin.toLower().toUtf8() << PasswordHash;

        fClient->write(SendingData);
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

        Stream << Commands::GetUserTypes;
        fClient->write(SendingData);
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
        Stream << Commands::FindUsers << inUserName.toLower().toUtf8();

        fClient->write(SendingData);
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
        Stream << Commands::AddContact << inSelfUuid << inContactUuid;

        fClient->write(SendingData);
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
        Stream << Commands::DeleteContact << inSelfUuid << inContactUuid;

        fClient->write(SendingData);
    }

    return Result;
}
//-----------------------------------------------------------------------------
bool TMSGClient::createChat(Users::TChatInfo &inChatInfo) // Метод отправит команду на создание беседы
{
    bool Result = true;

    if(!isConnected())
        Result = false;
    else
    {
        sig_LogMessage("Отправлен запрос на создание беседы");

        QByteArray SendingData;
        QDataStream Stream(&SendingData, QIODevice::WriteOnly);
        Stream << Commands::CreateChat << inChatInfo;

        fClient->write(SendingData);
    }

    return Result;
}
//-----------------------------------------------------------------------------
void TMSGClient::slot_ReadyRead()
{
    executCommand(fClient.get());
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

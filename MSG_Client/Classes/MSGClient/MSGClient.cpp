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
    ComandExecutor.reset(new TComandExecutor(this));
}
//-----------------------------------------------------------------------------
void TMSGClient::Link()
{
    connect(fClient.get(), &QTcpSocket::readyRead, this, &TMSGClient::slot_ReadyRead);
    connect(fClient.get(), &QTcpSocket::channelReadyRead, this, &TMSGClient::slot_ChannelReadyRead);
    connect(fClient.get(), &QTcpSocket::bytesWritten, this, &TMSGClient::slot_BytesWritten);

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
bool TMSGClient::createUser(QString inLogin, QString inPassword) /// Метод отправит команду на создание пользователья
{
    bool Result = true;

    if(isConnected())
    {
        QByteArray SendingData;
        QDataStream Stream(&SendingData, QIODevice::WriteOnly);

        QByteArray PasswordHash = QCryptographicHash::hash(inPassword.toUtf8(), QCryptographicHash::Md5);
        Stream << Commands::CreateUser << inLogin.toLower().toUtf8() << PasswordHash;

        fClient->write(SendingData);
        //Result = fClient->waitForBytesWritten();
    }
    else Result = false;

    return Result;
}
//-----------------------------------------------------------------------------
bool TMSGClient::authorization(QString inLogin, QString inPassword) // Метод отправит номанду на авторизацию пользователя
{
    bool Result = true;

    if(isConnected())
    {
        QByteArray SendingData;
        QDataStream Stream(&SendingData, QIODevice::WriteOnly);

        QByteArray PasswordHash = QCryptographicHash::hash(inPassword.toUtf8(), QCryptographicHash::Md5);
        Stream << Commands::Authorization << inLogin.toLower().toUtf8() << PasswordHash;

        fClient->write(SendingData);
        //Result = fClient->waitForBytesWritten();
    }
    else Result = false;

    return Result;
}
//-----------------------------------------------------------------------------
void TMSGClient::slot_ReadyRead()
{
    ComandExecutor->executCommand(fClient.get());
}
//-----------------------------------------------------------------------------
void TMSGClient::slot_ChannelReadyRead(int channel)
{
    qDebug() << "slot_ChannelReadyRead " + QString::number(channel);
}
//-----------------------------------------------------------------------------
void TMSGClient::slot_BytesWritten(qint64 bytes)
{
    qDebug() << "slot_BytesWritten " + QString::number(bytes);
}
//-----------------------------------------------------------------------------
void TMSGClient::slot_hostFound()
{ qDebug() << "slot_hostFound"; }
//-----------------------------------------------------------------------------
void TMSGClient::slot_connected()
{ qDebug() << "slot_hostFound"; }
//-----------------------------------------------------------------------------
void TMSGClient::slot_disconnected()
{ qDebug() << "slot_disconnected"; }
//-----------------------------------------------------------------------------

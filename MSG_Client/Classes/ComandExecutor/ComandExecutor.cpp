#include "ComandExecutor.h"

#include <QCryptographicHash>
#include <QHostAddress>

#include "comandes.h"
#include "resultcodes.h"

//-----------------------------------------------------------------------------
TComandExecutor::TComandExecutor(QObject *inParent) : QObject(inParent)
{

}
//-----------------------------------------------------------------------------
void TComandExecutor::executCommand(QTcpSocket* inClientSender)
{
    if (!inClientSender)
        return;

    QByteArray All = inClientSender->readAll(); // Получаем полный набор данных от клиента
    QDataStream inStream(&All, QIODevice::ReadOnly); // Оборачиваем его в поток данных

    quint8 Command; // Команда от клиента
    inStream >> Command; // Получаем команду

    switch (Command) // Проверяем команду
    {
        case Commands::CreateUser: // Регистрация пользователя
        {
            sig_LogMessage("Получен ответ на создание пользователя" + inClientSender->localAddress().toString());
            creteUserResult(inStream); // Обработатьм результат регистрации пользователя
            break;
        }
        case Commands::Authorization: // Авторизация пользователя
        {
            sig_LogMessage("Получен ответ на авторизацию" + inClientSender->localAddress().toString());
            userAuthorization(inStream); // Обработать результат авторизации
            break;
        }

        default: sig_LogMessage("Получена неизвестная команда от " + inClientSender->localAddress().toString());
    }
}
//-----------------------------------------------------------------------------
void TComandExecutor::creteUserResult(QDataStream &inDataStream)
{
    qint32 Result = Res::rUnknown;
    inDataStream >> Result; // Получаем результат выполнения
    sig_UserCreateResult(Result); // Шлём сигнал с результатом создания пользователя
}
//-----------------------------------------------------------------------------
void TComandExecutor::userAuthorization(QDataStream &inDataStream)
{
    qint32 Result = Res::rUnknown;
    TUserInfo UserInfo;

    inDataStream >> Result; // Получаем результат выполнения
    if (Result == Res::CanAut::caAuthorizationTrue) // Если авторизация прошла успешно
        inDataStream >> UserInfo; // Получаем информацию о пользователе

    sig_SetUserInfo(UserInfo); // Шлём сигнал с данными пользователя
    sig_AuthorizationResult(Result); // Шлём сигнал с результатом авторизации
}
//-----------------------------------------------------------------------------
QString TComandExecutor::ReadStringFromStream(QDataStream &inDataStream)
{
    QByteArray ByteBuf;
    inDataStream >> ByteBuf;

    return QString::fromUtf8(ByteBuf); // Помещаем буфер в результат (с раскодировкой из UTF8)
}
//-----------------------------------------------------------------------------

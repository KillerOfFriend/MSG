#include "ComandExecutor.h"

#include <QCryptographicHash>
#include <QHostAddress>

#include "comandes.h"
#include "resultcodes.h"
#include "Classes/UserInfo/UserInfo.h"

//-----------------------------------------------------------------------------
TComandExecutor::TComandExecutor(QObject *inParent) : QObject(inParent)
{

}
//-----------------------------------------------------------------------------
/**
 * @brief TComandExecutor::executCommand - Метод исполняющий команды
 * @param inClientSender - Сокет - получаетль
 */
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
            sig_LogMessage("Получен ответ на создание пользователя" + inClientSender->peerAddress().toString());
            creteUserResult(inStream); // Обработатьм результат регистрации пользователя
            break;
        }
        case Commands::Authorization: // Авторизация пользователя
        {
            sig_LogMessage("Получен ответ на авторизацию" + inClientSender->peerAddress().toString());
            userAuthorization(inStream); // Обработать результат авторизации
            break;
        }
        case Commands::FindUsers: // Поиск пользователей
        {
            sig_LogMessage("Получен ответ на поиск пользователей" + inClientSender->peerAddress().toString());
            findUsersResult(inStream); // Обработать результат поиска пользователей
            break;
        }
        case Commands::AddContact: // Добавление котнакта
        {
            sig_LogMessage("Получен ответ на добавление пользователя" + inClientSender->peerAddress().toString());
            addContactResult(inStream); // Обработать результат добавления контакта
            break;
        }
        case Commands::GetContacts: // Запрос списка контактов
        {
            sig_LogMessage("Получен ответ на запрос списка контактов" + inClientSender->peerAddress().toString());
            getContactsResult(inStream); // Обработать результат запроса списка контактов
            break;
        }

        default: sig_LogMessage("Получена неизвестная команда от " + inClientSender->peerAddress().toString());
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TComandExecutor::creteUserResult - Мтод обработает результат регистрации
 * @param inDataStream - Входящий поток
 */
void TComandExecutor::creteUserResult(QDataStream &inDataStream)
{
    qint32 Result = Res::rUnknown;
    inDataStream >> Result; // Получаем результат выполнения
    sig_UserCreateResult(Result); // Шлём сигнал с результатом создания пользователя
}
//-----------------------------------------------------------------------------
/**
 * @brief TComandExecutor::userAuthorization - Метод обработает результат авторизации
 * @param inDataStream - Входящий поток
 */
void TComandExecutor::userAuthorization(QDataStream &inDataStream)
{
    qint32 Result = Res::rUnknown;
    TUserInfo UserInfo;

    inDataStream >> Result; // Получаем результат выполнения
    if (Result == Res::CanAut::caAuthorizationTrue) // Если авторизация прошла успешно
    {
        inDataStream >> UserInfo; // Получаем информацию о пользователе
        sig_SetUserInfo(UserInfo); // Шлём сигнал с данными пользователя
    }

    sig_AuthorizationResult(Result); // Шлём сигнал с результатом авторизации
}
//-----------------------------------------------------------------------------
/**
 * @brief TComandExecutor::findUsersResult - Метод обработает результат поика пользователей
 * @param inDataStream - Входящий поток
 */
void TComandExecutor::findUsersResult(QDataStream &inDataStream)
{
    qint32 Result = Res::rUnknown;
    QList<TUserInfo> FoundUsers;

    inDataStream >> Result; // Получаем результат поиска

    if (Result == Res::FindUsers::fuUsersFound) // Если пользователи найдены
        inDataStream >> FoundUsers; // Получаем информацию о пользователях

    sig_FindUsersResult(FoundUsers); // Шлём сигнал с результаттом поиска пользователей
}
//-----------------------------------------------------------------------------
/**
 * @brief TComandExecutor::addContact - Метод обработает результат добавления контакта
 * @param inDataStream - Входящий поток
 */
void TComandExecutor::addContactResult(QDataStream &inDataStream)
{
    qint32 Result = Res::rUnknown;
    inDataStream >> Result;
    sig_AddContactResult(Result);
}
//-----------------------------------------------------------------------------
/**
 * @brief TComandExecutor::getContactsResult - Метод обработает результат запроса списка контактов
 * @param inDataStream - Входящий поток
 */
void TComandExecutor::getContactsResult(QDataStream &inDataStream)
{
    qint32 Result = Res::rUnknown;
    QList<TUserInfo> FoundContacts;

    inDataStream >> Result; // Получаем результат поиска

    if (Result == Res::GetContacts::gcUsersFound) // Если контакты найдены
        inDataStream >> FoundContacts; // Получаем информацию о контактах

    sig_GetContactsResult(FoundContacts);
}
//-----------------------------------------------------------------------------
/**
 * @brief TComandExecutor::ReadStringFromStream - Метод считает строку из потока
 * @param inDataStream - Входящий поток
 * @return Воернёт считанную строку
 */
QString TComandExecutor::ReadStringFromStream(QDataStream &inDataStream)
{
    QByteArray ByteBuf;
    inDataStream >> ByteBuf;

    return QString::fromUtf8(ByteBuf); // Помещаем буфер в результат (с раскодировкой из UTF8)
}
//-----------------------------------------------------------------------------

#include "MSGClient.h"

#include <QCryptographicHash>
#include <QHostAddress>

#include "comandes.h"
#include "resultcodes.h"
#include "Classes/UserInfo/UserInfo.h"

//-----------------------------------------------------------------------------
/**
 * @brief TMSGClient::executCommand - Метод исполняющий команды
 * @param inClientSender - Сокет - получаетль
 */
void TMSGClient::executCommand(QTcpSocket* inClientSender)
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
//        case Commands::GetContacts: // Запрос списка контактов
//        {
//            sig_LogMessage("Получен ответ на запрос списка контактов" + inClientSender->peerAddress().toString());
//            getContactsResult(inStream); // Обработать результат запроса списка контактов
//            break;
//        }
        case Commands::DeleteContact:
        {
            sig_LogMessage("Получен ответ на удаление контакта" + inClientSender->peerAddress().toString());
            deleteContactResult(inStream); // Обработать результат удаления контака
            break;
        }

        default: sig_LogMessage("Получена неизвестная команда от " + inClientSender->peerAddress().toString());
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGClient::creteUserResult - Мтод обработает результат регистрации
 * @param inDataStream - Входящий поток
 */
void TMSGClient::creteUserResult(QDataStream &inDataStream)
{
    qint32 Result = Res::rUnknown;
    inDataStream >> Result; // Получаем результат выполнения
    sig_UserCreateResult(Result); // Шлём сигнал с результатом создания пользователя
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGClient::userAuthorization - Метод обработает результат авторизации
 * @param inDataStream - Входящий поток
 */
void TMSGClient::userAuthorization(QDataStream &inDataStream)
{
    qint32 Result = Res::rUnknown;
    TUserInfo UserInfo;
    QList<TUserInfo> Contacts;

    inDataStream >> Result; // Получаем результат выполнения
    if (Result == Res::CanAut::caAuthorizationTrue) // Если авторизация прошла успешно
    {
        inDataStream >> UserInfo; // Получаем информацию о пользователе
        inDataStream >> Contacts; // Получаем список контактов
        sig_SetUserInfo(UserInfo); // Шлём сигнал с данными пользователя
        sig_SetContacts(Contacts); // Шлём сигнал со списком контактов пользователей
    }

    sig_AuthorizationResult(Result); // Шлём сигнал с результатом авторизации
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGClient::findUsersResult - Метод обработает результат поика пользователей
 * @param inDataStream - Входящий поток
 */
void TMSGClient::findUsersResult(QDataStream &inDataStream)
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
 * @brief TMSGClient::addContact - Метод обработает результат добавления контакта
 * @param inDataStream - Входящий поток
 */
void TMSGClient::addContactResult(QDataStream &inDataStream)
{
    qint32 Result = Res::rUnknown;
    TUserInfo ContactInfo;
    inDataStream >> Result;

    if (Result = Res::AddContact::acCreated)
        inDataStream >> ContactInfo;

    sig_AddContactResult(Result, ContactInfo);
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGClient::getContactsResult - Метод обработает результат запроса списка контактов
 * @param inDataStream - Входящий поток
 */
//void TMSGClient::getContactsResult(QDataStream &inDataStream)
//{
//    qint32 Result = Res::rUnknown;
//    QList<TUserInfo> FoundContacts;

//    inDataStream >> Result; // Получаем результат поиска

//    if (Result == Res::GetContacts::gcUsersFound) // Если контакты найдены
//        inDataStream >> FoundContacts; // Получаем информацию о контактах

//    sig_GetContactsResult(FoundContacts);
//}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGClient::deleteContactResult - Метод обработает результат удаления контакта
 * @param inDataStream - Входящий поток
 */
void TMSGClient::deleteContactResult(QDataStream &inDataStream)
{
    qint32 Result = Res::rUnknown;
    QUuid ContactUuid;
    inDataStream >> Result;

    if (Result = Res::DeleteContact::dcContactRemove)
        inDataStream >> ContactUuid;

    sig_DeleteContactResult(Result, ContactUuid);
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGClient::ReadStringFromStream - Метод считает строку из потока
 * @param inDataStream - Входящий поток
 * @return Воернёт считанную строку
 */
QString TMSGClient::ReadStringFromStream(QDataStream &inDataStream)
{
    QByteArray ByteBuf;
    inDataStream >> ByteBuf;

    return QString::fromUtf8(ByteBuf); // Помещаем буфер в результат (с раскодировкой из UTF8)
}
//-----------------------------------------------------------------------------

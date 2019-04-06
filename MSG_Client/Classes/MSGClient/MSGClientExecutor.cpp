#include "MSGClient.h"

#include <QCryptographicHash>
#include <QHostAddress>

#include "comandes.h"
#include "resultcodes.h"

#include "Classes/DataModule/DataModule.h"
#include "Classes/UserInfo/UserInfo.h"
#include "Classes/ChatMessage/ChatMessage.h"

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
            sig_LogMessage("Получен ответ на создание пользователя");
            creteUserResult(inStream); // Обработатьм результат регистрации пользователя
            break;
        }
        case Commands::Authorization: // Авторизация пользователя
        {
            sig_LogMessage("Получен ответ на авторизацию");
            userAuthorization(inStream); // Обработать результат авторизации
            break;
        }
        case Commands::GetUserTypes: // Возврат списка типов пользователей
        {
            sig_LogMessage("Получен ответ на запрос списка типов пользователей");
            getUserTypesResult(inStream); // Обрабатываем результат возврата списка пользователей
            break;
        }
        case Commands::FindUsers: // Поиск пользователей
        {
            sig_LogMessage("Получен ответ на поиск пользователей");
            findUsersResult(inStream); // Обработать результат поиска пользователей
            break;
        }
        case Commands::AddContact: // Добавление котнакта
        {
            sig_LogMessage("Получен ответ на добавление пользователя");
            addContactResult(inStream); // Обработать результат добавления контакта
            break;
        }
        case Commands::DeleteContact: // Удаление контакта
        {
            sig_LogMessage("Получен ответ на удаление контакта");
            deleteContactResult(inStream); // Обработать результат удаления контака
            break;
        }
        case Commands::ContactChangeStatus: // Контакт изменил свой статус
        {
            sig_LogMessage("Получено сообщение о смене статуса");
            contactChangeStatus(inStream);  // Обрабатываем изменение статуса контакта
            break;
        }
        case Commands::CreateChat: // Создание беседы
        {
            sig_LogMessage("Получен ответ на создание беседы");
            createChatResult(inStream); // Обрабатываем результат создания беседы
            break;
        }
        case Commands::InviteToChat: // Приглашение в беседу
        {
            sig_LogMessage("Получено приглашение в беседу");
            inviteToChatResult(inStream);
            break;
        }
        case Commands::DeleteUserFromChat: // Удаление пользователя из беседы
        {
            sig_LogMessage("Пролучeн запрос на удаление пользователя из беседы");
            deleteUserFromChatResult(inStream);
            break;
        }
        case Commands::ILeaveFromChat: // Выход из беседы
        {
            sig_LogMessage("Пролучeн ответ на выход из беседы");
            leaveFromChatResult(inStream);
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
    quint8 Result = Res::rUnknown;
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
    quint8 Result = Res::rUnknown;

    inDataStream >> Result; // Получаем результат выполнения
    if (Result == Res::CanAut::caAuthorizationTrue) // Если авторизация прошла успешно
    {
        Core::TUserAccount NewAccount;
        inDataStream >> NewAccount; // Получаем информацию о пользователе
        NewAccount.Connection.setSocket(fClient.get()); // Задаём сокет соединения

        //TDM::Instance().UserAccount().reset(new Users::TUserAccount(NewAccount));
        TDM::Instance().slot_SetUserAccount(NewAccount);
    }

    sig_AuthorizationResult(Result); // Шлём сигнал с результатом авторизации
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGClient::getUserTypesResult - Метод обработает результат запроса списка типов пользователей
 * @param inDataStream - Входящий поток
 */
void TMSGClient::getUserTypesResult(QDataStream &inDataStream)
{
    quint8 Result = Res::rUnknown;
    QList<OtherTypes::TUserType> UserTypes;

    inDataStream >> Result; // Получаем результат выполнения
    if (Result == Res::GetUserTypes::gtOK) // Если авторизация прошла успешно
        inDataStream >> UserTypes;

    sig_GetUserTypesResult(UserTypes);
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGClient::findUsersResult - Метод обработает результат поика пользователей
 * @param inDataStream - Входящий поток
 */
void TMSGClient::findUsersResult(QDataStream &inDataStream)
{
    quint8 Result = Res::rUnknown;
    QList<Core::TUserInfo> FoundUsers;

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
    quint8 Result = Res::rUnknown;
    Core::TUserInfo ContactInfo;
    inDataStream >> Result;

    if (Result == Res::AddContact::acCreated)
        inDataStream >> ContactInfo;

    switch (Result)
    {
        case Res::AddContact::acCreated:
        {
            TDM::Instance().UserAccount()->contacts()->insert(std::make_pair(ContactInfo.userUuid(), std::make_shared<Core::TUserInfo>(ContactInfo))); // Добавляем контакт в список
            sig_LogMessage(tr("Успешно добавлен контакт: ") + ContactInfo.userName());
            break;
        };
        case Res::AddContact::acAlredyExist:
        { sig_LogMessage(tr("Контакт уже существует!")); break; };
        default:
        { sig_LogMessage(tr("Произошла непредвиденная ошибка при добавлении контакта!")); break; };
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGClient::deleteContactResult - Метод обработает результат удаления контакта
 * @param inDataStream - Входящий поток
 */
void TMSGClient::deleteContactResult(QDataStream &inDataStream)
{
    quint8 Result = Res::rUnknown;
    QUuid ContactUuid;
    inDataStream >> Result;

    if (Result == Res::DeleteContact::dcContactRemove)
        inDataStream >> ContactUuid;

    switch (Result)
    {
        case Res::DeleteContact::dcContactRemove:
        {
            TDM::Instance().UserAccount()->contacts()->erase(ContactUuid);
            sig_LogMessage(tr("Контакт успешно удалён"));
            break;
        };
        case Res::DeleteContact::dcContactNotFound:
        { sig_LogMessage(tr("Не удалось найти пользователя для удаления!")); break; }
        default: { sig_LogMessage(tr("Произошла непредвиденная ошибка при удалении контакта!")); break; };
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGClient::createChatResult - Метод обработает результат создания беседы
 * @param inDataStream - Входящий поток
 */
void TMSGClient::createChatResult(QDataStream &inDataStream)
{
    quint8 Result = Res::rUnknown;
    inDataStream >> Result;

    switch (Result)
    {
        case Res::CreateChat::ccCreated:
        { sig_LogMessage(tr("Новая беседа успешно создана")); break; }
        case Res::CreateChat::ccAlredyExist:
        { sig_LogMessage(tr("Беседа уже существует")); break; }
        default:
        { sig_LogMessage(tr("При создании беседы произошла ошибка!")); break; }
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGClient::InviteToChatResult - Метод обработает добавление в беседу
 * @param inDataStream - Входящий поток
 */
void TMSGClient::inviteToChatResult(QDataStream &inDataStream)
{
    Core::TChatInfo AddedChat;
    inDataStream >> AddedChat;

    sig_InviteToChatResult(std::make_shared<Core::TChatInfo>(AddedChat));
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGClient::deleteUserFromChat - Метод обработает удаление из беседы
 * @param inDataStream - Входящий поток
 */
void TMSGClient::deleteUserFromChatResult(QDataStream &inDataStream)
{
    quint8 Result = Res::rUnknown;
    QUuid ChatUuid;
    QUuid UserUuid;
    inDataStream >> Result >> ChatUuid >> UserUuid;

    switch (Result)
    {
        case Res::DeleteUserFromChat::dufcSuccess: // Успешное удаление
        {
            TDM &DM = TDM::Instance();
            QString LogMessage = "";
            auto FindChatRes = DM.UserAccount()->chats()->find(ChatUuid); // Ищим беседу

            if (FindChatRes == DM.UserAccount()->chats()->end())
                LogMessage = tr("Ошибка, получен запрос на удаление пользователя из несуществующей беседы");
            else
            {
                if (UserUuid == DM.UserAccount()->userInfo()->userUuid()) // Если удалён текущий пользователь
                {   // То требуется удалить саму беседу
                    LogMessage = tr("Вы покидаете беседу: ") + FindChatRes->second->chatName();
                    DM.UserAccount()->chats()->erase(FindChatRes); // Удаляем беседу (сгенерируется сигнал об удалении беседы)
                }
                else // если удаляется не текущий пользователь
                {
                    auto FindUserRes = FindChatRes->second->clients()->find(UserUuid); // Ищим удаляемого пользователя

                    if (FindUserRes == FindChatRes->second->clients()->end()) // Если пользоваетль не найден
                        LogMessage = tr("Ошибка, получен запрос на удаление не существующего пользователя из беседы");
                    else // Если пользователь найден
                    {
                        LogMessage = tr("Пользователь %1 покидает беседу %2").arg(FindUserRes->second->userName()).arg(FindChatRes->second->chatName());
                        FindChatRes->second->clients()->erase(FindUserRes); // Удаляем пользователя из беседы
                    }
                }
            }

            sig_LogMessage(LogMessage);
            break;
        }
        case Res::DeleteUserFromChat::dufcNotInside: // Не удалось найти
        { sig_LogMessage(tr("Не удалось найти пользователя для удаления")); break; }
        default:
        { sig_LogMessage(tr("При удалении пользователя произошла ошибка!")); break; }
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGClient::leaveFromChat - Метод обработает выход из беседы
 * @param inDataStream - Входящий поток
 */
void TMSGClient::leaveFromChatResult(QDataStream &inDataStream)
{
    quint8 Result = Res::rUnknown;
    inDataStream >> Result;

    switch (Result)
    {
        case Res::ILeaveFromChat::lfcSuccess: // Выход успешен
        { sig_LogMessage(tr("Выход из беседы прошёл успешно")); break; }
        case Res::ILeaveFromChat::lfcFail: // Не удалось
        { sig_LogMessage(tr("Не удалось выйти из беседы")); break; }
        default:
        { sig_LogMessage(tr("При выходе из беседы произошла ошибка!")); break; }
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGClient::contactChangeStatus - Метод обработает сообщение о смене статуса контакта
 * @param inDataStream - Входящий поток
 */
void TMSGClient::contactChangeStatus(QDataStream &inDataStream)
{
    QUuid ContactUuid;
    quint8 ContactStatus;

    inDataStream >> ContactUuid >> ContactStatus;
    sig_ContactChangeStatus(ContactUuid, ContactStatus);
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

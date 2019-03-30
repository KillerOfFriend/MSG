#include "MSGServer.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>

#include "comandes.h"
#include "resultcodes.h"

#include "Classes/DB/DB.h"
#include "Classes/DataModule/DataModule.h"
#include "Classes/UserAccount/UserAccount.h"

//-----------------------------------------------------------------------------
void TMSGServer::executCommand(QTcpSocket* inClientSender)
{
    if (!inClientSender)
        return;

    QDataStream inStream(inClientSender); // Оборачиваем его в поток данных

    QByteArray ReturningData; // Возвращаемый результат
    QDataStream outStream(&ReturningData, QIODevice::WriteOnly);

    quint8 Command; // Команда от клиента
    inStream >> Command; // Получаем команду

    switch (Command) // Проверяем команду
    {
        case Commands::CreateUser: // Регистрация пользователя
        {
            sig_LogMessage(inClientSender->peerAddress(), "Получен запрос на создание пользователя");

            quint8 Resuslt = creteUser(inStream); // Регистрация пользователя
            outStream << Command << Resuslt; // Пишем в результат команду и результат обработки
            sig_LogMessage(inClientSender->peerAddress(), "Отправка ответа о создании пользователя");

            break;
        }
        //---
        case Commands::Authorization: // Авторизация пользователя
        {
            sig_LogMessage(inClientSender->peerAddress(), "Получен запрос на авторизацию");

            std::pair<quint8, QUuid> Resuslt = canAuthorization(inStream); // Обрабатываем возможность авторизации

            switch (Resuslt.first) // Проверяем результат обработки
            {
                case Res::CanAut::caAuthorizationFalse: // Пользователь не найден (авторизация не возможна)
                {
                    outStream << Command << Resuslt.first; // Пишем в результат команду и результат обработки
                    sig_LogMessage(inClientSender->peerAddress(), "Отказ в авторизации, пользователь не найден");
                    break;
                }
                case Res::CanAut::caIncorrectPass: // Пользователь найден, но пароль не верный (авторизация не возможна)
                {
                    outStream << Command << Resuslt.first; // Пишем в результат команду и результат обработки
                    sig_LogMessage(inClientSender->peerAddress(), "Отказ в авторизации, не верный пароль");
                    break;
                }
                case Res::CanAut::caAuthorizationTrue: // Пользователь найден (авторизация возможна)
                {
                    if (socketByUuid(Resuslt.second)) // Если пользователь с таким Uuid уже авторизирован (Есть сокет)
                    {
                        outStream << Command << Res::CanAut::caUserAlredyOnline; // Пишем в результат команду и флаг попытки повторной авторизации
                        sig_LogMessage(inClientSender->peerAddress(), "Повторная авторизация запрещена");
                    }
                    else // Авторизация уникальна (Сокет с таким Uuid не найден)
                    {
                        Core::UserInfo_Ptr UserInfo = fServerCache->getUserInfo(Resuslt.second); // Получаем информацию о пользователе

                        if (!UserInfo || UserInfo->userUuid() != Resuslt.second) // Если не удалось получить данные о пользователе (Или они не орректны)
                        {
                            outStream << Command << Res::CanAut::caUserInfoError; // Пишем в результат команду и результат обработки
                            sig_LogMessage(inClientSender->peerAddress(), "Ошибка чтения данных записи");
                        }
                        else // Данные о пользователе успешно получены
                        {
                            QList<Core::UserInfo_Ptr> Contacts = getContacts(UserInfo->userUuid()); // Получаем список контактов пользователя
                            QList<Core::ChatInfo_Ptr> Chats = getChats(UserInfo->userUuid()); // Получаем список бесед пользователя

                            checkUsersStatus(Contacts); // Проверяем контакты онлайн и устанавливаем им статус

                            Core::TUserAccount UserAccount(this);
                            UserAccount.slot_SetUserInfo(UserInfo);
                            UserAccount.slot_SetContacts(Contacts);
                            UserAccount.slot_SetChats(Chats);

                            slot_SetAuthorizedClient(inClientSender, UserAccount); // Авторизируем пользователя
                            outStream << Command << Resuslt.first << UserAccount; // Пишем в результат команду и результат обработки
                            sig_LogMessage(inClientSender->peerAddress(), "Авторизация разрешена");
                        }
                    }
                    break;
                }
                default: // Вернулось нипойми что
                {
                    outStream << Command << Res::rUnknown; // Пишем в результат команду и результат обработки
                    sig_LogMessage(inClientSender->peerAddress(), "Неизвестная ошибка");
                    break;
                }
            }
            break;
        }
        //---
        case Commands::GetUserTypes: // Запрос списка типов пользователей
        {
            sig_LogMessage(inClientSender->peerAddress(), "Получен запрос списка типов пользователей");

            if (!fClients.userTypes() || fClients.userTypes()->size() == 0) // Если не инициализирово нили пусто
            {
                outStream << Command << Res::GetUserTypes::gtFail;
                sig_LogMessage(inClientSender->peerAddress(), "Не удалось передать типы пользователей");
            }
            else
            {
                outStream << Command << Res::GetUserTypes::gtOK << fClients.userTypes()->toList();
                sig_LogMessage(inClientSender->peerAddress(), "Отправлен список типов пользователей");
            }

            break;
        }
        //---
        case Commands::FindUsers: // Поиск пользователей
        {
            sig_LogMessage(inClientSender->peerAddress(), "Получен запрос на поиск пользователя");
            QList<Core::UserInfo_Ptr> Resuslt = findUsers(inStream); // Поиск пользователя

            if (Resuslt.isEmpty()) // Если список пуст
            {
                outStream << Command << Res::FindUsers::fuUsersNotFound; // Возвращаем результат (Пользователи не найдены)
                sig_LogMessage(inClientSender->peerAddress(), "Пользователи не найдены");
            }
            else
            {
                checkUsersStatus(Resuslt); // Проверяем контакты онлайн и устанавливаем им статус
                QList<Core::TUserInfo> UsersBuf;
                std::for_each(Resuslt.begin(), Resuslt.end(), [&UsersBuf](const Core::UserInfo_Ptr &UserInfo)
                { // Приобразуем указетели к объектам
                    UsersBuf.push_back(*UserInfo);
                });
                outStream << Command << Res::FindUsers::fuUsersFound << UsersBuf; // Пишем в результат команду и результат обработки
                sig_LogMessage(inClientSender->peerAddress(), "Отправка списка пользователей");
            }

            break;
        }
        //---
        case Commands::AddContact: // Добавление контакта
        {
            sig_LogMessage(inClientSender->peerAddress(), "Получен запрос на добавление контакта");
            std::pair<quint8, Core::UserInfo_Ptr> Result = addContact(inStream); // Добавляем контакт

            if (Result.first != Res::AddContact::acCreated) // Если пользователь не был добавлен
                outStream << Command << Result.first; // Пишем в результат команду и результат обработки
            else // Если добавление прошло успешно
            {
                slot_AddContact(inClientSender, Result.second); // Добавляем контакт клиенту
                checkUserStatus(Result.second); // Проверяем его статус

                auto SenderIt = fClients.find(inClientSender); // Получаем данные о пославшем команду клиенте
                if (SenderIt != fClients.end()) // Если пославший найден
                    syncAddedUser(Result.second->userUuid(), SenderIt->second.userInfo()); // Синхронизируем добовление контакта (с этим самым контактом)

                outStream << Command << Result.first << *Result.second; // Пишем в результат команду, результат обработки и информацию о контакте
            }

            sig_LogMessage(inClientSender->peerAddress(), "Отправка результата добавления контакта");
            break;
        }
        //---
        case Commands::DeleteContact: // Удаление контакта
        {
            sig_LogMessage(inClientSender->peerAddress(), "Получен запрос на удаление контакта");
            std::pair<quint8, QUuid> Result = deleteContact(inStream); // Удаляем контакт

            if (Result.first != Res::DeleteContact::dcContactRemove) // Если контакт не удалён
                outStream << Command << Result.first; // Пишем в результат команду и результат обработки
            else
            {
                slot_DelContact(inClientSender, Result.second);

                auto SenderIt = fClients.find(inClientSender); // Получаем данные о пославшем команду клиенте
                if (SenderIt != fClients.end()) // Если пославший найден
                     syncDeletedUser(Result.second, SenderIt->second.userInfo()->userUuid()); // Синхронизируем удаление контакта (с этим самым контактом)

                outStream << Command << Result.first << Result.second; // Пишем в результат команду, результат обработки и Uuid удалённого контакта
            }

            sig_LogMessage(inClientSender->peerAddress(), "Отправка результата удаления контакта");
            break;
        }
        //---
        case Commands::CreateChat: // Создание беседы
        {
            sig_LogMessage(inClientSender->peerAddress(), "Получен запрос на создание беседы");
            quint8 Result = createChat(inStream); // Пытаемся добавить беседу

            outStream << Command << Result; // Шлём команду и результат обработки

            sig_LogMessage(inClientSender->peerAddress(), "Отправка результата создания беседы");
            break;
        }
        //---
        case Commands::DeleteUserFromChat: // Выход пользователя из беседы
        {
            sig_LogMessage(inClientSender->peerAddress(), "Получен запрос на удаление пользователя");
            std::pair<quint8, std::pair<QUuid, QUuid>> Result; // Результат состоит из резульатат обработки и прары Uuid чата Uuid пользоватля
            Result = deleteUserFromChat(inStream); // Пытаемся удалить пользователя

            if (Result.first == Res::DeleteUserFromChat::dufcSuccess) // Если пользователь успешно удалён из беседы (В БД)
                slot_DeleteUserFromChat(Result.second.first, Result.second.second); // Удаляем пользователя из бесседы в кеше

            outStream << Command << Result.first; // Шлём команду и результат обработки

            sig_LogMessage(inClientSender->peerAddress(), "Отправка результата удаления пользователя из чата");
            break;
        }
        case Commands::ILeaveFromChat: // Выход пользователя из беседы
        {
            sig_LogMessage(inClientSender->peerAddress(), "Получен запрос на выход пользователя из беседы");
            std::pair<quint8, std::pair<QUuid, QUuid>> Result; // Результат состоит из резульатат обработки и прары Uuid чата Uuid пользоватля
            Result = deleteUserFromChat(inStream); // Пытаемся удалить пользователя (приславшего запрос)

            if (Result.first == Res::DeleteUserFromChat::dufcSuccess) // Если пользователь успешно удалён из беседы (В БД)
                slot_DeleteUserFromChat(Result.second.first, Result.second.second); // Удаляем пользователя из бесседы в кеше

            outStream << Command << Result.first; // Шлём команду и результат обработки

            sig_LogMessage(inClientSender->peerAddress(), "Отправка результата выхода из чата");
            break;
        }

        default: sig_LogMessage(inClientSender->peerAddress(), "Получена неизвестная команда");
    }

    inClientSender->write(ReturningData); // Возвращаем результат
    //inClientSender->waitForBytesWritten();
}
//-----------------------------------------------------------------------------
quint8 TMSGServer::creteUser(QDataStream &inDataStream)
{
    quint8 Result = Res::rUnknown;

    QString Login = ReadStringFromStream(inDataStream); // Читаем логин

    QByteArray PasswordHash;
    inDataStream >> PasswordHash; // Читаем хеш пароля

    QString Name = ReadStringFromStream(inDataStream); // Читаем имя

    bool IsMale;
    inDataStream >> IsMale; // Читаем половую пренадлежность

    QSqlQuery Query(TDB::Instance().DB());

    if(!Query.prepare("SELECT * FROM create_user(:in_login, :in_password, :in_name, :in_is_male)"))
        qDebug() << "[ОШИБКА]: " + Query.lastError().text();
    else
    {
        Query.bindValue(":in_login", Login.toUtf8());
        Query.bindValue(":in_password", QString(PasswordHash).toUtf8());
        Query.bindValue(":in_name", Name.toUtf8());
        Query.bindValue(":in_is_male", IsMale);

        if (!Query.exec())
            qDebug() << "[ОШИБКА]: " + Query.lastError().text();
        else
        {
            while (Query.next()) // Вернётся только 1 запись
                Result = Query.value("create_user").toUInt();
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
std::pair<quint8, QUuid> TMSGServer::canAuthorization(QDataStream &inDataStream) // Метод авторизирует пользователя
{
    std::pair<quint8, QUuid> Result;

    QString Login = ReadStringFromStream(inDataStream);

    QByteArray PasswordHash;
    inDataStream >> PasswordHash;

    QSqlQuery Query(TDB::Instance().DB());

    if(!Query.prepare("SELECT * FROM can_user_authorization(:in_login, :in_password)"))
        qDebug() << "[ОШИБКА]: " + Query.lastError().text();
    else
    {
        Query.bindValue(":in_login", Login.toUtf8());
        Query.bindValue(":in_password", QString(PasswordHash).toUtf8());

        if (!Query.exec())
            qDebug() << "[ОШИБКА]: " + Query.lastError().text();
        else
        {
            while (Query.next()) // Вернётся только 1 запись
            {
                Result.first = Query.value("result_code").toUInt();
                Result.second = Query.value("uuid_found_user").toByteArray();
            }
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
QList<Core::UserInfo_Ptr> TMSGServer::findUsers(QDataStream &inDataStream) // Метод вернёт список пользователей по их имени\логину
{
    QList<Core::UserInfo_Ptr> Result;
    QString UserNameLogin = '%' + ReadStringFromStream(inDataStream) + '%'; // Получаем фильтр поиска

    QSqlQuery Query(TDB::Instance().DB());

    if(!Query.prepare("SELECT * FROM find_users(:in_user)"))
        qDebug() << "[ОШИБКА]: " + Query.lastError().text();
    else
    {
        Query.bindValue(":in_user", UserNameLogin.toUtf8());

        if (!Query.exec())
            qDebug() << "[ОШИБКА]: " + Query.lastError().text();
        else
        {
            QUuid FindRes; // Uuid найденного пользователя
            while (Query.next()) // Читаем все записи
            {
                FindRes = Query.value("found_user_uuid").toUuid();

                if (!FindRes.isNull())
                    Result.push_back(fServerCache->getUserInfo(FindRes));
            }
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
std::pair<quint8, Core::UserInfo_Ptr> TMSGServer::addContact(QDataStream &inDataStream) // Метод добавит котнтакт пользователю
{
    std::pair<quint8, Core::UserInfo_Ptr> Result;
    Result.first = Res::rUnknown;

    QUuid Owner; // Владелец нового контакта
    QUuid NewContact; // Сам новый контакт

    inDataStream >> Owner >> NewContact; // Читаем из потока Uuid'ы контактов

    QSqlQuery Query(TDB::Instance().DB());

    if(!Query.prepare("SELECT * FROM create_contact(:in_owner,:in_contact)"))
        qDebug() << "[ОШИБКА]: " + Query.lastError().text();
    else
    {
        Query.bindValue(":in_owner", Owner);
        Query.bindValue(":in_contact", NewContact);

        if (!Query.exec())
            qDebug() << "[ОШИБКА]: " + Query.lastError().text();
        else
        {
            while (Query.next()) // Вернётся только 1 запись
                Result.first = Query.value("create_contact").toUInt();

            if (Result.first == Res::AddContact::acCreated) // Если контакт был добавлен
                Result.second = fServerCache->getUserInfo(NewContact); // Получаем информацию об это контакте
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
QList<Core::UserInfo_Ptr> TMSGServer::getContacts(const QUuid &inOwnerUuid) // Метод вернёт список контактов по uuid указанного пользователя
{
    QList<Core::UserInfo_Ptr> Result;
    QSqlQuery Query(TDB::Instance().DB());

    if(!Query.prepare("SELECT * FROM get_contacts(:in_owner)"))
        qDebug() << "[ОШИБКА]: " + Query.lastError().text();
    else
    {
        Query.bindValue(":in_owner", inOwnerUuid);

        if (!Query.exec())
            qDebug() << "[ОШИБКА]: " + Query.lastError().text();
        else
        {
            QUuid FindRes; // Uuid найденного пользователя
            while (Query.next()) // Читаем все записи
            {
                FindRes = Query.value("found_friend_uuid").toUuid();

                if (!FindRes.isNull())
                    Result.push_back(fServerCache->getUserInfo(FindRes));
            }
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
QList<Core::UserInfo_Ptr> TMSGServer::getContacts(QDataStream &inDataStream) // Метод вернёт список контактов указанного пользователя
{
    QUuid OwnerUuid;

    inDataStream >> OwnerUuid; // Получаем владельца контактов
    return getContacts(OwnerUuid);
}
//-----------------------------------------------------------------------------
std::pair<quint8, QUuid> TMSGServer::deleteContact(QDataStream &inDataStream) // Метод удалит котнтакт пользователю
{
    std::pair<quint8, QUuid> Result;
    Result.first = Res::rUnknown;

    QUuid Owner; // Владелец контакта
    QUuid Contact; // Сам контакт

    inDataStream >> Owner >> Contact; // Читаем из потока Uuid'ы контактов

    QSqlQuery Query(TDB::Instance().DB());

    if(!Query.prepare("SELECT * FROM delete_contacts(:in_owner,:in_contact)"))
        qDebug() << "[ОШИБКА]: " + Query.lastError().text();
    else
    {
        Query.bindValue(":in_owner", Owner);
        Query.bindValue(":in_contact", Contact);

        if (!Query.exec())
            qDebug() << "[ОШИБКА]: " + Query.lastError().text();
        else
        {
            while (Query.next()) // Вернётся только 1 запись
                Result.first = Query.value("delete_contacts").toUInt();

            if (Result.first == Res::DeleteContact::dcContactRemove) // Если контакт был удалён
                Result.second = Contact; // Запоминаем его Uuid
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
quint8 TMSGServer::createChat(QDataStream &inDataStream) // Метод добавит новую беседу
{
    quint8 Result = Res::rUnknown;

    Core::TChatInfo NewChat; // Читаем данные о беседе из потока
    inDataStream >> NewChat;

    QSqlQuery Query(TDB::Instance().DB());

    if(!Query.prepare("SELECT * FROM create_chat(:in_uuid,:in_name,:in_private_status)"))
        qDebug() << "[ОШИБКА]: " + Query.lastError().text();
    else
    {
        Query.bindValue(":in_uuid", NewChat.chatUuid());
        Query.bindValue(":in_name", NewChat.chatName().toUtf8());
        Query.bindValue(":in_private_status", NewChat.chatPrivateStatus());

        if (!Query.exec())
            qDebug() << "[ОШИБКА]: " + Query.lastError().text();
        else
        {
            while (Query.next()) // Вернётся только 1 запись
                Result = Query.value("create_chat").toUInt();
        }
    }

    if (Result != Res::rUnknown) // Если добавление или обновление
    {
        std::for_each(NewChat.clients()->begin(), NewChat.clients()->end(), [&](const std::pair<QUuid, Core::UserInfo_Ptr> &ChatClient)
        {
            addUserToChat(NewChat.chatUuid(), ChatClient.second->userUuid()); // Добавляем пользователя в беседу
        });
        // Тут добавление завершено беседы и пользователей в неё завершено
        syncCreateChat(NewChat.chatUuid()); // Синхронизируем создание беседы с пользователями
    }

    return Result;
}
//-----------------------------------------------------------------------------
QList<QUuid> TMSGServer::findChats(QUuid inUserUuid) // Метод вернёт список бесед по uuid указанного пользователя
{
    QList<QUuid> Result;

    QSqlQuery Query(TDB::Instance().DB());

    if(!Query.prepare("SELECT * FROM get_chats(:in_owner_uuid)"))
        qDebug() << "[ОШИБКА]: " + Query.lastError().text();
    else
    {
        Query.bindValue(":in_owner_uuid", inUserUuid);

        if (!Query.exec())
            qDebug() << "[ОШИБКА]: " + Query.lastError().text();
        else
        {
            while (Query.next())
                Result.push_back(Query.value("found_chats_uuid").toUuid());
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
quint8 TMSGServer::addUserToChat(QUuid inChatUuid, QUuid inUserUuid) // Метод добавит пользователя в беседу
{
    quint8 Result = Res::rUnknown;

    QSqlQuery Query(TDB::Instance().DB());

    if(!Query.prepare("SELECT * FROM add_user_to_chat(:in_chat_uuid,:in_user_uuid)"))
        qDebug() << "[ОШИБКА]: " + Query.lastError().text();
    else
    {
        Query.bindValue(":in_chat_uuid", inChatUuid);
        Query.bindValue(":in_user_uuid", inUserUuid);

        if (!Query.exec())
            qDebug() << "[ОШИБКА]: " + Query.lastError().text();
        else
        {
            while (Query.next()) // Вернётся только 1 запись
                Result = Query.value("add_user_to_chat").toUInt();
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
QList<Core::ChatInfo_Ptr> TMSGServer::getChats(const QUuid &inOwnerUuid) // Метод вернёт список бесед указонного пользователя
{
    QList<Core::ChatInfo_Ptr> Result;

    QList<QUuid> Chats = findChats(inOwnerUuid); // Получаем список Uuid'ов бесед пользователя

    std::for_each(Chats.begin(), Chats.end(), [&](const QUuid &ChatUuid)
    {
        if (!ChatUuid.isNull()) // Если Uuid беседы валиден
        {
            Core::ChatInfo_Ptr Chat = fServerCache->getChatInfo(ChatUuid); // Получаем данные о беседе
            if (Chat) // Если полученная беседа валидна
                Result.push_back(Chat); // Добавляем беседу в список
        }
    });

    return Result;
}
//-----------------------------------------------------------------------------
std::pair<quint8, std::pair<QUuid, QUuid>> TMSGServer::deleteUserFromChat(QDataStream &inDataStream) // Метод удалит пользователя из беседы
{
    std::pair<quint8, std::pair<QUuid, QUuid>> Result; // Результат состоит из резульатат обработки и прары Uuid чата Uuid пользоватля
    Result.first = Res::rUnknown;

    QUuid ChatUuid; // Uuid беседы, из которой удаляем пользователя
    QUuid UserUuid; // Uuid удаляемого пользователя

    inDataStream >> ChatUuid >> UserUuid;

    Result.second.first = ChatUuid;
    Result.second.second = UserUuid;

    QSqlQuery Query(TDB::Instance().DB());

    if(!Query.prepare("SELECT * FROM delete_user_from_chat(:in_chat_uuid,:in_user_uuid)"))
        qDebug() << "[ОШИБКА]: " + Query.lastError().text();
    else
    {
        Query.bindValue(":in_chat_uuid", ChatUuid);
        Query.bindValue(":in_user_uuid", UserUuid);

        if (!Query.exec())
            qDebug() << "[ОШИБКА]: " + Query.lastError().text();
        else
        {
            while (Query.next()) // Вернётся только 1 запись
                Result.first = Query.value("delete_user_from_chat").toUInt();
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
quint8 TMSGServer::deleteChat(const QUuid inChatUuid) // Метод удалит беседу
{
    quint8 Result = Res::rUnknown;

    QSqlQuery Query(TDB::Instance().DB());

    if(!Query.prepare("SELECT * FROM delete_chat(:in_chat_uuid)"))
        qDebug() << "[ОШИБКА]: " + Query.lastError().text();
    else
    {
        Query.bindValue(":in_chat_uuid", inChatUuid);

        if (!Query.exec())
            qDebug() << "[ОШИБКА]: " + Query.lastError().text();
        else
        {
            while (Query.next()) // Вернётся только 1 запись
                Result = Query.value("delete_chat").toUInt();
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
QString TMSGServer::ReadStringFromStream(QDataStream &inDataStream)
{
    QByteArray ByteBuf;
    inDataStream >> ByteBuf;

    return QString::fromUtf8(ByteBuf); // Помещаем буфер в результат (с раскодировкой из UTF8)
}
//-----------------------------------------------------------------------------

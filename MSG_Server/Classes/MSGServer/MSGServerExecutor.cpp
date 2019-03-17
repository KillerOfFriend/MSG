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

            qint32 Resuslt = creteUser(inStream); // Регистрация пользователя
            outStream << Command << Resuslt; // Пишем в результат команду и результат обработки
            sig_LogMessage(inClientSender->peerAddress(), "Отправка ответа о создании пользователя");

            break;
        }
        case Commands::Authorization: // Авторизация пользователя
        {
            sig_LogMessage(inClientSender->peerAddress(), "Получен запрос на авторизацию");

            std::pair<qint32, QUuid> Resuslt = canAuthorization(inStream); // Обрабатываем возможность авторизации

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
                    Users::TUserInfo UserInfo = getUserInfo(Resuslt.second); // Получаем информацию о пользователе из БД

                    if (UserInfo.userUuid() != Resuslt.second)
                    {
                        outStream << Command << Res::CanAut::caUserInfoError; // Пишем в результат команду и результат обработки
                        sig_LogMessage(inClientSender->peerAddress(), "Ошибка чтения данных записи");
                    }
                    else
                    {
                        QList<Users::TUserInfo> Contacts = getContacts(UserInfo.userUuid()); // Получаем список контактов пользователя

                        checkUsersStatus(Contacts); // Проверяем контакты онлайн и устанавливаем им статус

                        Users::TUserAccount UserAccount(this);
                        UserAccount.slot_SetUserInfo(UserInfo);
                        UserAccount.slot_SetContacts(Contacts);

                        slot_SetAuthorizedClient(inClientSender, UserAccount); // Авторизируем пользователя

                        outStream << Command << Resuslt.first << UserInfo << Contacts; // Пишем в результат команду и результат обработки
                        sig_LogMessage(inClientSender->peerAddress(), "Авторизация разрешена");
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
        case Commands::FindUsers: // Поиск пользователей
        {
            sig_LogMessage(inClientSender->peerAddress(), "Получен запрос на поиск пользователя");
            QList<Users::TUserInfo> Resuslt = findUsers(inStream); // Поиск пользователя

            if (Resuslt.isEmpty()) // Если список пуст
            {
                outStream << Command << Res::FindUsers::fuUsersNotFound; // Возвращаем результат (Пользователи не найдены)
                sig_LogMessage(inClientSender->peerAddress(), "Пользователи не найдены");
            }
            else
            {
                checkUsersStatus(Resuslt); // Проверяем контакты онлайн и устанавливаем им статус
                outStream << Command << Res::FindUsers::fuUsersFound << Resuslt; // Пишем в результат команду и результат обработки
                sig_LogMessage(inClientSender->peerAddress(), "Отправка списка пользователей");
            }

            break;
        }
        case Commands::AddContact: // Добавление контакта
        {
            sig_LogMessage(inClientSender->peerAddress(), "Получен запрос на добавление контакта");
            std::pair<qint32, Users::TUserInfo> Result = addContact(inStream); // Добавляем контакт

            if (Result.first != Res::AddContact::acCreated) // Если пользователь не был добавлен
                outStream << Command << Result.first; // Пишем в результат команду и результат обработки
            else // Если добавление прошло успешно
            {
                slot_AddContact(inClientSender, Result.second); // Добавляем контакт клиенту
                checkUserStatus(Result.second); // Проверяем его статус

                auto SenderIt = fClients.find(inClientSender); // Получаем данные о пославшем команду клиенте
                if (SenderIt != fClients.end()) // Если пославший найден
                    syncAddedUser(Result.second.userUuid(), *SenderIt->second.userInfo()); // Синхронизируем добовление контакта (с этим самым контактом)

                outStream << Command << Result.first << Result.second; // Пишем в результат команду, результат обработки и информацию о контакте
            }

            sig_LogMessage(inClientSender->peerAddress(), "Отправка результата добавления контакта");
            break;
        }
//        case Commands::GetContacts: // Запрос списка контактов
//        {
//            sig_LogMessage(inClientSender->peerAddress(), "Получен запрос списка контактов");
//            QList<TUserInfo> Resuslt = getContacts(inStream); // Поиск контактов

//            if (Resuslt.isEmpty()) // Если список пуст
//                outStream << Command << Res::GetContacts::gcUsersFound; // Возвращаем результат (Контакты не найдены)
//            else
//            {
//                sig_SetUserContacts(inClientSender, Resuslt); // Задаём пользователю список контактов
//                outStream << Command << Res::GetContacts::gcUsersFound << Resuslt; // Пишем в результат команду и результат обработки
//            }

//            sig_LogMessage(inClientSender->peerAddress(), "Отправка списка контактов");
//            break;
//        }
        case Commands::DeleteContact:
        {
            sig_LogMessage(inClientSender->peerAddress(), "Получен запрос на удаление контакта");
            std::pair<qint32, QUuid> Result = deleteContact(inStream); // Удаляем контакт

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

        default: sig_LogMessage(inClientSender->peerAddress(), "Получена неизвестная команда");
    }

    inClientSender->write(ReturningData); // Возвращаем результат
    //inClientSender->waitForBytesWritten();
}
//-----------------------------------------------------------------------------
qint32 TMSGServer::creteUser(QDataStream &inDataStream)
{
    quint32 Result = Res::rUnknown;

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
                Result = Query.value("create_user").toInt();
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
std::pair<qint32, QUuid> TMSGServer::canAuthorization(QDataStream &inDataStream) // Метод авторизирует пользователя
{
    std::pair<qint32, QUuid> Result;

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
                Result.first = Query.value("result_code").toInt();
                Result.second = Query.value("uuid_found_user").toByteArray();
            }
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
QList<Users::TUserInfo> TMSGServer::findUsers(QDataStream &inDataStream) // Метод вернёт список пользователей по их имени\логину
{
    QList<Users::TUserInfo> Result;
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
                    Result.push_back(getUserInfo(FindRes));
            }
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
Users::TUserInfo TMSGServer::getUserInfo(QUuid inUserUuid)
{
    Users::TUserInfo Result;

    QSqlQuery Query(TDB::Instance().DB());

    if(!Query.prepare("SELECT * FROM get_user_info(:in_uuid)"))
        qDebug() << "[ОШИБКА]: " + Query.lastError().text();
    else
    {
        Query.bindValue(":in_uuid", inUserUuid);

        if (!Query.exec())
            qDebug() << "[ОШИБКА]: " + Query.lastError().text();
        else
        {
            while (Query.next()) // Читаем все записи
            {
                Result.setUserUuid(Query.value("f_user_uuid").toUuid());
                Result.setUserType(Query.value("f_user_type").toUInt());
                Result.setUserLogin(QString::fromUtf8(Query.value("f_user_login").toByteArray()));
                Result.setUserName(QString::fromUtf8(Query.value("f_user_name").toByteArray()));
                Result.setUserBirthday(Query.value("f_user_birthday").toDate());
                Result.setUserRegistrationDate(Query.value("f_user_date_of_registration").toDate());
                Result.setUserIsMale(Query.value("f_user_is_male").toBool());

                QImage UserAvatar(Query.value("f_user_avatar").toByteArray());
                Result.setUserAvatar(UserAvatar);
            }
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
std::pair<qint32, Users::TUserInfo> TMSGServer::addContact(QDataStream &inDataStream) // Метод добавит котнтакт пользователю
{
    std::pair<qint32, Users::TUserInfo> Result;
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
                Result.first = Query.value("create_contact").toInt();

            if (Result.first == Res::AddContact::acCreated) // Если контакт был добавлен
                Result.second = getUserInfo(NewContact); // Получаем информацию об это контакте
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
QList<Users::TUserInfo> TMSGServer::getContacts(const QUuid &inOwnerUuid) // Метод вернёт список контактов по uuid указанного пользователя
{
    QList<Users::TUserInfo> Result;
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
                    Result.push_back(getUserInfo(FindRes));
            }
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
QList<Users::TUserInfo> TMSGServer::getContacts(QDataStream &inDataStream) // Метод вернёт список контактов указанного пользователя
{
    QUuid OwnerUuid;

    inDataStream >> OwnerUuid; // Получаем владельца контактов
    return getContacts(OwnerUuid);
}
//-----------------------------------------------------------------------------
std::pair<qint32, QUuid> TMSGServer::deleteContact(QDataStream &inDataStream) // Метод удалит котнтакт пользователю
{
    std::pair<qint32, QUuid> Result;
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
                Result.first = Query.value("delete_contacts").toInt();

            if (Result.first == Res::DeleteContact::dcContactRemove) // Если контакт был удалён
                Result.second = Contact; // Запоминаем его Uuid

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

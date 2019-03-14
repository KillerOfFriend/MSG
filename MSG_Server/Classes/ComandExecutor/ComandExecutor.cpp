#include "ComandExecutor.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>

#include "comandes.h"
#include "resultcodes.h"

#include "Classes/DB/DB.h"
#include "Classes/DataModule/DataModule.h"
#include "Classes/UserAccount/UserAccount.h"

//-----------------------------------------------------------------------------
TComandExecutor::TComandExecutor(QObject *inParent) : QObject(inParent)
{

}
//-----------------------------------------------------------------------------
void TComandExecutor::executCommand(QTcpSocket* inClientSender)
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
                case Res::CanAut::caAuthorizationTrue: // Вользователь найден (авторизация возможна)
                {
                    TUserInfo UserInfo = getUserInfo(Resuslt.second); // Получаем информацию о пользователе из БД

                    if (UserInfo.userUuid() != Resuslt.second)
                    {
                        outStream << Command << Res::CanAut::caUserInfoError; // Пишем в результат команду и результат обработки
                        sig_LogMessage(inClientSender->peerAddress(), "Ошибка чтения данных записи");
                    }
                    else
                    {
                        QList<TUserInfo> Contacts = getContacts(UserInfo.userUuid()); // Получаем список контактов пользователя

                        TUserAccount UserAccount(this);
                        UserAccount.slot_SetUserInfo(UserInfo);
                        UserAccount.slot_SetContacts(Contacts);

                        outStream << Command << Resuslt.first << UserInfo << Contacts; // Пишем в результат команду и результат обработки
                        sig_SetUserInfo(inClientSender, UserAccount); // Авторизируем пользователя
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
        case Commands::FindUsers: // Поиск пользователей
        {
            sig_LogMessage(inClientSender->peerAddress(), "Получен запрос на поиск пользователя");
            QList<TUserInfo> Resuslt = findUsers(inStream); // Поиск пользователя

            if (Resuslt.isEmpty()) // Если список пуст
                outStream << Command << Res::FindUsers::fuUsersNotFound; // Возвращаем результат (Пользователи не найдены)
            else
                outStream << Command << Res::FindUsers::fuUsersFound << Resuslt; // Пишем в результат команду и результат обработки

            sig_LogMessage(inClientSender->peerAddress(), "Отправка списка пользователей");
            break;
        }
        case Commands::AddContact: // Добавление контакта
        {
            sig_LogMessage(inClientSender->peerAddress(), "Получен запрос на добавление контакта");
            qint32 Result = addContact(inStream); // Добавляем контакт

            outStream << Command << Result; // Пишем в результат команду и результат обработки
            sig_LogMessage(inClientSender->peerAddress(), "Отправка результата добавления контакта");
            break;
        }
        case Commands::GetContacts: // Запрос списка контактов
        {
            sig_LogMessage(inClientSender->peerAddress(), "Получен запрос списка контактов");
            QList<TUserInfo> Resuslt = getContacts(inStream); // Поиск контактов

            if (Resuslt.isEmpty()) // Если список пуст
                outStream << Command << Res::GetContacts::gcUsersFound; // Возвращаем результат (Контакты не найдены)
            else
            {
                sig_SetUserContacts(inClientSender, Resuslt); // Задаём пользователю список контактов
                outStream << Command << Res::GetContacts::gcUsersFound << Resuslt; // Пишем в результат команду и результат обработки
            }

            sig_LogMessage(inClientSender->peerAddress(), "Отправка списка контактов");
            break;
        }
    case Commands::DeleteContact:
    {
        sig_LogMessage(inClientSender->peerAddress(), "Получен запрос на удаление контакта");
        qint32 Result = deleteContact(inStream); // Добавляем контакт

        outStream << Command << Result; // Пишем в результат команду и результат обработки
        sig_LogMessage(inClientSender->peerAddress(), "Отправка результата удаления контакта");
        break;
    }

        default: sig_LogMessage(inClientSender->peerAddress(), "Получена неизвестная команда");
    }

    inClientSender->write(ReturningData); // Возвращаем результат
    //inClientSender->waitForBytesWritten();
}
//-----------------------------------------------------------------------------
qint32 TComandExecutor::creteUser(QDataStream &inDataStream)
{
    quint32 Result = Res::rUnknown;

    QString Login = ReadStringFromStream(inDataStream);

    QByteArray PasswordHash;
    inDataStream >> PasswordHash;

    QSqlQuery Query(TDB::Instance().DB());

    if(!Query.prepare("SELECT * FROM create_user(:in_login, :in_password, :in_name)"))
        qDebug() << "[ОШИБКА]: " + Query.lastError().text();
    else
    {
        Query.bindValue(":in_login", Login.toUtf8());
        Query.bindValue(":in_password", QString(PasswordHash).toUtf8());
        Query.bindValue(":in_name", Login.toUtf8());

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
std::pair<qint32, QUuid> TComandExecutor::canAuthorization(QDataStream &inDataStream) // Метод авторизирует пользователя
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
QList<TUserInfo> TComandExecutor::findUsers(QDataStream &inDataStream) // Метод вернёт список пользователей по их имени\логину
{
    QList<TUserInfo> Result;
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
TUserInfo TComandExecutor::getUserInfo(QUuid inUserUuid)
{
    TUserInfo Result;

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
                Result.setUserUuid(Query.value("found_user_uuid").toUuid());
                Result.setUserType(Query.value("found_user_type").toUInt());
                Result.setUserLogin(QString::fromUtf8(Query.value("found_user_login").toByteArray()));
                Result.setUserName(QString::fromUtf8(Query.value("found_user_name").toByteArray()));
            }
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
qint32 TComandExecutor::addContact(QDataStream &inDataStream) // Метод добавит котнтакт пользователю
{
    qint32 Result = Res::rUnknown;

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
                Result = Query.value("create_contact").toInt();
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
QList<TUserInfo> TComandExecutor::getContacts(const QUuid &inOwnerUuid) // Метод вернёт список контактов по uuid указанного пользователя
{
    QList<TUserInfo> Result;
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
QList<TUserInfo> TComandExecutor::getContacts(QDataStream &inDataStream) // Метод вернёт список контактов указанного пользователя
{
    QUuid OwnerUuid;

    inDataStream >> OwnerUuid; // Получаем владельца контактов
    return getContacts(OwnerUuid);
}
//-----------------------------------------------------------------------------
qint32 TComandExecutor::deleteContact(QDataStream &inDataStream) // Метод удалит котнтакт пользователю
{
    quint32 Result = Res::rUnknown;

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
                Result = Query.value("delete_contacts").toInt();
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
QString TComandExecutor::ReadStringFromStream(QDataStream &inDataStream)
{
    QByteArray ByteBuf;
    inDataStream >> ByteBuf;

    return QString::fromUtf8(ByteBuf); // Помещаем буфер в результат (с раскодировкой из UTF8)
}
//-----------------------------------------------------------------------------

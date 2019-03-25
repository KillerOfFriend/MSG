#include "ServerCache.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

#include "Classes/DB/DB.h"

//-----------------------------------------------------------------------------
/**
 * @brief TServerCache::getUserInfoFromDB - Метод получит информацию о пользователе из БД и сохранит в кеше
 * @param inUserUuid - Uuid пользователя
 * @return Вернёт указатель на информацию о пользователе
 */
Users::UserInfo_Ptr TServerCache::uploadUserInfoFromDB(const QUuid inUserUuid)
{
    Users::UserInfo_Ptr Result = nullptr;

    QSqlQuery Query(TDB::Instance().DB());

    if(!Query.prepare("SELECT * FROM get_user_info(:in_uuid)")) // Подготавливаем данные ХП на запрос
        qDebug() << "[ОШИБКА]: " + Query.lastError().text();
    else
    {
        Query.bindValue(":in_uuid", inUserUuid);

        if (!Query.exec()) // Выполняем запрос
            qDebug() << "[ОШИБКА]: " + Query.lastError().text();
        else
        {
            Result = std::make_shared<Users::TUserInfo>(this); // Выделяем память под результат
            while (Query.next()) // Читаем результат
            {
                Result->setUserUuid(Query.value("f_user_uuid").toUuid());
                Result->setUserType(Query.value("f_user_type").toUInt());
                Result->setUserLogin(QString::fromUtf8(Query.value("f_user_login").toByteArray()));
                Result->setUserName(QString::fromUtf8(Query.value("f_user_name").toByteArray()));
                Result->setUserBirthday(Query.value("f_user_birthday").toDate());
                Result->setUserRegistrationDate(Query.value("f_user_date_of_registration").toDate());
                Result->setUserIsMale(Query.value("f_user_is_male").toBool());

                QImage UserAvatar(Query.value("f_user_avatar").toByteArray());
                Result->setUserAvatar(UserAvatar);
            }
            fUsersCache->insert(std::make_pair(Result->userUuid(), Result)); // Добавляем результат в кеш
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
/**
 * @brief TServerCache::getChatInfoFromDB - Метод получит информацию о беседе из БД и сохранит в кеше
 * @param inChatUuid - Uuid беседы
 * @return Вернёт указатель на информацию о беседе
 */
Users::ChatInfo_Ptr TServerCache::uploadChatInfoFromDB(const QUuid inChatUuid)
{
    Users::ChatInfo_Ptr Result = nullptr;

    QSqlQuery Query(TDB::Instance().DB());

    if(!Query.prepare("SELECT * FROM get_chat_info(:in_chat_uuid)")) // Подготавливаем данные ХП на запрос
        qDebug() << "[ОШИБКА]: " + Query.lastError().text();
    else
    {
        Query.bindValue(":in_chat_uuid", inChatUuid);

        if (!Query.exec()) // Выполняем запрос
            qDebug() << "[ОШИБКА]: " + Query.lastError().text();
        else
        {
            Result = std::make_shared<Users::TChatInfo>(this); // Выделяем память под результат
            while (Query.next())
            {
                Result->setChatUuid(Query.value("f_chat_uuid").toUuid());
                Result->setChatName(QString::fromUtf8(Query.value("f_chat_name").toByteArray()));
                Result->setChatPrivateStatus(Query.value("f_chat_is_private").toBool());

                QList<QUuid> ChatUsersBuf = getChatUsers(Result->chatUuid()); // Получаем список пользователей чата
//                Result.slot_SetClients(ChatUsersBuf); // Задаём список пользователей чата
            }
            fChatsCache->insert(std::make_pair(Result->chatUuid(), Result)); // Добавляем результат в кеш
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
/**
 * @brief TServerCache::getChatUsers - Метод получит список Uuid'ов пользователей указанной беседы
 * @param inChatUuid - Uuid беседы
 * @return Вернёт список Uuid'ов пользователей чатов
 */
QList<QUuid> TServerCache::getChatUsers(QUuid inChatUuid)
{
    QList<QUuid> Result;

    QSqlQuery Query(TDB::Instance().DB());

    if(!Query.prepare("SELECT * FROM get_chat_users(:in_chat_uuid)")) // Подготавливаем данные ХП на запрос
        qDebug() << "[ОШИБКА]: " + Query.lastError().text();
    else
    {
        Query.bindValue(":in_chat_uuid", inChatUuid);

        if (!Query.exec()) // Выполняем запрос
            qDebug() << "[ОШИБКА]: " + Query.lastError().text();
        else
        {
            while (Query.next())
                Result.push_back(Query.value("found_users_uuid").toUuid());
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------

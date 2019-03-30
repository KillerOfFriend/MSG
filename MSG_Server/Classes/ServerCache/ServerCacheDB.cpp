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
Core::UserInfo_Ptr TServerCache::uploadUserInfoFromDB(const QUuid inUserUuid)
{
    Core::UserInfo_Ptr Result = nullptr;

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
            Result = std::make_shared<Core::TUserInfo>(this); // Выделяем память под результат
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
Core::ChatInfo_Ptr TServerCache::uploadChatInfoFromDB(const QUuid inChatUuid)
{
    Core::ChatInfo_Ptr Result = nullptr;

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
            Result = std::make_shared<Core::TChatInfo>(this); // Выделяем память под результат
            while (Query.next())
            {
                Result->setChatUuid(Query.value("f_chat_uuid").toUuid());
                Result->setChatName(QString::fromUtf8(Query.value("f_chat_name").toByteArray()));
                Result->setChatPrivateStatus(Query.value("f_chat_is_private").toBool());

                QList<Core::UserInfo_Ptr> ChatUsersBuf = getChatUsers(Result->chatUuid()); // Получаем список пользователей чата
                Result->slot_SetClients(ChatUsersBuf); // Задаём список пользователей чата
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
 * @return Вернёт список пользователей чатов
 */
QList<Core::UserInfo_Ptr> TServerCache::getChatUsers(QUuid inChatUuid)
{
    QList<Core::UserInfo_Ptr> Result;

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
            {
                QUuid UserUuid = Query.value("found_users_uuid").toUuid(); // Получаем uuid пользователя
                if (!UserUuid.isNull())
                {
                    Core::UserInfo_Ptr UiserInfo = getUserInfo(UserUuid); // Получаем данные о пользователе
                    if (UiserInfo) // Если пользователь валиден
                        Result.push_back(UiserInfo); // Добавляем его в список
                }
            }
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------

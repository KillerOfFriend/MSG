#include "ServerCache.h"

#include "resultcodes.h"

//-----------------------------------------------------------------------------
TServerCache::TServerCache(QObject *inParent) : QObject(inParent)
{
    fUsersCache.reset(new std::map<QUuid, Core::UserInfo_Ptr>());
    fChatsCache.reset(new std::map<QUuid, Core::ChatInfo_Ptr>());
}
//-----------------------------------------------------------------------------
TServerCache::~TServerCache()
{
    fChatsCache->clear();
    fUsersCache->clear();
}
//-----------------------------------------------------------------------------
/**
 * @brief TServerCache::getUserInfo - Метод вернёт указатель на информацию о пользователе
 * @param inUserUuid - Uuid пользователя
 * @return Вернёт указатель на информацию о пользователе
 */
Core::UserInfo_Ptr TServerCache::getUserInfo(const QUuid inUserUuid)
{
    auto UserFindRes = fUsersCache->find(inUserUuid); // Ищим пользователя в кеше

    if (UserFindRes == fUsersCache->end()) // Если в кеше нет
        return uploadUserInfoFromDB(inUserUuid); // Запрашиваем из базы и сохраняем в кеш
    else // Если найдена в кеше
        return UserFindRes->second; // Возвращаем из кеша
}
//-----------------------------------------------------------------------------
/**
 * @brief TServerCache::getChatInfo - Метод вернёт указатель на информацию о беседе
 * @param inChatUuid - Uuid беседы
 * @return Вернёт указатель на информацию о беседе
 */
Core::ChatInfo_Ptr TServerCache::getChatInfo(const QUuid inChatUuid)
{
    auto UserChatRes = fChatsCache->find(inChatUuid); // Ищим беседу в кеше

    if (UserChatRes == fChatsCache->end()) // Если в кеше нет
        return uploadChatInfoFromDB(inChatUuid); // Запрашиваем из базы и сохраняем в кеш
    else // Если найдена в кеше
        return UserChatRes->second; // Возвращаем из кеша
}
//-----------------------------------------------------------------------------
/**
 * @brief TServerCache::isContainUser - Метод проверит наличие пользователя в кеше
 * @param inUserUuid - Uuid пользователя
 * @return Флаг присутствия
 */
bool TServerCache::isContainUser(const QUuid inUserUuid)
{
    return (fUsersCache->find(inUserUuid) != fUsersCache->end());
}
//-----------------------------------------------------------------------------
/**
 * @brief TServerCache::isContainChat - Метод проверит наличие беседы в кеше
 * @param inChatUuid - Uuid беседы
 * @return Флаг присутствия
 */
bool TServerCache::isContainChat(const QUuid inChatUuid)
{
    return (fChatsCache->find(inChatUuid) != fChatsCache->end());
}
//-----------------------------------------------------------------------------

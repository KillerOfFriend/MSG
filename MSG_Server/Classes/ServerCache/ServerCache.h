#ifndef SERVERCACHE_H
#define SERVERCACHE_H

#include <memory>
#include <map>

#include <QObject>
#include <QUuid>
#include <QList>

#include "Classes/UserInfo/UserInfo.h"
#include "Classes/ChatInfo/ChatInfo.h"

class TServerCache : public QObject
{
    Q_OBJECT
public:
    explicit TServerCache(QObject *inParent = nullptr);
    ~TServerCache();

    Core::UserInfo_Ptr getUserInfo(const QUuid inUserUuid); // Метод вернёт указатель на информацию о пользователе
    Core::ChatInfo_Ptr getChatInfo(const QUuid inChatUuid); // Метод вернёт указатель на информацию о беседе

    bool isContainUser(const QUuid inUserUuid); // Метод проверит наличие пользователя в кеше
    bool isContainChat(const QUuid inChatUuid); // Метод проверит наличие беседы в кеше

private:
    std::unique_ptr<std::map<QUuid, Core::UserInfo_Ptr>> fUsersCache = nullptr; // Контейнер с пользователями
    std::unique_ptr<std::map<QUuid, Core::ChatInfo_Ptr>> fChatsCache = nullptr; // Контейнер с беседами

    Core::UserInfo_Ptr uploadUserInfoFromDB(const QUuid inUserUuid); // Метод получит информацию о пользователе из БД и сохранит в кеше
    Core::ChatInfo_Ptr uploadChatInfoFromDB(const QUuid inChatUuid); // Метод получит информацию о беседе из БД и сохранит в кеше

    QList<Core::UserInfo_Ptr> getChatUsers(QUuid inChatUuid); // Метод получит список пользователей указанной беседы

public slots:

};

#endif // SERVERCACHE_H

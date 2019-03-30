#ifndef CHATINFO_H
#define CHATINFO_H

#include <map>
#include <list>
#include <memory>

#include <QObject>
#include <QUuid>

#include "Classes/UserInfo/UserInfo.h"
#include "Classes/ChatMessage/ChatMessage.h"

namespace Core
{
    class TChatInfo;

    typedef std::shared_ptr<TChatInfo> ChatInfo_Ptr;

    class TChatInfo : public QObject
    {
        Q_OBJECT
    public:
        explicit TChatInfo(QObject *inParent = nullptr);
        TChatInfo(const TChatInfo &inOther);
        ~TChatInfo();

        TChatInfo &operator =(const TChatInfo &inOther);

        void setChatUuid(QUuid inUuid); // Метод задаст UUid беседы
        QUuid chatUuid() const; // Метод вернёт Uuid беседы

        void setChatName(QString inName); // Метод задаст имя беседы
        QString chatName() const; // Метод вернёт имя беседы

        void setChatPrivateStatus(bool inPrivateStatus); // Метод задаст статус приватности беседы
        bool chatPrivateStatus() const; // Метод вернёт статус приватности беседы

        std::shared_ptr<std::map<QUuid, UserInfo_Ptr>> clients(); // Метод вернёт список клиентов беседы
        void addUser(UserInfo_Ptr inUserInfo); // Метод добавит пользователя в беседу
        void deleteUser(QUuid inUserUuid); // Метод удалит пользователя из беседы

        std::shared_ptr<std::list<ChatMessage_Ptr>> messages(); // Метод вернёт список сообщений беседы
        void addMessage(ChatMessage_Ptr inChatMessage); // Метод добавит сообщение

        friend QDataStream& operator <<(QDataStream&, const TChatInfo&);
        friend QDataStream& operator >>(QDataStream&, TChatInfo&);

    private:
        QUuid fUuid; // Uuid беседы
        QString fName; // Название беседы
        bool fPrivateStatus = true; // Флаг приватности беседы (в приватную беседу нельзя добавить пользователя)
        std::shared_ptr<std::map<QUuid, UserInfo_Ptr>> fClients = nullptr; // Список клиентов беседы
        std::shared_ptr<std::list<ChatMessage_Ptr>> fMessages = nullptr; // Список сообщений

    signals:
        void sig_ChatUserAdded(QUuid inChatUuid, UserInfo_Ptr inUserUuid); // Сигнал о добавлении пользователя в чат
        void sig_ChatIsEmpty(QUuid inChatUuid); // Сигнал, посылающий QUuid опустевшено чата
        void sig_ChatUserDeleted(QUuid inChatUuid, QUuid inUserUuid); // Сигнал о удалении пользователя из чата

    public slots:
        void slot_SetClients(QList<UserInfo_Ptr> &inClientList); // Слот задаст список клиентов беседы
    };

}

#endif // CHATINFO_H

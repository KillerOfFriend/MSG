#ifndef CHATINFO_H
#define CHATINFO_H

#include <set>
#include <memory>

#include <QObject>
#include <QUuid>

#include "Classes/UserInfo/UserInfo.h"

namespace Users
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

        std::size_t usersCount(); // Метод вернёт кол-во пользователей
        void addUser(QUuid inUserUuid); // Метод добавит пользователя в беседу
        void deleteUser(QUuid inUserUuid); // Метод удалит пользователя из беседы
        QUuid user(std::size_t inIndex); // Метод вернёт Uuid пользователя по индексу

        friend QDataStream& operator <<(QDataStream&, const TChatInfo&);
        friend QDataStream& operator >>(QDataStream&, TChatInfo&);

    private:
        QUuid fUuid; // Uuid беседы
        QString fName; // Название беседы
        bool fPrivateStatus = true; // Флаг приватности беседы (в приватную беседу нельзя добавить пользователя)
        std::set<QUuid> fClients; // Список клиентов беседы

    signals:
        void sig_ChatUserAdded(QUuid inChatUuid, QUuid inUserUuid); // Сигнал о добавлении пользователя в чат
        void sig_ChatIsEmpty(QUuid inChatUuid); // Сигнал, посылающий QUuid опустевшено чата
        void sig_ChatUserDeleted(QUuid inChatUuid, QUuid inUserUuid); // Сигнал о удалении пользователя из чата

    public slots:
        void slot_SetClients(QList<QUuid> &inClientList); // Слот задаст список клиентов беседы
    };

}

#endif // CHATINFO_H

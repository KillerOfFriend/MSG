#ifndef USERACCOUNT_H
#define USERACCOUNT_H

#include <memory>
#include <map>

#include <QObject>
#include <QList>
#include <QUuid>
#include <QAbstractTableModel>

#include "Classes/UserInfo/UserInfo.h"
#include "Models/UsersModel/UsersModel.h"
#include "Models/ChatsModel/ChatsModel.h"

namespace Users
{

    class TUserAccount : public QObject
    {
        Q_OBJECT
    public:
        explicit TUserAccount(QObject *inParent = nullptr);
        TUserAccount(const TUserAccount &inOther);
        ~TUserAccount();

        TUserAccount &operator =(const TUserAccount &inOther);

        std::shared_ptr<TUserInfo> userInfo() const; // Метод вернёт информацию о пользователе
        std::shared_ptr<TUsersModel> contacts() const; // Метод вернёт список контактов
        std::shared_ptr<TChatsModel> chats() const; // Метод вернёт список бесед

    private:
        std::shared_ptr<TUserInfo> fUserInfo = nullptr; // Данные о пользователе
        std::shared_ptr<TUsersModel> fContacts = nullptr; // Список контактов
        std::shared_ptr<TChatsModel> fChats = nullptr; // Список бесед пользователя

    signals:

    public slots:
        void slot_SetUserInfo(const TUserInfo &inUserInfo); // Слот задаст данные пользователя
        void slot_SetContacts(const QList<TUserInfo> &inContacts); // Слот задаст список контактов
        void slot_SetChats(const QList<TChatInfo> &inChats); // Слот задаст список бесед
        void slot_ContactChangeStatus(QUuid inContactUuid, quint8 inNewStatus); // Сот установит контакту указанный статус

    };

}

#endif // USERACCOUNT_H

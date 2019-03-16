#ifndef USERACCOUNT_H
#define USERACCOUNT_H

#include <memory>

#include <QObject>
#include <QList>
#include <QAbstractTableModel>

#include "Classes/UserInfo/UserInfo.h"
#include "Models/UsersModel/UsersModel.h"

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

    private:
        std::shared_ptr<TUserInfo> fUserInfo = nullptr; // Данные о пользователе
        std::shared_ptr<TUsersModel> fContacts = nullptr; // Список контактов

    signals:

    public slots:
        void slot_SetUserInfo(const TUserInfo &inUserInfo); // Слот задаст данные пользователя
        void slot_SetContacts(const QList<TUserInfo> &inContacts); // Слот задаст список контактов
        void slot_ContactChangeStatus(QUuid inContactUuid, quint8 inNewStatus); // Сот установит контакту указанный статус

    };

}

#endif // USERACCOUNT_H

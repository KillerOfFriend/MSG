#ifndef USERACCOUNT_H
#define USERACCOUNT_H

#include <memory>

#include <QObject>
#include <QList>

#include "Classes/UserInfo/UserInfo.h"
#include "Models/UsersModel/UsersModel.h"

class TUserAccount : public QObject
{
    Q_OBJECT
public:
    explicit TUserAccount(QObject *inParent = nullptr);
    TUserAccount(const TUserAccount &inOther);
    ~TUserAccount();

    TUserAccount &operator =(const TUserAccount &inOther);

    TUserInfo UserInfo; // Данные о пользователе
    TUsersModel Contscts; // Список контактов

signals:

public slots:
    void slot_SetUserInfo(const TUserInfo &inUserInfo); // Слот задаст данные пользователя
    void slot_SetContacts(const QList<TUserInfo> &inContacts); // Слот задаст список контактов

};

#endif // USERACCOUNT_H

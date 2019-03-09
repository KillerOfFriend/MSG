#ifndef USERACCOUNT_H
#define USERACCOUNT_H

#include <QObject>

#include "Classes/UserInfo/UserInfo.h"

class TUserAccount : public QObject
{
    Q_OBJECT
public:
    explicit TUserAccount(QObject *inParent = nullptr);
    ~TUserAccount();

    TUserInfo UserInfo; // Данные о пользователе

private:

signals:

public slots:
    void slot_SetUserinfo(const TUserInfo &inUserInfo); // Слот задаст данные пользователя
};

#endif // USERACCOUNT_H

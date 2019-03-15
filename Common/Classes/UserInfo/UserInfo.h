#ifndef USERINFO_H
#define USERINFO_H

#include <QObject>
#include <QUuid>
#include <QDataStream>

class TUserInfo : public QObject
{
    Q_OBJECT
public:
    enum eUserStatus : quint8
    {
        usUnknown = 0,
        usOnline = 1,
        usOffline = 2
    };

    explicit TUserInfo(QObject *inParent = nullptr);
    TUserInfo(const TUserInfo &inOther);
    ~TUserInfo();

    TUserInfo& operator = (const TUserInfo &inOther);
    bool operator == (const TUserInfo &inOther) const;

    void setUserUuid(QUuid inUuid);
    QUuid userUuid() const;

    void setUserType(quint32 inType);
    quint32 userType() const;

    void setUserLogin(QString inLogin);
    QString userLogin() const;

    void setUserName(QString inName);
    QString userName() const;

    void setUserStatus(quint8 inStatus);
    quint8 userStatus() const;

    friend QDataStream& operator <<(QDataStream&, const TUserInfo&);
    friend QDataStream& operator >>(QDataStream&, TUserInfo&);

private:
    QUuid fUuid; // Уникальный идентификатор пользователя
    quint32 fType; // Тип пользователя
    QString fLogin; // Логин пользователя
    QString fName; // Имя пользователя
    quint8 fUserStatus = eUserStatus::usUnknown; // Статус пользователя

signals:

public slots:

};

#endif // USERINFO_H

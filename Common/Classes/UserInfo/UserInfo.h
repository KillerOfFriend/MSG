#ifndef USERINFO_H
#define USERINFO_H

#include <memory>

#include <QObject>
#include <QUuid>
#include <QPixmap>
#include <QDate>
#include <QDataStream>

namespace Core
{
    class TUserInfo;

    typedef std::shared_ptr<TUserInfo> UserInfo_Ptr;

    namespace UserStatus
    {
        const qint8 usUnknown = 0;
        const qint8 usOnline = 1;
        const qint8 usOffline = 2;
    }


    class TUserInfo : public QObject
    {
        Q_OBJECT
    public:


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

        void setUserRegistrationDate(QDate inRegDate);
        QDate userRegistrationDate() const;

        void setUserBirthday(QDate inBirthday);
        QDate userBirthday() const;

        void setUserIsMale(bool inIsMale);
        bool userIsMale() const;


        void setUserAvatar(QImage &inAvatar);
        QImage userAvatar() const;

        friend QDataStream& operator <<(QDataStream&, const TUserInfo&);
        friend QDataStream& operator >>(QDataStream&, TUserInfo&);

    private:
        QUuid fUuid; // Уникальный идентификатор пользователя
        quint32 fType = 0; // Тип пользователя
        QString fLogin; // Логин пользователя
        QString fName; // Имя пользователя
        quint8 fStatus = UserStatus::usUnknown; // Статус пользователя
        QDate fRegistrationDate; // Дата регистрации
        QDate fBirthday; // День рождения пользователя
        bool fIsMale = true; // Половая пренадлежность пользователя

        QImage fAvatar; // Аватар пользователя

    signals:

    public slots:

    };
}

#endif // USERINFO_H

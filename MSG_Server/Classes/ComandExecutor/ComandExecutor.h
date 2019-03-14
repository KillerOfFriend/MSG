#ifndef COMANDEXECUTOR_H
#define COMANDEXECUTOR_H

#include <memory>

#include <QUuid>
#include <QList>
#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <QHostAddress>

#include "Classes/UserAccount/UserAccount.h"

class TComandExecutor : public QObject
{
    Q_OBJECT
public:
    explicit TComandExecutor(QObject *inParent = nullptr);

    void executCommand(QTcpSocket* inSender); // Метод выполнит команду

private:
    qint32 creteUser(QDataStream &inDataStream); // Метод создаст нового пользователя
    std::pair<qint32, QUuid> canAuthorization(QDataStream &inDataStream); // Метод авторизирует пользователя
    TUserInfo getUserInfo(QUuid inUserUuid); // Метод вернёт инфо пользователя по Uuid
    QList<TUserInfo> findUsers(QDataStream &inDataStream); // Метод вернёт список пользователей по их имени\логину
    qint32 addContact(QDataStream &inDataStream); // Метод добавит котнтакт пользователю
    QList<TUserInfo> getContacts(QDataStream &inDataStream); // Метод вернёт список контактов по uuid указанного пользователя
    QList<TUserInfo> getContacts(const QUuid &inOwnerUuid); // Метод вернёт список контактов указанного пользователя
    qint32 deleteContact(QDataStream &inDataStream); // Метод удалит котнтакт пользователю

    QString ReadStringFromStream(QDataStream &inDataStream); // Метод прочитает строку из потока

signals:
    void sig_LogMessage(QHostAddress inAddres, QString inMessage); //
    void sigSocketError();
    void sigTimeOut();

    void sig_SetUserInfo(QTcpSocket* Client, TUserAccount &inUserInfo);
    void sig_SetUserContacts(QTcpSocket* Client, QList<TUserInfo> &inUserContacts);
public slots:
};

#endif // COMANDEXECUTOR_H

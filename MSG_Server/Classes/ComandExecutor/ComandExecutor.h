#ifndef COMANDEXECUTOR_H
#define COMANDEXECUTOR_H

#include <memory>

#include <QUuid>
#include <QList>
#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <QHostAddress>

#include "Classes/UserInfo/UserInfo.h"

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
    QList<TUserInfo> getContacts(QDataStream &inDataStream); // Метод вернёт список контактов по указанного пользователя

    QString ReadStringFromStream(QDataStream &inDataStream); // Метод прочитает строку из потока

signals:
    void sig_LogMessage(QHostAddress inAddres, QString inMessage); //
    void sigSocketError();
    void sigTimeOut();

    void sig_SetUserInfo(QTcpSocket* Client, TUserInfo &inUserInfo);
public slots:
};

#endif // COMANDEXECUTOR_H

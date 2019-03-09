#ifndef COMANDEXECUTOR_H
#define COMANDEXECUTOR_H

#include <memory>

#include <QUuid>
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

    QString ReadStringFromStream(QDataStream &inDataStream); // Метод прочитает строку из потока
    TUserInfo getUserInfo(QUuid inUserUuid); // Метод вернёт инфо пользователя по Uuid

signals:
    void sig_LogMessage(QHostAddress inAddres, QString inMessage); //
    void sigSocketError();
    void sigTimeOut();

    void sigSetUserInfo(QTcpSocket* Client, TUserInfo &inUserInfo);
public slots:
};

#endif // COMANDEXECUTOR_H

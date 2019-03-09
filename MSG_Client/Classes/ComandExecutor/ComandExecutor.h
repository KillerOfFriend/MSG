#ifndef COMANDEXECUTOR_H
#define COMANDEXECUTOR_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>

#include "Classes/UserInfo/UserInfo.h"

class TComandExecutor : public QObject
{
    Q_OBJECT
public:
    explicit TComandExecutor(QObject *inParent = nullptr);

    void executCommand(QTcpSocket* inSender);

private:
    void creteUserResult(QDataStream &inDataStream); // Мтод обработает результат регистрации
    void userAuthorization(QDataStream &inDataStream); // Метод обработает результат авторизации

    QString ReadStringFromStream(QDataStream &inDataStream);

signals:
    void sig_LogMessage(QString inMessage);
    void sig_SetUserInfo(const TUserInfo &inUserInfo);

    void sig_UserCreateResult(qint32 inResult);
    void sig_AuthorizationResult(qint32 inResult);


public slots:
};

#endif // COMANDEXECUTOR_H

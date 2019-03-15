#ifndef COMANDEXECUTOR_H
#define COMANDEXECUTOR_H

#include <QList>
#include <QObject>
#include <QTcpSocket>
#include <QDataStream>

#include "Classes/UserInfo/UserInfo.h"

class TComandExecutor : public QObject
{
    Q_OBJECT
public:
    explicit TComandExecutor(QObject *inParent = nullptr);

    void executCommand(QTcpSocket* inSender); // Метод исполняющий команды

private:
    void creteUserResult(QDataStream &inDataStream); // Мтод обработает результат регистрации
    void userAuthorization(QDataStream &inDataStream); // Метод обработает результат авторизации
    void findUsersResult(QDataStream &inDataStream); // Метод обработает результат поика пользователей
    void addContactResult(QDataStream &inDataStream); // Метод обработает результат добавления контакта
//    void getContactsResult(QDataStream &inDataStream); // Метод обработает результат запроса списка контактов
    void deleteContactResult(QDataStream &inDataStream); // Метод обработает результат удаления контакта

    QString ReadStringFromStream(QDataStream &inDataStream); // Метод считает строку из потока

signals:
    void sig_LogMessage(QString inMessage);
    void sig_SetUserInfo(const TUserInfo &inUserInfo);
    void sig_SetContacts(const QList<TUserInfo> &inUsers);

    void sig_UserCreateResult(qint32 inResult);
    void sig_AuthorizationResult(qint32 inResult);
    void sig_FindUsersResult(const QList<TUserInfo> &inUsers);
    void sig_AddContactResult(qint32 inResult, TUserInfo &inContactInfo);
    void sig_DeleteContactResult(qint32 inResult, QUuid &inContactUuid);


public slots:
};

#endif // COMANDEXECUTOR_H

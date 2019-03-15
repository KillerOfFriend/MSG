#ifndef MSGCLIENT_H
#define MSGCLIENT_H

#include <memory>

#include <QTcpSocket>
#include <QTimer>

#include "Classes/UserInfo/UserInfo.h"

class TMSGClient : public QObject
{
    Q_OBJECT
public:
    TMSGClient(QObject* inParent = nullptr);
    ~TMSGClient();

    bool connectToServer();
    void disconnect();
    bool isConnected();

    bool createUser(QString inLogin, QString inPassword); // Метод отправит команду на создание пользователья
    bool authorization(QString inLogin, QString inPassword); // Метод отправит номанду на авторизацию пользователя

    bool findUsers(QString inUserName); // Метод отправит команду на поиск пользователей
    bool addContact(QUuid inSelfUuid, QUuid inContactUuid); // Метод отправит команду на добавление контакта
//    bool getContacts(QUuid inSelfUuid); // Метод отправит команду на на возвращение списка контактов
    bool deleteContact(QUuid inSelfUuid, QUuid inContactUuid); // Метод отправит команду на удаление контакта

private:
    std::unique_ptr<QTcpSocket> fClient = nullptr;

    void init();
    void Link();

    void executCommand(QTcpSocket* inSender); // Метод исполняющий команды

    void creteUserResult(QDataStream &inDataStream); // Мтод обработает результат регистрации
    void userAuthorization(QDataStream &inDataStream); // Метод обработает результат авторизации
    void findUsersResult(QDataStream &inDataStream); // Метод обработает результат поика пользователей
    void addContactResult(QDataStream &inDataStream); // Метод обработает результат добавления контакта
//    void getContactsResult(QDataStream &inDataStream); // Метод обработает результат запроса списка контактов
    void deleteContactResult(QDataStream &inDataStream); // Метод обработает результат удаления контакта

    QString ReadStringFromStream(QDataStream &inDataStream); // Метод считает строку из потока

private slots: 
    void slot_ReadyRead();

    void slot_hostFound();
    void slot_connected();
    void slot_disconnected();

signals:
    void sig_LogMessage(QString inMessage);
    void sig_TimeOut(qint32 inResult);

    void sig_SetUserInfo(const TUserInfo &inUserInfo);
    void sig_SetContacts(const QList<TUserInfo> &inUsers);

    void sig_UserCreateResult(qint32 inResult);
    void sig_AuthorizationResult(qint32 inResult);
    void sig_FindUsersResult(const QList<TUserInfo> &inUsers);
    void sig_AddContactResult(qint32 inResult, TUserInfo &inContactInfo);
    void sig_DeleteContactResult(qint32 inResult, QUuid &inContactUuid);
};

#endif // MSGCLIENT_H

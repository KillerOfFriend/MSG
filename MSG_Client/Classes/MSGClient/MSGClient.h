#ifndef MSGCLIENT_H
#define MSGCLIENT_H

#include <memory>

#include <QTcpSocket>
#include <QTimer>

#include "othertypes.h"
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

    bool createUser(QString inLogin, QString inPassword, QString inName, bool inIsMale); // Метод отправит команду на создание пользователья
    bool authorization(QString inLogin, QString inPassword); // Метод отправит номанду на авторизацию пользователя
    bool getUserTypes(); // Метод отправит запрос получение списка типов пользователей

    bool findUsers(QString inUserName); // Метод отправит команду на поиск пользователей
    bool addContact(QUuid inSelfUuid, QUuid inContactUuid); // Метод отправит команду на добавление контакта
//    bool getContacts(QUuid inSelfUuid); // Метод отправит команду на на возвращение списка контактов
    bool deleteContact(QUuid inSelfUuid, QUuid inContactUuid); // Метод отправит команду на удаление контакта

    bool createChat(QUuid inChatUuid, bool inPrivate, QList<QUuid> &inUsersList); // Метод отправит команду на создание беседы

private:
    std::unique_ptr<QTcpSocket> fClient = nullptr;

    void init();
    void Link();

    void executCommand(QTcpSocket* inSender); // Метод исполняющий команды

    void creteUserResult(QDataStream &inDataStream); // Мтод обработает результат регистрации
    void userAuthorization(QDataStream &inDataStream); // Метод обработает результат авторизации
    void getUserTypesResult(QDataStream &inDataStream); // Метод обработает результат запроса списка типов пользователей
    void findUsersResult(QDataStream &inDataStream); // Метод обработает результат поика пользователей
    void addContactResult(QDataStream &inDataStream); // Метод обработает результат добавления контакта
//    void getContactsResult(QDataStream &inDataStream); // Метод обработает результат запроса списка контактов
    void deleteContactResult(QDataStream &inDataStream); // Метод обработает результат удаления контакта
    void contactChangeStatus(QDataStream &inDataStream); // Метод обработает сообщение о смене статуса контакта

    QString ReadStringFromStream(QDataStream &inDataStream); // Метод считает строку из потока

private slots: 
    void slot_ReadyRead();

    void slot_hostFound();
    void slot_connected();
    void slot_disconnected();

signals:
    void sig_LogMessage(QString inMessage);
    void sig_TimeOut(qint32 inResult);

    void sig_SetUserInfo(const Users::TUserInfo &inUserInfo);
    void sig_SetContacts(const QList<Users::TUserInfo> &inUsers);

    void sig_UserCreateResult(qint32 inResult);
    void sig_AuthorizationResult(qint32 inResult);
    void sig_GetUserTypesResult(QList<OtherTypes::TUserType> &inUserTypes);
    void sig_FindUsersResult(const QList<Users::TUserInfo> &inUsers);
    void sig_AddContactResult(qint32 inResult, Users::TUserInfo &inContactInfo);
    void sig_DeleteContactResult(qint32 inResult, QUuid &inContactUuid);
    void sig_ContactChangeStatus(QUuid inContactUuid, quint8 inNewStatus);
};

#endif // MSGCLIENT_H

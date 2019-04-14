#ifndef MSGCLIENT_H
#define MSGCLIENT_H

#include <memory>

#include <QTcpSocket>
#include <QTimer>

#include "othertypes.h"
#include "Classes/UserInfo/UserInfo.h"
#include "Classes/ChatInfo/ChatInfo.h"

class TMSGClient : public QObject
{
    Q_OBJECT
public:
    TMSGClient(QObject* inParent = nullptr);
    ~TMSGClient();

    bool connectToServer();
    void disconnect();
    bool isConnected();
    std::shared_ptr<QTcpSocket> clientSocket(); // Метод вернёт сокет клиента

    /*!!!*/void executCommand(QTcpSocket* inSender); // Метод исполняющий команды

    //--
    bool createUser(QString inLogin, QString inPassword, QString inName, bool inIsMale); // Метод отправит команду на создание пользователья
    bool authorization(QString inLogin, QString inPassword); // Метод отправит номанду на авторизацию пользователя
    bool getUserTypes(); // Метод отправит запрос получение списка типов пользователей
    //--
    bool findUsers(QString inUserName); // Метод отправит команду на поиск пользователей
    bool addContact(QUuid inSelfUuid, QUuid inContactUuid); // Метод отправит команду на добавление контакта
    bool deleteContact(QUuid inSelfUuid, QUuid inContactUuid); // Метод отправит команду на удаление контакта
    //--
    bool createChat(Core::TChatInfo &inChatInfo); // Метод отправит команду на создание беседы
    bool leaveFromChat(const QUuid inChatUuid); // Метод удалит беседу (По факту, удалит из беседы пользователя)
    bool sendMessage(QUuid inChatUuid, Core::TChatMessage &inMessage); // Метод отправит сообщение
    //--

private:
    std::shared_ptr<QTcpSocket> fClient = nullptr;

    void init();
    void Link();

    //void executCommand(QTcpSocket* inSender); // Метод исполняющий команды
    //--
    void creteUserResult(QDataStream &inDataStream); // Мтод обработает результат регистрации
    void userAuthorization(QDataStream &inDataStream); // Метод обработает результат авторизации
    void getUserTypesResult(QDataStream &inDataStream); // Метод обработает результат запроса списка типов пользователей
    //--
    void findUsersResult(QDataStream &inDataStream); // Метод обработает результат поика пользователей
    void addContactResult(QDataStream &inDataStream); // Метод обработает результат добавления контакта
    void deleteContactResult(QDataStream &inDataStream); // Метод обработает результат удаления контакта
    //--
    void createChatResult(QDataStream &inDataStream); // Метод обработает результат создания беседы
    void inviteToChatResult(QDataStream &inDataStream); // Метод обработает добавление в беседу
    void deleteUserFromChatResult(QDataStream &inDataStream); // Метод обработает удаление из беседы
    void leaveFromChatResult(QDataStream &inDataStream); // Метод обработает выход из беседы

    void contactChangeStatus(QDataStream &inDataStream); // Метод обработает сообщение о смене статуса контакта

    QString ReadStringFromStream(QDataStream &inDataStream); // Метод считает строку из потока

private slots: 
    void slot_ReadyRead();

    void slot_hostFound();
    void slot_connected();
    void slot_disconnected();

signals:
    void sig_LogMessage(QString inMessage);
    void sig_TimeOut(quint8 inResult);
    //--
    void sig_SetUserInfo(const Core::TUserInfo &inUserInfo);
    void sig_SetContacts(const QList<Core::TUserInfo> &inUsers);
    void sig_SetChats(const QList<Core::TChatInfo> &inChats);
    //--
    void sig_UserCreateResult(quint8 inResult);
    void sig_AuthorizationResult(quint8 inResult);
    void sig_GetUserTypesResult(QList<OtherTypes::TUserType> &inUserTypes);
    //--
    void sig_FindUsersResult(const QList<Core::TUserInfo> &inUsers);
    //--
    void sig_InviteToChatResult(const Core::ChatInfo_Ptr inChat);

    void sig_ContactChangeStatus(QUuid inContactUuid, quint8 inNewStatus);
};

#endif // MSGCLIENT_H

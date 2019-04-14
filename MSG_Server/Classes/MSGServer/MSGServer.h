#ifndef MSGSERVER_H
#define MSGSERVER_H

#include <map>
#include <set>
#include <memory>
#include <QHostAddress>

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include "Classes/ServerCache/ServerCache.h"
#include "Models/ConnectedUsers/ConnectedUsers.h"

class TMSGServer : public QObject
{
    Q_OBJECT
public:
    explicit TMSGServer(QObject *inParent = nullptr);
    ~TMSGServer();

    bool isActive(); // Метод вернёт флаг активности сервера
    void disconnectClient(QTcpSocket* inClient); // Метод отключит клиента
    void disconnectAll(); // Метод отключит всех клиентов

    QHostAddress serverAddress() const; // Метод вернёт адрес сервера
    quint16 serverPort() const; // Метод вернёт порт сервера

    QAbstractTableModel* clientsModel(); // Метод вернёт модель данных подключённых клиентов    

private:    
    std::unique_ptr<QTcpServer> fServer = nullptr; // Сервер
    std::unique_ptr<TServerCache> fServerCache = nullptr; // Кеш

    TConnectedUsersModel fClients; // Список подключённых клиентов
    std::map<QUuid, Core::ChatInfo_Ptr> fChats; // Список бесед

    //---
    void init(); // Метод инициализирует класс
    void Link(); // Метод соединит сигналы\слоты

    QTcpSocket* socketByUuid(QUuid inClientUuid); // Метод вернёт сокет клиента если он онлайн
    void executCommand(QTcpSocket* inSender); // Метод выполнит команду

    quint8 creteUser(QDataStream &inDataStream); // Метод создаст нового пользователя
    std::pair<quint8, QUuid> canAuthorization(QDataStream &inDataStream); // Метод авторизирует пользователя

    QList<Core::UserInfo_Ptr> findUsers(QDataStream &inDataStream); // Метод вернёт список пользователей по их имени\логину

    std::pair<quint8, Core::UserInfo_Ptr> addContact(QDataStream &inDataStream); // Метод добавит котнтакт пользователю
    QList<Core::UserInfo_Ptr> getContacts(QDataStream &inDataStream); // Метод вернёт список контактов по uuid указанного пользователя
    QList<Core::UserInfo_Ptr> getContacts(const QUuid &inOwnerUuid); // Метод вернёт список контактов указанного пользователя
    std::pair<quint8, QUuid> deleteContact(QDataStream &inDataStream); // Метод удалит котнтакт пользователю

    quint8 createChat(QDataStream &inDataStream); // Метод добавит новую беседу
    QList<QUuid> findChats(QUuid inUserUuid); // Метод вернёт список бесед по uuid указанного пользователя
    quint8 addUserToChat(QUuid inChatUuid, QUuid inUserUuid); // Метод добавит пользователя в беседу
    QList<Core::ChatInfo_Ptr> getChats(const QUuid &inOwnerUuid); // Метод вернёт список бесед указонного пользователя
    std::pair<quint8, std::pair<QUuid,QUuid>> deleteUserFromChat(QDataStream &inDataStream); // Метод удалит пользователя из беседы
    quint8 deleteChat(const QUuid inChatUuid); // Метод удалит беседу
    quint8 sendMessage(QDataStream &inDataStream); // Метод примит и разашлёт сообщение

    QString ReadStringFromStream(QDataStream &inDataStream); // Метод прочитает строку из потока

    void checkUsersStatus(QList<Core::UserInfo_Ptr> &inUsers); // Метод проверит наличие контактов онлайн и установит их статус
    void checkUserStatus(Core::UserInfo_Ptr inUsers); // Метод проверит наличие контакта онлайнт и установит их статус
    void userChangeStatus(QTcpSocket* inClientSender, quint8 inNewStatus); // Метод отправит контактам пользователя сообщение об изменении его статуса

    void syncAddedUser(QUuid inContactUuid, Core::UserInfo_Ptr inOwnerInfo); // Метод синхранизирует список контактов после добавления пользователя
    void syncDeletedUser(QUuid inContactUuid, QUuid inOwnerUuid); // Метод синхранизирует список контактов после удаления пользователя

    void syncCreateChat(QUuid inChatUuid); // Метод синхронизирует добавленную беседу с пользователями
    void syncAddedUserToChat(Core::ChatInfo_Ptr inChatUuid, Core::UserInfo_Ptr inUser); // Метод синхронизирует беседу послед добавления пользователя в неё
    void syncDeletedUserFromChat(Core::ChatInfo_Ptr inChat, QUuid inChatUuid); // Метод синхронизирует беседу послед удаления пользователя из неё

signals:
    void sig_LogMessage(QHostAddress inAddres, QString inMessage); // Сигнал пошлёт сообщение
    void sigSocketError();
    void sigTimeOut();

private slots:
    void slot_NewConnection(); // Слот, реагирующий на подключение нового клиента
    void slot_ClientDisconnect(); // Слот, реагирующий на отключение клиента
    void slot_ClientReadData(); // Слот, реагирующий на готовность к чтению данных от клиентов
    void slot_ClientChangeState(QAbstractSocket::SocketState inState); // Слот, реагирующий на изменение состояния клиента
    void slot_ClientError(QAbstractSocket::SocketError inError); // Слот, реагирующий на ошибку клиента

    void slot_SetAuthorizedClient(QTcpSocket* inClient, Core::TUserAccount &inUserAccount); // Слот, задающий авториированного пользователя

    void slot_AddContact(QTcpSocket* inClient, Core::UserInfo_Ptr &inContactInfo); // Слот, добавляющйи контакт пользователю
    void slot_DelContact(QTcpSocket* inClient, QUuid &inContactUuid); // Слот, удаляющий контакт пользователя

    void slot_DeleteUserFromChat(QUuid inChatUuid, QUuid inUserUuid); // Слот, удаляющий пользователя из беседы
    //void slot_DeleteChat(QUuid inChatUuid); // Слот, удаляющий беседу
};

#endif // MSGSERVER_H

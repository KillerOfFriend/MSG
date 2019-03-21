#ifndef MSGSERVER_H
#define MSGSERVER_H

#include <map>
#include <set>
#include <memory>
#include <QHostAddress>

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

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
    TConnectedUsersModel fClients; // Список подключённых клиентов

    //---
    void init(); // Метод инициализирует класс
    void Link(); // Метод соединит сигналы\слоты

    void executCommand(QTcpSocket* inSender); // Метод выполнит команду

    qint32 creteUser(QDataStream &inDataStream); // Метод создаст нового пользователя
    std::pair<qint32, QUuid> canAuthorization(QDataStream &inDataStream); // Метод авторизирует пользователя

    Users::TUserInfo getUserInfo(QUuid inUserUuid); // Метод вернёт инфо пользователя по Uuid
    QList<Users::TUserInfo> findUsers(QDataStream &inDataStream); // Метод вернёт список пользователей по их имени\логину

    std::pair<qint32, Users::TUserInfo> addContact(QDataStream &inDataStream); // Метод добавит котнтакт пользователю
    QList<Users::TUserInfo> getContacts(QDataStream &inDataStream); // Метод вернёт список контактов по uuid указанного пользователя
    QList<Users::TUserInfo> getContacts(const QUuid &inOwnerUuid); // Метод вернёт список контактов указанного пользователя
    std::pair<qint32, QUuid> deleteContact(QDataStream &inDataStream); // Метод удалит котнтакт пользователю

    std::pair<qint32, Users::TChatInfo> createChat(QDataStream &inDataStream); // Метод добавит новую беседу
    QList<QUuid> findChats(QUuid inUserUuid); // Метод вернёт список бесед по uuid указанного пользователя
    qint32 addUserToChat(QUuid inChatUuid, QUuid inUserUuid); // Метод добавит пользователя в беседу
    Users::TChatInfo getChatInfo(QUuid inChatUuid); // Метод вернёт информацию о беседе
    QList<QUuid> getChatUsers(QUuid inChatUuid); // Метод вернёт список пользователй чата
    QList<Users::TChatInfo> getChats(const QUuid &inOwnerUuid); // Метод вернёт список бесед указонного пользователя

    QString ReadStringFromStream(QDataStream &inDataStream); // Метод прочитает строку из потока

    void checkUsersStatus(QList<Users::TUserInfo> &inUsers); // Метод проверит наличие контактов онлайн и установит их статус
    void checkUserStatus(Users::TUserInfo &inUsers); // Метод проверит наличие контакта онлайнт и установит их статус
    void userChangeStatus(QTcpSocket* inClientSender, quint8 inNewStatus); // Метод отправит контактам пользователя сообщение об изменении его статуса

    void syncAddedUser(QUuid inContactUuid, Users::TUserInfo &inOwnerInfo); // Метод синхранизирует список контактов после добавления пользователя
    void syncDeletedUser(QUuid inContactUuid, QUuid inOwnerUuid); // Метод синхранизирует список контактов после удаления пользователя

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

    void slot_SetAuthorizedClient(QTcpSocket* inClient, Users::TUserAccount &inUserAccount); // Слот, задающий авториированного пользователя
    void slot_AddContact(QTcpSocket* inClient, Users::TUserInfo &inContactInfo); // Слот, добавляющйи контакт пользователю
    void slot_DelContact(QTcpSocket* inClient, QUuid &inContactUuid); // Слот, удаляющий контакт пользователя
};

#endif // MSGSERVER_H

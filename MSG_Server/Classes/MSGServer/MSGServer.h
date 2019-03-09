#ifndef MSGSERVER_H
#define MSGSERVER_H

#include <map>
#include <set>
#include <memory>

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include "Classes/ComandExecutor/ComandExecutor.h"
#include "Classes/UserInfo/UserInfo.h"
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

    QAbstractTableModel* clientsModel(); // Метод вернёт модель данных подключённых клиентов

private:    
    void init(); // Метод инициализирует класс
    void Link(); // Метод соединит сигналы\слоты

    std::unique_ptr<QTcpServer> fServer = nullptr; // Сервер
    std::unique_ptr<TComandExecutor> fComandExecutor = nullptr; // Исполнитель команд
    TConnectedUsersModel fClients; // Список подключённых клиентов

signals:
    void sig_LogMessage(QHostAddress inAddres, QString inMessage); // Сигнал пошлёт сообщение

private slots:
    void slot_NewConnection(); // Слот, реагирующий на подключение нового клиента
    void slot_ClientDisconnect(); // Слот, реагирующий на отключение клиента
    void slot_ClientReadData(); // Слот, реагирующий на готовность к чтению данных от клиентов
    void slot_ClientChangeState(QAbstractSocket::SocketState inState); // Слот, реагирующий на изменение состояния клиента
    void slot_ClientError(QAbstractSocket::SocketError inError); // Слот, реагирующий на ошибку клиента

    void slot_SetAutClient(QTcpSocket* inClient, TUserInfo &inUserInfo); // Слот, задающий авториированного пользователя

};

#endif // MSGSERVER_H

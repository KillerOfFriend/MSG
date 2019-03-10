#ifndef MSGCLIENT_H
#define MSGCLIENT_H

#include <memory>

#include <QTcpSocket>
#include <QTimer>

#include "Classes/ComandExecutor/ComandExecutor.h"

class TMSGClient : public QObject
{
    Q_OBJECT
public:
    TMSGClient(QObject* inParent = nullptr);
    ~TMSGClient();

    std::unique_ptr<TComandExecutor> ComandExecutor = nullptr; // Указатель на исполнитель команд

    bool connectToServer();
    void disconnect();
    bool isConnected();

    bool createUser(QString inLogin, QString inPassword); // Метод отправит команду на создание пользователья
    bool authorization(QString inLogin, QString inPassword); // Метод отправит номанду на авторизацию пользователя

    bool findUsers(QString inUserName); // Метод отправит команду на поиск пользователей
    bool addContact(QUuid inSelfUuid, QUuid inContactUuid); // Метод отправит команду на добавление контакта
    bool getContacts(QUuid inSelfUuid); // Метод отправит команду на на возвращение списка контактов

private:
    std::unique_ptr<QTcpSocket> fClient = nullptr;

    void init();
    void Link();

private slots: 
    void slot_ReadyRead();

    void slot_hostFound();
    void slot_connected();
    void slot_disconnected();

signals:
    void sig_TimeOut(qint32 inResult);
};

#endif // MSGCLIENT_H

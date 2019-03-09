#include "MSGServer.h"
#include <QFile>
#include <QDebug>
#include <QSettings>
#include <QApplication>

#include "Classes/DataModule/DataModule.h"

//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::TMSGServer - Конструктор по умолчанию
 * @param inParent - Предок типа QObject
 */
TMSGServer::TMSGServer(QObject *inParent) : QObject(inParent)
{
    init();
    Link();
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::~TMSGServer - Диструктор по умолчанию
 */
TMSGServer::~TMSGServer()
{
    disconnectAll();
    fServer->close();

    qDebug() << "[ИНФО] TCP Сервер отключен.";
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::isActive - Метод вернёт флаг активности сервера
 * @return Вернёт флаг активности
 */
bool TMSGServer::isActive()
{
    return fServer->isListening();
}
//-----------------------------------------------------------------------------
/**
 * @brief disconnectClient - Метод отключит клиента
 * @param Client - Откулючаемый клиент
 */
void TMSGServer::disconnectClient(QTcpSocket* inClient)
{
    if (inClient) // Если клиент валиден
    {
        auto It = fClients.find(inClient); // Ищим клиента в контейнере
        inClient->disconnect(); // Отключаем

        if (It != fClients.end()) // Если найден
            fClients.erase(It); // Удаляем
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::disconnectAll - Метод отключит всех клиентов
 */
void TMSGServer::disconnectAll()
{
    std::for_each(fClients.begin(), fClients.end(), [&](const std::pair<QTcpSocket*, TUserInfo> &Client)
    {
        disconnectClient(Client.first);
    });
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::clientsModel - Метод вернёт модель данных подключённых клиентов
 * @return Вернёт табличную модель данных клиентов
 */
QAbstractTableModel* TMSGServer::clientsModel()
{ return &fClients; }
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::init - Метод инициализирует класс
 */
void TMSGServer::init()
{
    fServer.reset(new QTcpServer(this));
    fComandExecutor.reset(new TComandExecutor(this));

    if (!QFile::exists(QApplication::applicationDirPath() + "/Settings.ini"))
    {
        QFile SettingsFile(QApplication::applicationDirPath() + "/Settings.ini");
        SettingsFile.open(QIODevice::WriteOnly);
        SettingsFile.close();

        init();
    }
    else
    {
        QSettings Settings(QApplication::applicationDirPath() + "/Settings.ini", QSettings::IniFormat);

        if (!Settings.contains("SERVER/Port"))
            Settings.setValue("SERVER/Port", "7876");

        if(!fServer->listen(QHostAddress::Any, Settings.value("SERVER/Port", "7876").toInt()))
            qDebug() << "[ОШИБКА] Не удалось запустить TCP сервер!";
        else
            qDebug() << "[УСПЕХ] TCP сервер запущен.";

        Settings.sync();
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::Link - Метод соединит сигналы\слоты
 */
void TMSGServer::Link()
{
    connect(fServer.get(), &QTcpServer::newConnection, this, &TMSGServer::slot_NewConnection);
    connect(fComandExecutor.get(), &TComandExecutor::sig_LogMessage, this, &TMSGServer::sig_LogMessage);
    connect(fComandExecutor.get(), &TComandExecutor::sigSetUserInfo, this, &TMSGServer::slot_SetAutClient);
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::slot_NewConnection - Слот, реагирующий на подключение нового клиента
 */
void TMSGServer::slot_NewConnection()
{
    QTcpSocket* NewConnection = fServer->nextPendingConnection();
    TUserInfo NewClient(this);
    NewClient.setUserType(0);
    NewClient.setUserUuid(QUuid());
    NewClient.setUserLogin("Anonimus");
    NewClient.setUserName("Anonimus");

    auto InsertRes = fClients.insert(std::make_pair(NewConnection, NewClient));  // Получаем сокет подключаемого клиента

    if (!InsertRes.second)
    {
        sig_LogMessage(NewConnection->localAddress(), "Ошибка подключения к серверу");
        NewConnection->disconnect();
    }
    else
    {
        connect(NewConnection, &QTcpSocket::readyRead, this, &TMSGServer::slot_ClientReadData);
        connect(NewConnection, &QTcpSocket::disconnected, this, &TMSGServer::slot_ClientDisconnect);
        connect(NewConnection, &QAbstractSocket::stateChanged, this, &TMSGServer::slot_ClientChangeState);
        connect(NewConnection, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &TMSGServer::slot_ClientError);

        sig_LogMessage(NewConnection->localAddress(), "Новое подключение");
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::slot_ClientDisconnect - Слот, реагирующий на отключение клиента
 */
void TMSGServer::slot_ClientDisconnect()
{
    QTcpSocket* Client = qobject_cast<QTcpSocket*>(QObject::sender());

    if (Client)
    {
        auto FindRes = fClients.find(Client); // Ищим клиента среди авторизированных пользователей
        if (FindRes != fClients.end()) // Если находим
            fClients.erase(FindRes); // Удаляем

        sig_LogMessage(Client->localAddress(), "Клиент отключён");
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::slot_ClientReadData - Слот, реагирующий на готовность к чтению данных от клиентов
 */
void TMSGServer::slot_ClientReadData()
{
    QTcpSocket* Client = qobject_cast<QTcpSocket*>(QObject::sender());

    if (!Client)
        sig_LogMessage(Client->localAddress(), "Ошибка получения сокета!");
    else
    {
        if (!fComandExecutor)
            sig_LogMessage(Client->localAddress(), "Невозможно выполнить команду");
        else
        {
            fComandExecutor->executCommand(Client);
        }
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::slot_ClientChangeState - Слот, реагирующий на изменение состояния клиента
 * @param inState - Новое состояние
 */
void TMSGServer::slot_ClientChangeState(QAbstractSocket::SocketState inState)
{
    QTcpSocket* Client = qobject_cast<QTcpSocket*>(QObject::sender());
    if (!Client)
        return;

    QString Message = "Статус изменён на: ";

    switch (inState)
    {
        case QAbstractSocket::UnconnectedState: { Message += "ОТСОЕДЕНЁН"; break; }
        case QAbstractSocket::HostLookupState:  { Message += "ОБРАЩЕНИЕ К ХОСТУ"; break; }
        case QAbstractSocket::ConnectingState:  { Message += "ПОДКЛЮЧЕН"; break; }
        case QAbstractSocket::ConnectedState:   { Message += "ПОДКЛЮЧАЕТСЯ"; break; }
        case QAbstractSocket::BoundState:       { Message += "СВЯЗАН"; break; }
        case QAbstractSocket::ListeningState:   { Message += "ПРОСЛУШИВАНИЕ"; break; }
        case QAbstractSocket::ClosingState:     { Message += "ЗАКРЫТ"; break; }
        default : Message += "НЕИЗВЕСТНЫЙ";
    }

    sig_LogMessage(Client->localAddress(), Message);
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::slot_ClientError - Слот, реагирующий на ошибку клиента
 * @param inError - Код ошибки
 */
void TMSGServer::slot_ClientError(QAbstractSocket::SocketError inError)
{
    QTcpSocket* Client = qobject_cast<QTcpSocket*>(QObject::sender());
    if (!Client)
        return;

    sig_LogMessage(Client->localAddress(), "Ошибка: " + QString::number(inError));
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::slot_SetAutClient - Слот, задающий авториированного пользователя
 * @param Client - Сокет клиента
 * @param inUserInfo - Данные клиента
 */
void TMSGServer::slot_SetAutClient(QTcpSocket* inClient, TUserInfo &inUserInfo)
{
    auto It = fClients.find(inClient); // Ищим сокет

    if (It != fClients.end()) // Если сокет найлен
    {
        qint32 Row = std::distance(fClients.begin(), It); // Вычисляем номер изменяемой строки
        It->second = inUserInfo; // Задаём личные данные пользователя
        fClients.slot_UpdateRow(Row); // Вызываем обновление данных пользоваетля
    }
}
//-----------------------------------------------------------------------------

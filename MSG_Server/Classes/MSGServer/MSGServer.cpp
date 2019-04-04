#include "MSGServer.h"
#include <QFile>
#include <QDebug>
#include <QSettings>
#include <QApplication>

#include "Classes/DataModule/DataModule.h"
#include "Classes/MessageHeadline/MessageHeadline.h"
#include "comandes.h"
#include "resultcodes.h"

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
    std::for_each(fClients.begin(), fClients.end(), [&](const std::pair<QTcpSocket*, Core::TUserAccount> &Client)
    {
        disconnectClient(Client.first);
    });
}
//-----------------------------------------------------------------------------
/**
 * @brief serverAddress - Метод вернёт адрес сервера
 * @return Вернёт адрес сервера
 */
QHostAddress TMSGServer::serverAddress() const
{
    if (!fServer)
        return QHostAddress();
    else
        return fServer->serverAddress();
}
//-----------------------------------------------------------------------------
/**
 * @brief serverPort - Метод вернёт порт сервера
 * @return Вернёт порт сервера
 */
quint16 TMSGServer::serverPort() const
{
    if (!fServer)
        return 0;
    else
        return fServer->serverPort();
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
    fServerCache.reset(new TServerCache(this));

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
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::socketByUuid - Метод вернёт сокет клиента если он онлайн
 * @param inClientUuid - Uuid клиента
 * @return Вернёт сокет или нулевой указаетль
 */
QTcpSocket* TMSGServer::socketByUuid(QUuid inClientUuid)
{
    auto FindRes = std::find_if(fClients.begin(), fClients.end(), [&inClientUuid](const std::pair<QTcpSocket*, Core::TUserAccount> &ClientOnline)
    {
        return ClientOnline.second.userInfo()->userUuid() == inClientUuid;
    });

    if (FindRes != fClients.end())
        return FindRes->first;
    else return nullptr;
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::checkUsersStatus - Метод проверит наличие контактов и онлайн и установит их статус
 * @param inContacts - Список проверяемых пользователей
 */
void TMSGServer::checkUsersStatus(QList<Core::UserInfo_Ptr> &inUsers)
{
    std::for_each(inUsers.begin(), inUsers.end(), [&](Core::UserInfo_Ptr &UserInfo) // Перебираем все контакты полученного клиента
    {
        checkUserStatus(UserInfo);
    });
}
//-----------------------------------------------------------------------------
void TMSGServer::checkUserStatus(Core::UserInfo_Ptr inUsers) // Метод проверит наличие контакта онлайнт и установит их статус
{
    auto FindRes = std::find_if(fClients.begin(), fClients.end(), [&inUsers](const std::pair<QTcpSocket*, Core::TUserAccount> &Client) // Ищим контакт в списке подключённых клиентов
    {
        return Client.second.userInfo()->userUuid() == inUsers->userUuid(); // Сравнение по Uuid
    });

    if (FindRes != fClients.end()) // Если клиент онлайн
        inUsers->setUserStatus(Core::UserStatus::usOnline);
    else inUsers->setUserStatus(Core::UserStatus::usOffline);
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::userChangeStatus - Метод отправит контактам пользователя сообщение об изменении его статуса
 * @param inSender - Сокет клиента
 * @param inNewStatus - Новый статус
 */
void TMSGServer::userChangeStatus(QTcpSocket* inClientSender, quint8 inNewStatus)
{
    auto SenderFindRes = fClients.find(inClientSender); // Ищим пользователя (Меняющего статус) в списке подключённых клиентов

    if (SenderFindRes != fClients.end()) // Если найден
    {
        sig_LogMessage(inClientSender->peerAddress(), "Отправка данных о смене своего состояния");

        std::for_each(SenderFindRes->second.contacts()->begin(), SenderFindRes->second.contacts()->end(), [&](const std::pair<QUuid, Core::UserInfo_Ptr> &Contact) // Перебираем все контакты пользователя
        {
            auto ContactFindRes = std::find_if(fClients.begin(), fClients.end(), [&](const std::pair<QTcpSocket*, Core::TUserAccount> &Client) // Ищим контакт в списке подключённых клиентов
            {
                return Client.second.userInfo()->userUuid() == Contact.second->userUuid(); // Сравнение по Uuid
            });

            if (ContactFindRes != fClients.end()) // Если клиент онлайн
            {
                QByteArray StatusData; // Отправляемый статус пользователя
                QDataStream outStream(&StatusData, QIODevice::WriteOnly);
                // Отправляем команду "Статус контакта изменён" + Uuid этого контакта + новый статус
                outStream << Commands::ContactChangeStatus << SenderFindRes->second.userInfo()->userUuid()  << inNewStatus;

                ContactFindRes->first->write(StatusData); // Отправляем данные
            }
        });
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::slot_NewConnection - Слот, реагирующий на подключение нового клиента
 */
void TMSGServer::slot_NewConnection()
{
    QTcpSocket* NewConnection = fServer->nextPendingConnection();
    Core::TUserAccount NewAccount(this);
    Core::UserInfo_Ptr AnonimusInfo = std::make_shared<Core::TUserInfo>(this);

    AnonimusInfo->setUserType(0);
    AnonimusInfo->setUserUuid(QUuid());
    AnonimusInfo->setUserLogin("Anonimus");
    AnonimusInfo->setUserName("Anonimus");
    NewAccount.slot_SetUserInfo(AnonimusInfo);

    auto InsertRes = fClients.insert(std::make_pair(NewConnection, NewAccount));  // Получаем сокет подключаемого клиента

    if (!InsertRes.second)
    {
        sig_LogMessage(NewConnection->peerAddress(), "Ошибка подключения к серверу");
        NewConnection->disconnect();
    }
    else
    {
        connect(NewConnection, &QTcpSocket::readyRead, this, &TMSGServer::slot_ClientReadData);
        connect(NewConnection, &QTcpSocket::disconnected, this, &TMSGServer::slot_ClientDisconnect);
        connect(NewConnection, &QAbstractSocket::stateChanged, this, &TMSGServer::slot_ClientChangeState);
        connect(NewConnection, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &TMSGServer::slot_ClientError);

        sig_LogMessage(NewConnection->peerAddress(), "Новое подключение");
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
        userChangeStatus(Client, Core::UserStatus::usOffline); // Сообщаем об изменении статуса клиента на "Offline"

        auto FindRes = fClients.find(Client); // Ищим клиента среди авторизированных пользователей
        if (FindRes != fClients.end()) // Если находим
            fClients.erase(FindRes); // Удаляем

        sig_LogMessage(Client->peerAddress(), "Клиент отключён");
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
        sig_LogMessage(Client->peerAddress(), "Ошибка получения сокета!");
    else
    {
        executCommand(Client);
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

    sig_LogMessage(Client->peerAddress(), Message);
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

    sig_LogMessage(Client->peerAddress(), "Ошибка: " + QString::number(inError));
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::slot_SetAutClient - Слот, задающий авториированного пользователя
 * @param Client - Сокет клиента
 * @param inUserInfo - Данные клиента
 */
void TMSGServer::slot_SetAuthorizedClient(QTcpSocket* inClient, Core::TUserAccount &inUserAccount)
{
    auto It = fClients.find(inClient); // Ищим сокет

    if (It != fClients.end()) // Если сокет найден
    {
        inUserAccount.userInfo()->setUserStatus(Core::UserStatus::usOnline); // Задаём статус "Онлайн"
        It->second = inUserAccount; // Задаём личные данные пользователя
        qint32 Row = std::distance(fClients.begin(), It); // Вычисляем номер изменяемой строки
        fClients.slot_UpdateRow(Row); // Вызываем обновление данных пользоваетля

        // Нужно обеспечить доступ к беседе пользователя
        std::for_each(inUserAccount.chats()->begin(), inUserAccount.chats()->end(), [&](const std::pair<QUuid, Core::ChatInfo_Ptr> &Chat)
        {
            auto FindChatRes = fChats.find(Chat.first); // Ищим данную беседу в списке отслеживаемых бесед
            if (FindChatRes == fChats.end()) // Если данной беседы нет в списке отслеживаемых бесед
                fChats.insert(Chat); // Добавляем
        });

        userChangeStatus(inClient, Core::UserStatus::usOnline); // Сообщаем об изменении статуса клиента на "Online"
    }
    else // Если сокет не найден (НЕ ДОЛЖНО ТАКОГО БЫТЬ)
    {
        fClients.insert(std::make_pair(inClient, inUserAccount)); // Добавляем новый аккаунт
        qDebug() << "Некорректно добавленый пользователь " + inUserAccount.userInfo()->userLogin();
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::slot_AddContact - Слот, добавляющйи контакт пользователю
 * @param inClient - Сокет клиента
 * @param inContactInfo - Данные добавляемого контакта
 */
void TMSGServer::slot_AddContact(QTcpSocket* inClient, Core::UserInfo_Ptr &inContactInfo)
{
    auto It = fClients.find(inClient); // Ищим сокет

    if (It != fClients.end()) // Если сокет найден
    {
        It->second.contacts()->insert(std::make_pair(inContactInfo->userUuid(), inContactInfo)); // Добавляем контакт
        qint32 Row = std::distance(fClients.begin(), It); // Вычисляем номер изменяемой строки
        fClients.slot_UpdateRow(Row); // Вызываем обновление данных пользоваетля
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::slot_DelContact - Слот, добавляющйи контакт пользователю
 * @param inClient - Сокет клиента
 * @param inContactUuid - Uuid удаляемого контакта
 */
void TMSGServer::slot_DelContact(QTcpSocket* inClient, QUuid &inContactUuid)
{
    auto It = fClients.find(inClient); // Ищим сокет

    if (It != fClients.end()) // Если сокет найден
    {
        auto FindRes = It->second.contacts()->find(inContactUuid); // Ищим этот контакт

        if (FindRes != It->second.contacts()->end())
        {
            It->second.contacts()->erase(FindRes); // Удаляем пользователя
            qint32 Row = std::distance(fClients.begin(), It); // Вычисляем номер изменяемой строки
            fClients.slot_UpdateRow(Row); // Вызываем обновление данных пользоваетля
        }
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::slot_DeleteUserFromChat - Слот, удаляющий пользователя из беседы
 * @param inChatUuid - Uuid беседы
 * @param inUserUuid - Uuid удаляемого пользователя
 */
void TMSGServer::slot_DeleteUserFromChat(QUuid inChatUuid, QUuid inUserUuid)
{
    Core::ChatInfo_Ptr Chat = fServerCache->getChatInfo(inChatUuid); // Ищим беседу

    if (Chat) // Беседа найдена
    {
        auto FindRes = Chat->clients()->find(inUserUuid); // Ищим удаляемого пользователя
        if (FindRes != Chat->clients()->end()) // Пользоваетль беседы найден
        {   // ВАЖНО! Сначала запускаем синхронизацию удаления, а только посл выполняем удаление само удаление (иначе синхранизация с удалеяемым пользователем не пройдет!)
            syncDeletedUserFromChat(Chat, inUserUuid); // Синхронизируем удаление пользователя из беседы
            Chat->clients()->erase(FindRes); // Удаляем пользователя из беседы
        }
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief slot_DeleteChat - Слот, удаляющий беседу
 * @param inChatUuid - Uuid удаляемой беседы
 */
void TMSGServer::slot_DeleteChat(QUuid inChatUuid)
{
    //deleteChat(inChatUuid); // Вызываем удалеие беседы
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::syncAddedUser - Метод синхранизирует список контактов после добавления пользователя
 * @param inContactUuid - Uuid пользователя, которого нужно синхронизировать
 * @param inOwnerInfo - Данные пользователя, добавившего контакт
 */
void TMSGServer::syncAddedUser(QUuid inContactUuid, Core::UserInfo_Ptr inOwnerInfo)
{
    if (!inOwnerInfo)
        return;

    // Ищим контакт в списке подключённых клиентов
    auto ContactIt = std::find_if(fClients.begin(), fClients.end(), [&](const std::pair<QTcpSocket*, Core::TUserAccount> &Item)
    {
        return Item.second.userInfo()->userUuid() == inContactUuid;
    });

    if (ContactIt != fClients.end()) // Если контакт найден
    {
        QByteArray Data;
        QDataStream outStream(&Data, QIODevice::WriteOnly); // Создаём выходной поток

        // Шлём команду на добавление пользоваеля вместе с информацией о добовившем (добавляемом) юзере
        outStream << Commands::AddContact << Res::AddContact::acCreated << *inOwnerInfo;
        ContactIt->first->write(Data);
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::syncDeletedUser - Метод синхранизирует список контактов после удаления пользователя
 * @param inContactUuid - Uuid пользователя, которого нужно синхронизировать
 * @param inOwnerUuid - Uuid пользователя, удалившего контакт
 */
void TMSGServer::syncDeletedUser(QUuid inContactUuid, QUuid inOwnerUuid)
{
    // Ищим контакт в списке подключённых клиентов
    auto ContactIt = std::find_if(fClients.begin(), fClients.end(), [&](const std::pair<QTcpSocket*, Core::TUserAccount> &Item)
    {
        return Item.second.userInfo()->userUuid() == inContactUuid;
    });

    if (ContactIt != fClients.end()) // Если контакт найден
    {
        QByteArray Data;
        QDataStream outStream(&Data, QIODevice::WriteOnly); // Создаём выходной поток

        // Шлём команду на удаление пользоваеля вместе с информацией о удалившем (удаляемом) юзере
        outStream << Commands::DeleteContact << Res::DeleteContact::dcContactRemove << inOwnerUuid;
        ContactIt->first->write(Data);
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::syncCreateChat - Метод синхронизирует добавленную беседу с пользователями
 * @param inChatUuid - QUuid беседы
 */
void TMSGServer::syncCreateChat(QUuid inChatUuid)
{
    Core::ChatInfo_Ptr Chat = fServerCache->getChatInfo(inChatUuid); // Ищим беседу

    if (Chat)
    {
        std::for_each(Chat->clients()->begin(), Chat->clients()->end(), [&](const std::pair<QUuid, Core::UserInfo_Ptr> &ChatClient)
        {
            syncAddedUserToChat(Chat, ChatClient.second); // Запускаем синхронизацию
        });
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::syncAddedUserToChat - Метод синхронизирует беседу послед добавления пользователя в неё
 * @param inChat - Беседа, в которую был добавлен пользователь
 * @param inUser - Клиента беседы
 */
void TMSGServer::syncAddedUserToChat(Core::ChatInfo_Ptr inChatUuid, Core::UserInfo_Ptr inUser)
{
    if (!inChatUuid || !inUser)
        return;

    if (inUser->userStatus() == Core::UserStatus::usOnline ) // Если юзер и беседа валидны, а так же юзер онлайн
    {
        QTcpSocket* ClientSocket = socketByUuid(inUser->userUuid()); // Получаем сокет клиента

        if (ClientSocket) // Если сокет получен
        {
            QByteArray Data;
            QDataStream outStream(&Data, QIODevice::WriteOnly);

            outStream << Commands::InviteToChat << *inChatUuid; // Шлём команду на добавление + беседу

            ClientSocket->write(Data);
        }
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TMSGServer::syncDeletedUserFromChat - Метод синхронизирует беседу послед удаления пользователя из неё
 * @param inChat - Беседа, из которой был удалён пользователь
 * @param inUserUuid - Uuid пользователя
 */
void TMSGServer::syncDeletedUserFromChat(Core::ChatInfo_Ptr inChat, QUuid inUserUuid)
{
    if (!inChat)
        return;

    std::for_each(inChat->clients()->begin(), inChat->clients()->end(), [&](const std::pair<QUuid, Core::UserInfo_Ptr> &Client) // Перебираем всех клиентов беседы
    {
        QTcpSocket* ClientSocket = socketByUuid(Client.second->userUuid()); // Получаем сокет клиента беседы

        if(ClientSocket) // Если сокет получен (клиент онлайн)
        {
            QByteArray Data;
            QDataStream outStream(&Data, QIODevice::WriteOnly);

            outStream << Commands::DeleteUserFromChat << Res::DeleteUserFromChat::dufcSuccess << inChat->chatUuid() << inUserUuid; // Шлём команду на удаление + uuid беседы и uuid удаляемого пользователя

            ClientSocket->write(Data);
        }
    });
}
//-----------------------------------------------------------------------------

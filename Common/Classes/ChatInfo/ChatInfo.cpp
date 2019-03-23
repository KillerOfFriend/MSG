#include "ChatInfo.h"

#include <QList>

using namespace Users;

//-----------------------------------------------------------------------------
TChatInfo::TChatInfo(QObject *inParent) : QObject(inParent)
{

}
//-----------------------------------------------------------------------------
TChatInfo::TChatInfo(const TChatInfo &inOther) : QObject(inOther.parent())
{
    this->fUuid = inOther.fUuid; // Копируем Uuid беседы
    this->fName = inOther.fName; // Копируем имя беседы
    this->fPrivateStatus = inOther.fPrivateStatus; // Копируем статус приватности беседы
    this->fClients = inOther.fClients; // Копируем список клиентов беседы
}
//-----------------------------------------------------------------------------
TChatInfo::~TChatInfo()
{
    fName.clear();
    fClients.clear();
}
//-----------------------------------------------------------------------------
TChatInfo& TChatInfo::operator =(const TChatInfo &inOther)
{
    if (this == &inOther)
         return *this;

    this->fUuid = inOther.fUuid; // Копируем Uuid беседы
    this->fName = inOther.fName; // Копируем имя беседы
    this->fPrivateStatus = inOther.fPrivateStatus; // Копируем статус приватности беседы
    this->fClients = inOther.fClients; // Копируем список клиентов беседы

    this->setParent(inOther.parent());

    return *this;
}
//-----------------------------------------------------------------------------
void TChatInfo::setChatUuid(QUuid inUuid) // Метод задаст UUid беседы
{ fUuid = inUuid; }
//-----------------------------------------------------------------------------
QUuid TChatInfo::chatUuid() const // Метод вернёт Uuid беседы
{ return fUuid; }
//-----------------------------------------------------------------------------
void TChatInfo::setChatName(QString inName) // Метод задаст имя беседы
{ fName = inName; }
//-----------------------------------------------------------------------------
QString TChatInfo::chatName() const // Метод вернёт имя беседы
{ return fName; }
//-----------------------------------------------------------------------------
void TChatInfo::setChatPrivateStatus(bool inPrivateStatus) // Метод задаст статус приватности беседы
{ fPrivateStatus = inPrivateStatus; }
//-----------------------------------------------------------------------------
bool TChatInfo::chatPrivateStatus() const // Метод вернёт статус приватности беседы
{ return fPrivateStatus; }
//-----------------------------------------------------------------------------
std::size_t TChatInfo::usersCount() // Метод вернёт кол-во пользователей
{ return fClients.size(); }
//-----------------------------------------------------------------------------
void TChatInfo::addUser(QUuid inUserUuid) // Метод добавит пользователя в беседу
{
    auto InsertRes = fClients.insert(inUserUuid); // Добавляем пользователя в беседу
    if (InsertRes.second) // Если пользователя успешно добавлен
        sig_ChatUserAdded(fUuid, inUserUuid); // Шлём сигнал об добавлении пользователя в беседу
}
//-----------------------------------------------------------------------------
void TChatInfo::deleteUser(QUuid inUserUuid) // Метод удалит пользователя из беседы
{
    auto FindRes = fClients.find(inUserUuid); // Ищим удаляемого польователя

    if(FindRes != fClients.end()) // Если такой пользоваетль в беседе есть
    {
        fClients.erase(FindRes); // Удаляем его из контейнера
        sig_ChatUserDeleted(fUuid, inUserUuid); // Шлём сигнал об удалении пользователя
        if (fClients.empty()) // Если контейнер пуст
            sig_ChatIsEmpty(fUuid); // Шлём сигнал о том, что беседа опустела
    }
}
//-----------------------------------------------------------------------------
QUuid TChatInfo::user(std::size_t inIndex) // Метод вернёт Uuid пользователя по индексу
{
    if (inIndex >= fClients.size())
        return QUuid();
    else
    {
        auto It = fClients.begin();
        std::advance(It, inIndex);
        return *It;
    }
}
//-----------------------------------------------------------------------------
void TChatInfo::slot_SetClients(QList<QUuid> &inClientList) // Слот задаст список клиентов беседы
{
    fClients.clear(); // Предварительно чистим содержимое контейнера клиентов

    std::for_each(inClientList.begin(), inClientList.end(), [&](const QUuid &UserUuid) // Копируем пользователей
    {
        fClients.insert(UserUuid);
    });
}
//-----------------------------------------------------------------------------
namespace Users
{   // Во избежании затупов со стороны компиллера, требуется оборачивать реализацию в тот же неймспейс принудительно
    QDataStream& operator <<(QDataStream &outStream, const TChatInfo &ChatInfo)
    {
        outStream << ChatInfo.fUuid;
        outStream << ChatInfo.fName.toUtf8();
        outStream << ChatInfo.fPrivateStatus;

        QList<QUuid> ClientsBuf; // Создаём временный буфер пользователей
        std::for_each(ChatInfo.fClients.begin(), ChatInfo.fClients.end(), [&ClientsBuf](const QUuid &UserUuid) // Копируем пользователей
        {
            ClientsBuf.push_back(UserUuid);
        });
        outStream << ClientsBuf;

        return outStream;
    }
    //-----------------------------------------------------------------------------
    QDataStream& operator >>(QDataStream &inStream, TChatInfo &ChatInfo)
    {
        inStream >> ChatInfo.fUuid;

        QByteArray Buff;
        inStream >> Buff;
        ChatInfo.fName = QString::fromUtf8(Buff);

        inStream >> ChatInfo.fPrivateStatus;

        ChatInfo.fClients.clear(); // Предварительно чистим содержимое контейнера клиентов
        QList<QUuid> ClientsBuf; // Создаём временный буфер пользователей
        inStream >> ClientsBuf;
        std::for_each(ClientsBuf.begin(), ClientsBuf.end(), [&](const QUuid &UserUuid) // Копируем пользователей
        {
            ChatInfo.fClients.insert(UserUuid);
        });

        return inStream;
    }
    //-----------------------------------------------------------------------------
}

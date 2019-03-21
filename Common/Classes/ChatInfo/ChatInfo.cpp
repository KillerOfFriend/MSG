#include "ChatInfo.h"

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

#include "UserAccount.h"

#include "Classes/DataModule/DataModule.h"

using namespace Users;

//-----------------------------------------------------------------------------
TUserAccount::TUserAccount(QObject *inParent) : QObject(inParent)
{
    fUserInfo = std::make_shared<TUserInfo>();
    fContacts = std::make_shared<TUsersModel>();
    fChats = std::make_shared<TChatsModel>();
}
//-----------------------------------------------------------------------------
TUserAccount::TUserAccount(const TUserAccount &inOther) : QObject(inOther.parent())
{
    this->fUserInfo = inOther.fUserInfo; // Копируем инфо пользователя
    this->fContacts = inOther.fContacts; // Копируем контейнер контактов
    this->fChats = inOther.fChats; // Копируем контейнер бесед
}
//-----------------------------------------------------------------------------
TUserAccount::~TUserAccount()
{
    fContacts->clear(); // Очищаем список контактов
    fChats->clear(); // Очищаем список бесед
}
//-----------------------------------------------------------------------------
TUserAccount& TUserAccount::operator = (const TUserAccount &inOther)
{
    if (this == &inOther)
         return *this;

    this->fUserInfo = inOther.fUserInfo; // Копируем инфо пользователя
    this->fContacts = inOther.fContacts; // Копируем контейнер контактов
    this->fChats = inOther.fChats; // Копируем контейнер бесед

    this->setParent(inOther.parent());

    return *this;
}
//-----------------------------------------------------------------------------
/**
 * @brief TUserAccount::userInfo - Метод вернёт информацию о пользователе
 * @return Вернёт указатель на данные пользователя
 */
std::shared_ptr<TUserInfo> TUserAccount::userInfo() const
{ return fUserInfo; }
//-----------------------------------------------------------------------------
/**
 * @brief TUserAccount::contacts - Метод вернёт список контактов
 * @return Вернёт указатель на список контактов
 */
std::shared_ptr<TUsersModel> TUserAccount::contacts() const
{ return fContacts; }
//-----------------------------------------------------------------------------
/**
 * @brief TUserAccount::chats - Метод вернёт список бесед
 * @return Вернёт указатель на список бесед
 */
std::shared_ptr<TChatsModel> TUserAccount::chats() const
{ return fChats; }
//-----------------------------------------------------------------------------
/**
 * @brief TUserAccount::slot_SetUserInfo - Слот задаст данные пользователя
 * @param inUserInfo - Ссылка на данные пользователя
 */
void TUserAccount::slot_SetUserInfo(const TUserInfo &inUserInfo)
{
    fUserInfo = std::make_shared<TUserInfo>(inUserInfo);
    fContacts->clear();
}
//-----------------------------------------------------------------------------
/**
 * @brief TUserAccount::slot_SetContacts - Слот задаст список контактов
 * @param inContacts - Ссылка на список контактов
 */
void TUserAccount::slot_SetContacts(const QList<TUserInfo> &inContacts)
{
    fContacts->clear();

    std::for_each(inContacts.begin(), inContacts.end(), [&](const TUserInfo &Info)
    {
        auto InsertRes = fContacts->insert(std::make_pair(Info.userUuid(), Info));
        if (!InsertRes.second)
            qDebug() << "Не удалось вставить контакт: " + Info.userLogin();
    });
}
//-----------------------------------------------------------------------------
/**
 * @brief TUserAccount::slot_ContactChangeStatus - Слот установит контакту указанный статус
 * @param inContactUuid - Uuid контакта
 * @param inNewStatus - Новый статус
 */
void TUserAccount::slot_ContactChangeStatus(QUuid inContactUuid, quint8 inNewStatus)
{
    auto FindRes = fContacts->find(inContactUuid);

    if (FindRes != fContacts->end())
    {
        auto Row = std::distance(fContacts->begin(), FindRes);

        FindRes->second.setUserStatus(inNewStatus);
        fContacts->dataChanged(fContacts->index(Row, TUsersModel::cUserStatus), fContacts->index(Row, TUsersModel::cUserStatus));
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TUserAccount::slot_SetChats - Слот задаст список бесед
 * @param inChats - Список бесед
 */
void TUserAccount::slot_SetChats(const QList<TChatInfo> &inChats)
{
    fChats->clear();

    std::for_each(inChats.begin(), inChats.end(), [&](const TChatInfo &Info)
    {
        auto InsertRes = fChats->insert(std::make_pair(Info.chatUuid(), std::make_shared<Users::TChatInfo>(Info)));
        if (!InsertRes.second)
            qDebug() << "Не удалось вставить беседу: " + Info.chatName();
    });
}
//-----------------------------------------------------------------------------
/**
 * @brief TUserAccount::slot_AddChat - Слот добавит беседу
 * @param inChat - Новая беседа
 */
void TUserAccount::slot_AddChat(const Users::TChatInfo &inChat)
{
    auto FindRes = fChats->find(inChat.chatUuid());

    if (FindRes == fChats->end())
    {
        fChats->insert(std::make_pair(inChat.chatUuid(), std::make_shared<Users::TChatInfo>(inChat)));
    }
    else
    {
        FindRes->second = std::make_shared<Users::TChatInfo>(inChat);
    }
}
//-----------------------------------------------------------------------------

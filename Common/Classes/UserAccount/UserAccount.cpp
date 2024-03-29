#include "UserAccount.h"

#include "Classes/DataModule/DataModule.h"

using namespace Core;

//-----------------------------------------------------------------------------
TUserAccount::TUserAccount(QObject *inParent) : TConnection(inParent)
{
    fUserInfo = std::make_shared<TUserInfo>();
    fContacts = std::make_shared<TUsersModel>();
    fChats = std::make_shared<TChatsModel>();
}
//-----------------------------------------------------------------------------
TUserAccount::TUserAccount(const TUserAccount &inOther) : TConnection(inOther)
{
    this->fUserInfo = inOther.fUserInfo; // Копируем инфо пользователя
    // Копируем контейнер контактов
    this->fContacts = std::make_shared<TUsersModel>();
    std::for_each(inOther.fContacts->begin(), inOther.fContacts->end(), [&](const std::pair<QUuid, Core::UserInfo_Ptr> &Contact)
    { this->fContacts->insert(Contact); });
    // Копируем контейнер контактов
    this->fChats = std::make_shared<TChatsModel>();
    std::for_each(inOther.fChats->begin(), inOther.fChats->end(), [&](const std::pair<QUuid, Core::ChatInfo_Ptr> &Chat)
    { this->fChats->insert(Chat); });
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
    // Копируем контейнер контактов
    this->fContacts = std::make_shared<TUsersModel>();
    std::for_each(inOther.fContacts->begin(), inOther.fContacts->end(), [&](const std::pair<QUuid, Core::UserInfo_Ptr> &Contact)
    { this->fContacts->insert(Contact); });
    // Копируем контейнер контактов
    this->fChats = std::make_shared<TChatsModel>();
    std::for_each(inOther.fChats->begin(), inOther.fChats->end(), [&](const std::pair<QUuid, Core::ChatInfo_Ptr> &Chat)
    { this->fChats->insert(Chat); });

    this->setParent(inOther.parent());

    return *this;
}
//-----------------------------------------------------------------------------
/**
 * @brief TUserAccount::userInfo - Метод вернёт информацию о пользователе
 * @return Вернёт указатель на данные пользователя
 */
UserInfo_Ptr TUserAccount::userInfo() const
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
void TUserAccount::slot_SetUserInfo(const UserInfo_Ptr &inUserInfo)
{
    fContacts->clear();
    fUserInfo = inUserInfo;
}
//-----------------------------------------------------------------------------
/**
 * @brief TUserAccount::slot_SetContacts - Слот задаст список контактов
 * @param inContacts - Ссылка на список контактов
 */
void TUserAccount::slot_SetContacts(const QList<UserInfo_Ptr> &inContacts)
{
    fContacts->clear();

    std::for_each(inContacts.begin(), inContacts.end(), [&](const UserInfo_Ptr Info)
    {
        auto InsertRes = fContacts->insert(std::make_pair(Info->userUuid(), Info));
        if (!InsertRes.second)
            qDebug() << "Не удалось вставить контакт: " + Info->userLogin();
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

        FindRes->second->setUserStatus(inNewStatus);
        fContacts->dataChanged(fContacts->index(Row, TUsersModel::cUserStatus), fContacts->index(Row, TUsersModel::cUserStatus));
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TUserAccount::slot_SetChats - Слот задаст список бесед
 * @param inChats - Список бесед
 */
void TUserAccount::slot_SetChats(const QList<ChatInfo_Ptr> &inChats)
{
    fChats->clear();

    std::for_each(inChats.begin(), inChats.end(), [&](const ChatInfo_Ptr &Info)
    {
        auto InsertRes = fChats->insert(std::make_pair(Info->chatUuid(), Info));
        if (!InsertRes.second)
            qDebug() << "Не удалось вставить беседу: " + Info->chatName();
    });
}
//-----------------------------------------------------------------------------
/**
 * @brief TUserAccount::slot_AddChat - Слот добавит беседу
 * @param inChat - Новая беседа
 */
void TUserAccount::slot_AddChat(const ChatInfo_Ptr inChat)
{
    if (!inChat)
        return;

    auto FindRes = fChats->find(inChat->chatUuid()); // Ищим беседу по Uuid

    if (FindRes == fChats->end()) // Если не найдена
    {   // Добавляем
        fChats->insert(std::make_pair(inChat->chatUuid(), inChat));
    }
    else // Если найдена
    {   // Обновляем
        FindRes->second = inChat;
    }
}
//-----------------------------------------------------------------------------

namespace Core
{   // Во избежании затупов со стороны компиллера, требуется оборачивать реализацию в тот же неймспейс принудительно
    QDataStream& operator <<(QDataStream &outStream, const TUserAccount &UserAccount)
    {
        outStream << *UserAccount.fUserInfo; // Передаём информацию о пользователе
        //--
        QList<Core::TUserInfo> ContactsBuf; // Список передоваемых объектов типа "Инфо Пользователя"
        std::for_each(UserAccount.fContacts->begin(), UserAccount.fContacts->end(), [&ContactsBuf](const std::pair<QUuid, Core::UserInfo_Ptr> &Contact)
        { // Преобразовываем указатели в объекты
            ContactsBuf.push_back(*Contact.second);
        });
        outStream << ContactsBuf; // Передаём список контактов
        //--
        QList<Core::TChatInfo> ChatsBuf; // Список передоваемых объектов типа "Инфо Беседы"
        std::for_each(UserAccount.fChats->begin(), UserAccount.fChats->end(), [&ChatsBuf](const std::pair<QUuid, Core::ChatInfo_Ptr> &Chat)
        { // Преобразовываем указатели в объекты
            ChatsBuf.push_back(*Chat.second);
        });
        outStream << ChatsBuf; // Передаём список бесед
        // Члены TConnection не передаются

        return outStream;
    }
    //-----------------------------------------------------------------------------
    QDataStream& operator >>(QDataStream &inStream, TUserAccount &UserAccount)
    {
        Core::TUserInfo BufUserInfo;
        inStream >> BufUserInfo; // Получаем информацию о пользователе
        UserAccount.fUserInfo = std::make_shared<Core::TUserInfo>(BufUserInfo); // Преобразуем объект к указателю
        //--
        UserAccount.fContacts->clear();
        QList<Core::TUserInfo> ContactsBuf; // Список получаемых объектов типа "Инфо Пользователя"
        inStream >> ContactsBuf; // Получаем список контактов
        std::for_each(ContactsBuf.begin(), ContactsBuf.end(), [&](const Core::TUserInfo &Contact)
        { // Преобразовываем объекты в указатели
            UserAccount.contacts()->insert(std::make_pair(Contact.userUuid(), std::make_shared<Core::TUserInfo>(Contact)));
        });
        //--
        UserAccount.fChats->clear();
        QList<Core::TChatInfo> ChatsBuf; // Список получаемых объектов типа "Инфо Беседы"
        inStream >> ChatsBuf; // Получаем список бесед
        std::for_each(ChatsBuf.begin(), ChatsBuf.end(), [&](const Core::TChatInfo &Chat)
        { // Преобразовываем объекты в указатели
            UserAccount.chats()->insert(std::make_pair(Chat.chatUuid(), std::make_shared<Core::TChatInfo>(Chat)));
        });
        // Члены TConnection не передаются

        return inStream;
    }
    //-----------------------------------------------------------------------------
}


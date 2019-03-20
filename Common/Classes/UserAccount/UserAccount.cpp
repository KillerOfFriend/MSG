#include "UserAccount.h"

#include "Classes/DataModule/DataModule.h"

using namespace Users;

//-----------------------------------------------------------------------------
TUserAccount::TUserAccount(QObject *inParent) : QObject(inParent)
{
    fUserInfo = std::make_shared<TUserInfo>();
    fContacts = std::make_shared<TUsersModel>();
    fChats = std::make_shared<std::map<QUuid,TChatInfo>>();
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
std::shared_ptr<TUserInfo> TUserAccount::userInfo() const // Метод вернёт информацию о пользователе
{ return fUserInfo; }
//-----------------------------------------------------------------------------
std::shared_ptr<TUsersModel> TUserAccount::contacts() const // Метод вернёт список контактов
{ return fContacts; }
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
void TUserAccount::slot_ContactChangeStatus(QUuid inContactUuid, quint8 inNewStatus) // Сот установит контакту указанный статус
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

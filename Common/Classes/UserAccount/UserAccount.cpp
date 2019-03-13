#include "UserAccount.h"

#include "Classes/DataModule/DataModule.h"

//-----------------------------------------------------------------------------
TUserAccount::TUserAccount(QObject *inParent) : QObject(inParent)
{

}
//-----------------------------------------------------------------------------
TUserAccount::TUserAccount(const TUserAccount &inOther) : QObject(inOther.parent())
{
    this->UserInfo = inOther.UserInfo; // Копируем инфо пользователя
    this->Contscts = inOther.Contscts; // Копируем контейнер контактов
}
//-----------------------------------------------------------------------------
TUserAccount::~TUserAccount()
{

}
//-----------------------------------------------------------------------------
TUserAccount& TUserAccount::operator = (const TUserAccount &inOther)
{
    if (this == &inOther)
         return *this;

    this->UserInfo = inOther.UserInfo; // Копируем инфо пользователя
    this->Contscts = inOther.Contscts; // Копируем контейнер контактов

    return *this;
}
//-----------------------------------------------------------------------------
/**
 * @brief TUserAccount::slot_SetUserInfo - Слот задаст данные пользователя
 * @param inUserInfo - Ссылка на данные пользователя
 */
void TUserAccount::slot_SetUserInfo(const TUserInfo &inUserInfo)
{
    UserInfo = inUserInfo;
    Contscts.clear();
}
//-----------------------------------------------------------------------------
/**
 * @brief TUserAccount::slot_SetContacts - Слот задаст список контактов
 * @param inContacts - Ссылка на список контактов
 */
void TUserAccount::slot_SetContacts(const QList<TUserInfo> &inContacts)
{
    Contscts.clear();

    std::for_each(inContacts.begin(), inContacts.end(), [&](const TUserInfo &Info)
    {
       // auto a = std::make_pair(Info.userUuid(), Info);
        Contscts.insert(std::make_pair(Info.userUuid(), Info));
    });
}
//-----------------------------------------------------------------------------

#include "UserAccount.h"

#include "Classes/DataModule/DataModule.h"

//-----------------------------------------------------------------------------
TUserAccount::TUserAccount(QObject *inParent) : QObject(inParent)
{
    fContscts = std::make_shared<TUsersModel>(this);
}
//-----------------------------------------------------------------------------
TUserAccount::~TUserAccount()
{

}
//-----------------------------------------------------------------------------
/**
 * @brief TUserAccount::Contacts - Метод вернёт список контактов
 * @return Вернёт указатель на список контактов
 */
std::shared_ptr<TUsersModel> TUserAccount::Contacts()
{ return fContscts; }
//-----------------------------------------------------------------------------
/**
 * @brief TUserAccount::slot_SetUserInfo - Слот задаст данные пользователя
 * @param inUserInfo - Ссылка на данные пользователя
 */
void TUserAccount::slot_SetUserInfo(const TUserInfo &inUserInfo)
{
    UserInfo = inUserInfo;

    if (fContscts)
    {
        fContscts->clear();
        TDM::Instance().Client()->getContacts(UserInfo.userUuid());
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TUserAccount::slot_SetContacts - Слот задаст список контактов
 * @param inContacts - Ссылка на список контактов
 */
void TUserAccount::slot_SetContacts(const QList<TUserInfo> &inContacts)
{
    if (fContscts)
    {
        fContscts->clear();
        (*fContscts) << inContacts;
    }
}
//-----------------------------------------------------------------------------

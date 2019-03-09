#include "UserAccount.h"

//-----------------------------------------------------------------------------
TUserAccount::TUserAccount(QObject *inParent) : QObject(inParent)
{

}
//-----------------------------------------------------------------------------
TUserAccount::~TUserAccount()
{

}
//-----------------------------------------------------------------------------
void TUserAccount::slot_SetUserinfo(const TUserInfo &inUserInfo)
{ UserInfo = inUserInfo; }
//-----------------------------------------------------------------------------


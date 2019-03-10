#include "UserInfo.h"

//-----------------------------------------------------------------------------
TUserInfo::TUserInfo(QObject *inParent) : QObject(inParent)
{

}
//-----------------------------------------------------------------------------
TUserInfo::TUserInfo(const TUserInfo &inOther) : QObject(inOther.parent())
{
    this->fUuid = inOther.fUuid;
    this->fType = inOther.fType;
    this->fLogin = inOther.fLogin;
    this->fName = inOther.fName;
}
//-----------------------------------------------------------------------------
TUserInfo& TUserInfo::operator =(const TUserInfo &inOther)
{
    if (this == &inOther)
         return *this;

    this->fUuid = inOther.fUuid;
    this->fType = inOther.fType;
    this->fLogin = inOther.fLogin;
    this->fName = inOther.fName;

    return *this;
}
//-----------------------------------------------------------------------------
bool TUserInfo::operator == (const TUserInfo &inOther) const
{ return this->userUuid() == inOther.userUuid(); }
//-----------------------------------------------------------------------------
TUserInfo::~TUserInfo()
{
    fLogin.clear();
    fName.clear();
}
//-----------------------------------------------------------------------------
void TUserInfo::setUserUuid(QUuid inUuid)
{ fUuid = inUuid; }
//-----------------------------------------------------------------------------
QUuid TUserInfo::userUuid() const
{ return fUuid; }
//-----------------------------------------------------------------------------
void TUserInfo::setUserType(quint32 inType)
{ fType = inType; }
//-----------------------------------------------------------------------------
quint32 TUserInfo::userType() const
{ return fType; }
//-----------------------------------------------------------------------------
void TUserInfo::setUserLogin(QString inLogin)
{ fLogin = inLogin; }
//-----------------------------------------------------------------------------
QString TUserInfo::userLogin() const
{ return fLogin; }
//-----------------------------------------------------------------------------
void TUserInfo::setUserName(QString inName)
{ fName = inName; }
//-----------------------------------------------------------------------------
QString TUserInfo::userName() const
{ return fName; }
//-----------------------------------------------------------------------------
QDataStream &operator <<(QDataStream &outStream, const TUserInfo &UserInfo)
{
    outStream << UserInfo.fUuid;
    outStream << UserInfo.fType;
    outStream << UserInfo.fLogin;
    outStream << UserInfo.fName;

    return outStream;
}
//-----------------------------------------------------------------------------
QDataStream &operator >>(QDataStream &inStream, TUserInfo &UserInfo)
{
    inStream >> UserInfo.fUuid;
    inStream >> UserInfo.fType;
    inStream >> UserInfo.fLogin;
    inStream >> UserInfo.fName;

    return inStream;
}
//-----------------------------------------------------------------------------

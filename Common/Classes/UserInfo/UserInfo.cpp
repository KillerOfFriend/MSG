#include "UserInfo.h"

using namespace Users;

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
    this->fStatus = inOther.fStatus;
    this->fRegistrationDate = inOther.fRegistrationDate;
    this->fBirthday = inOther.fBirthday;
    this->fIsMale = inOther.fIsMale;
    this->fAvatar = inOther.fAvatar;
}
//-----------------------------------------------------------------------------
TUserInfo::~TUserInfo()
{
    fLogin.clear();
    fName.clear();
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
    this->fStatus = inOther.fStatus;
    this->fRegistrationDate = inOther.fRegistrationDate;
    this->fBirthday = inOther.fBirthday;
    this->fIsMale = inOther.fIsMale;
    this->fAvatar = inOther.fAvatar;

    this->setParent(inOther.parent());

    return *this;
}
//-----------------------------------------------------------------------------
bool TUserInfo::operator == (const TUserInfo &inOther) const
{ return this->userUuid() == inOther.userUuid(); }
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
void TUserInfo::setUserStatus(quint8 inStatus)
{ fStatus = inStatus; }
//-----------------------------------------------------------------------------
quint8 TUserInfo::userStatus() const
{ return fStatus; }
//-----------------------------------------------------------------------------
void TUserInfo::setUserRegistrationDate(QDate inRegDate)
{ fRegistrationDate = inRegDate; }
//-----------------------------------------------------------------------------
QDate TUserInfo::userRegistrationDate() const
{ return fRegistrationDate; }
//-----------------------------------------------------------------------------
void TUserInfo::setUserBirthday(QDate inBirthday)
{ fBirthday = inBirthday; }
//-----------------------------------------------------------------------------
QDate TUserInfo::userBirthday() const
{ return fBirthday; }
//-----------------------------------------------------------------------------
void TUserInfo::setUserIsMale(bool inIsMale)
{ fIsMale = inIsMale; }
//-----------------------------------------------------------------------------
bool TUserInfo::userIsMale() const
{ return fIsMale; }
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void TUserInfo::setUserAvatar(QImage &inAvatar)
{ fAvatar = inAvatar; }
//-----------------------------------------------------------------------------
QImage TUserInfo::userAvatar() const
{ return fAvatar; }
//-----------------------------------------------------------------------------

namespace Users
{   // Во избежании затупов со стороны компиллера, требуется оборачивать реализацию в тот же неймспейс принудительно
    QDataStream& operator <<(QDataStream &outStream, const TUserInfo &UserInfo)
    {
        outStream << UserInfo.fUuid;
        outStream << UserInfo.fType;
        outStream << UserInfo.fLogin;
        outStream << UserInfo.fName;
        outStream << UserInfo.fStatus;
        outStream << UserInfo.fRegistrationDate;
        outStream << UserInfo.fBirthday;
        outStream << UserInfo.fIsMale;
        outStream << UserInfo.fAvatar;

        return outStream;
    }
    //-----------------------------------------------------------------------------
    QDataStream& operator >>(QDataStream &inStream, TUserInfo &UserInfo)
    {
        inStream >> UserInfo.fUuid;
        inStream >> UserInfo.fType;
        inStream >> UserInfo.fLogin;
        inStream >> UserInfo.fName;
        inStream >> UserInfo.fStatus;
        inStream >> UserInfo.fRegistrationDate;
        inStream >> UserInfo.fBirthday;
        inStream >> UserInfo.fIsMale;
        inStream >> UserInfo.fAvatar;

        return inStream;
    }
    //-----------------------------------------------------------------------------
}

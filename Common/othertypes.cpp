#include "othertypes.h"

using namespace OtherTypes;

//-----------------------------------------------------------------------------
TUserType::TUserType(qint32 inTypeCode, QString inTypeName) : TypeCode(inTypeCode), TypeName(inTypeName)
{

}
//-----------------------------------------------------------------------------
TUserType::TUserType(const TUserType &inOther) : TypeCode(inOther.TypeCode), TypeName(inOther.TypeName)
{

}
//-----------------------------------------------------------------------------
TUserType& TUserType::operator =(const TUserType &inOther)
{
    if (this == &inOther)
         return *this;

    this->TypeCode = inOther.TypeCode;
    this->TypeName = inOther.TypeName;

    return *this;
}
//-----------------------------------------------------------------------------
namespace OtherTypes
{
    //-----------------------------------------------------------------------------
    QDataStream& operator <<(QDataStream &outStream, const TUserType &UserType)
    {
        outStream << UserType.TypeCode;
        outStream << UserType.TypeName;

        return outStream;
    }
    //-----------------------------------------------------------------------------
    QDataStream& operator >>(QDataStream &inStream, TUserType &UserType)
    {
        inStream >> UserType.TypeCode;
        inStream >> UserType.TypeName;

        return inStream;
    }
    //-----------------------------------------------------------------------------
}

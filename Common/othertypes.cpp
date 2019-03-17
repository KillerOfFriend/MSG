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

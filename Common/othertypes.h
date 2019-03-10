#ifndef OTHERTYPES_H
#define OTHERTYPES_H

#include <qglobal.h>

namespace OtherTypes
{
    struct TUserType
    {
        TUserType(qint32 inTypeCode = 0, QString inTypeName = ""): TypeCode(inTypeCode), TypeName(inTypeName) {};

        qint32 TypeCode;
        QString TypeName;

        bool operator () (const TUserType &inFirst, const TUserType &inSecond) const
        { return inFirst.TypeCode < inSecond.TypeCode; }
    };


}

#endif // OTHERTYPES_H

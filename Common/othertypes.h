#ifndef OTHERTYPES_H
#define OTHERTYPES_H

#include <qglobal.h>

namespace OtherTypes
{
    struct TUserType
    {
        qint32 TypeCode;
        QString TypeName;

        bool operator () (const TUserType &inFirst, const TUserType &inSecond) const
        { return inFirst.TypeCode < inSecond.TypeCode; }
    };


}

#endif // OTHERTYPES_H

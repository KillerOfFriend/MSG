#ifndef OTHERTYPES_H
#define OTHERTYPES_H

#include <QtGlobal>
#include <QDataStream>

namespace OtherTypes
{
    struct TUserType
    {
        TUserType(qint32 inTypeCode = 0, QString inTypeName = "");
        TUserType(const TUserType &inOther);

        qint32 TypeCode;
        QString TypeName;

        TUserType &operator =(const TUserType &inOther);

        bool operator () (const TUserType &inFirst, const TUserType &inSecond) const
        { return inFirst.TypeCode < inSecond.TypeCode; }

        friend QDataStream& operator <<(QDataStream&, const TUserType&);
        friend QDataStream& operator >>(QDataStream&, TUserType&);
    };


}

#endif // OTHERTYPES_H

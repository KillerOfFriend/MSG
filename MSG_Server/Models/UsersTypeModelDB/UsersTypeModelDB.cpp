#include "UsersTypeModelDB.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

#include "Classes/DB/DB.h"

//-----------------------------------------------------------------------------
TUsersTypeModelDB::TUsersTypeModelDB(QObject* inParent) : TUsersTypeModel(inParent)
{
    initFromDB();
}
//-----------------------------------------------------------------------------
TUsersTypeModelDB::~TUsersTypeModelDB()
{

}
//-----------------------------------------------------------------------------
bool TUsersTypeModelDB::initFromDB()
{
    bool Result = true;
    this->clear();

    QSqlQuery Query(TDB::Instance().DB());

    if (!Query.exec("SELECT * FROM get_users_types()"))
    {
        Result = false;
        qDebug() << "[ОШИБКА]: " + Query.lastError().text();
    }
    else
    {
        if (!Query.record().count())
            Result = false;
        else
            while (Query.next()) // Вернётся только 1 запись
            {
                OtherTypes::TUserType NewType;

                NewType.TypeCode = Query.value("out_type_code").toInt();
                NewType.TypeName = QString::fromUtf8(Query.value("out_type_name").toByteArray());

                this->insert(NewType);
            }
    }

    return Result;
}
//-----------------------------------------------------------------------------

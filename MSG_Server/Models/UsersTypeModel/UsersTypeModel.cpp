#include "UsersTypeModel.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

#include "Classes/DB/DB.h"

//-----------------------------------------------------------------------------
TUsersTypeModel::TUsersTypeModel(QObject *inParent) : QAbstractListModel(inParent)
{
    initFromDB();
}
//-----------------------------------------------------------------------------
TUsersTypeModel::~TUsersTypeModel()
{
    this->clear();
}
//-----------------------------------------------------------------------------
bool TUsersTypeModel::initFromDB()
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
int TUsersTypeModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return this->size();
}
//-----------------------------------------------------------------------------
QVariant TUsersTypeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if ( index.row() < 0 || index.row() >= rowCount() )
        return QVariant();

    QVariant Result = QVariant();

    auto It = this->begin();
    std::advance(It, index.row());

    switch (role)
    {
        case TypeCodeRole: { Result = It->TypeCode; break; }
        case Qt::DisplayRole: { Result = It->TypeName;  break; }
        default: { Result = QVariant(); break; }
    }

    return Result;
}
//-----------------------------------------------------------------------------
std::pair<std::set<OtherTypes::TUserType>::iterator, bool> TUsersTypeModel::insert(OtherTypes::TUserType &inItem)
{
    auto It = std::set<OtherTypes::TUserType, OtherTypes::TUserType>::insert(inItem);

    if (It.second)
    {
        std::size_t Row = std::distance(this->begin(), It.first);
        beginInsertRows(QModelIndex(), Row, Row);
        endInsertRows();
    }

    return It;
}
//-----------------------------------------------------------------------------
void TUsersTypeModel::erase(std::set<OtherTypes::TUserType>::iterator inIt)
{
    std::size_t Row = std::distance(this->begin(), inIt);
    beginRemoveRows(QModelIndex(), Row, Row);
    std::set<OtherTypes::TUserType, OtherTypes::TUserType>::erase(inIt);
    endRemoveRows();
}
//-----------------------------------------------------------------------------
void TUsersTypeModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, rowCount());
    std::set<OtherTypes::TUserType, OtherTypes::TUserType>::clear();
    endRemoveRows();
}
//-----------------------------------------------------------------------------

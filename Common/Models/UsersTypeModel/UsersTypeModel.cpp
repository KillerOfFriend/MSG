#include "UsersTypeModel.h"

#include <QDebug>

//-----------------------------------------------------------------------------
TUsersTypeModel::TUsersTypeModel(QObject *inParent) : QAbstractListModel(inParent)
{

}
//-----------------------------------------------------------------------------
TUsersTypeModel::~TUsersTypeModel()
{
    this->clear();
}
//-----------------------------------------------------------------------------
int TUsersTypeModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return this->size();
}
//-----------------------------------------------------------------------------
void TUsersTypeModel::fromList(QList<OtherTypes::TUserType> &inList) // Метод инициализирует содержимое из списка
{
    this->clear();

    std::for_each(inList.begin(), inList.end(), [&](const OtherTypes::TUserType& Item)
    {
        this->insert(Item);
    });
}
//-----------------------------------------------------------------------------
QList<OtherTypes::TUserType> TUsersTypeModel::toList() // Метод вернёт содержимое в виде списка
{
    QList<OtherTypes::TUserType> Result;

    std::for_each(this->begin(), this->end(), [&](const OtherTypes::TUserType& Item)
    {
        Result.push_back(Item);
    });

    return Result;
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
std::pair<std::set<OtherTypes::TUserType>::iterator, bool> TUsersTypeModel::insert(const OtherTypes::TUserType &inItem)
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

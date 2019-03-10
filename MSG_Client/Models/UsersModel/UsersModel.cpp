#include "UsersModel.h"

//-----------------------------------------------------------------------------
TUsersModel::TUsersModel(QObject *inParent) : QAbstractTableModel(inParent)
{
    initColumns();
}
//-----------------------------------------------------------------------------
TUsersModel::~TUsersModel()
{
    clear();
}
//-----------------------------------------------------------------------------
int TUsersModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return this->size();
}
//-----------------------------------------------------------------------------
int TUsersModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return fColumnCount;
}
//-----------------------------------------------------------------------------
QVariant TUsersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if ( (index.row() < 0 || index.row() >= rowCount()) || (index.column() < 0 || index.column() >= columnCount()) )
        return QVariant();

    QVariant Result = QVariant();

    auto It = this->begin();
    std::advance(It, index.row());

    switch (role)
    {
        case Qt::DisplayRole:
        {
            switch (index.column())
            {
                case cUserLogin: { Result = It->userLogin(); break; }
                case cUserName: { Result = It->userName(); break; }
                case cUserUuid: { Result = It->userUuid(); break; }
                case cUserType: { Result = It->userType(); break; }
                default: { Result = QVariant(); break; }
            }
            break; // case Qt::DisplayRole:
        }
        default: { Result = QVariant(); break; }
    }

    return Result;
}
//-----------------------------------------------------------------------------
QVariant TUsersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( section < 0 || section >= columnCount() )
        return QVariant();

    QVariant Result = QVariant();

    if (role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal)
        Result = fColumns[section];
    else
        Result = QVariant();

    return Result;
}
//-----------------------------------------------------------------------------
void TUsersModel::push_back(const TUserInfo &inUserInfo)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    QList<TUserInfo>::push_back(inUserInfo);
    endInsertRows();
}
//-----------------------------------------------------------------------------
void TUsersModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, rowCount());
    QList<TUserInfo>::clear();
    endRemoveRows();
}
//-----------------------------------------------------------------------------
void TUsersModel::initColumns()
{
    fColumns[cUserLogin] = tr("Логин");
    fColumns[cUserName] = tr("Имя");
    fColumns[cUserType] = tr("Тип");
    fColumns[cUserUuid] = tr("Uuid");
}
//-----------------------------------------------------------------------------

#include "UsersModel.h"

#include <QDebug>
#include <QPixmap>

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
TUsersModel& TUsersModel::operator = (const TUsersModel &inOther)
{
    if (this == &inOther)
         return *this;

    this->fColumns = inOther.fColumns;
    this->clear();

    std::for_each(inOther.begin(), inOther.end(), [&](const std::pair<QUuid, Users::TUserInfo> &Item)
    {
        this->insert(Item);
    });

    return *this;
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
                case cUserLogin:    { Result = It->second.userLogin(); break; }
                case cUserName:     { Result = It->second.userName(); break; }
                case cUserStatus:   {Result = It->second.userStatus(); break; }
                case cUserUuid:     { Result = It->second.userUuid(); break; }
                case cUserType:     { Result = It->second.userType(); break; }
                case cUserRegDate:  { Result = It->second.userRegistrationDate(); break; }
                case cUserBirthday: { Result = It->second.userBirthday(); break; }
                case cUserIsMale:   { Result = It->second.userIsMale(); break; }

                case cUserAvatar: { Result = It->second.userAvatar(); break; }
                default: { Result = QVariant(); break; }
            }
            break; // case Qt::DisplayRole:
        }
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
std::pair<std::map<QUuid, Users::TUserInfo>::iterator, bool> TUsersModel::insert(const std::pair<QUuid, Users::TUserInfo> &inValue)
{
   auto It = std::map<QUuid, Users::TUserInfo>::insert(inValue);

   if (It.second)
   {
        qint32 Row = std::distance(this->begin(), It.first);
        beginInsertRows(QModelIndex(), Row, Row);
        endInsertRows();
   }
   else
       qDebug() << "Дубликат вставки! " << inValue.second.userLogin();

   return It;
}
//-----------------------------------------------------------------------------
void TUsersModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, rowCount());
    std::map<QUuid, Users::TUserInfo>::clear();
    endRemoveRows();
}
//-----------------------------------------------------------------------------
void TUsersModel::initColumns()
{
    fColumns[cUserLogin] = tr("Логин");
    fColumns[cUserName] = tr("Имя");
    fColumns[cUserStatus] = tr("Статус");
    fColumns[cUserType] = tr("Тип");
    fColumns[cUserUuid] = tr("Uuid");
    fColumns[cUserRegDate] = tr("Дата регистрации");
    fColumns[cUserBirthday] = tr("Дата рождения");
    fColumns[cUserIsMale] = tr("Мужик? =)");
    fColumns[cUserAvatar] = tr("Аватар");
}
//-----------------------------------------------------------------------------

#include "ConnectedUsers.h"

#include <algorithm>

#include <QHostAddress>
#include <QColor>

//-----------------------------------------------------------------------------
TConnectedUsersModel::TConnectedUsersModel(QObject *inParent) : QAbstractTableModel(inParent)
{
    fUserTypes = std::make_shared<TUsersTypeModel>(this);
    initColumns();
}
//-----------------------------------------------------------------------------
TConnectedUsersModel::~TConnectedUsersModel()
{
    clear();
}
//-----------------------------------------------------------------------------
int TConnectedUsersModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return this->size();
}
//-----------------------------------------------------------------------------
int TConnectedUsersModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return fColumnCount;
}
//-----------------------------------------------------------------------------
QVariant TConnectedUsersModel::data(const QModelIndex &index, int role) const
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
                case cUserAddres: { Result = It->first->peerAddress().toString(); break; }
                case cUserLogin: { Result = It->second.userLogin(); break; }
                case cUserName: { Result = It->second.userName(); break; }
                case cUserUuid: { Result = It->second.userUuid(); break; }
                case cUserType:
                {
                    OtherTypes::TUserType ForSeach(It->second.userType(), "");
                    auto TypeIt = fUserTypes->find(ForSeach);

                    if (TypeIt != fUserTypes->end())
                        Result = TypeIt->TypeName;
                    else Result = QVariant();

                    break;
                }
                default: { Result = QVariant(); break; }
            }
            break; // case Qt::DisplayRole:
        }
        case Qt::TextColorRole:
        {
            switch (index.column())
            {
                case cUserAddres: { Result = QColor(64, 0, 255); break; }
                case cUserLogin: { Result = QColor(255,99,71); break; }
                case cUserName: { Result = QColor(210,105,30); break; }
                case cUserUuid: { Result = QColor(255,215,0); break; }
                case cUserType: { Result = QColor(50, 205, 50); break; }
                default: { Result = QVariant(); break; }
            }
            break; // case Qt::TextColorRole:
        }
        default: { Result = QVariant(); break; }
    }

    return Result;
}
//-----------------------------------------------------------------------------
QVariant TConnectedUsersModel::headerData(int section, Qt::Orientation orientation, int role) const
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
std::pair<std::map<QTcpSocket*, TUserInfo>::iterator, bool> TConnectedUsersModel::insert(std::pair<QTcpSocket*, TUserInfo> inItem)
{
    auto It = std::map<QTcpSocket*, TUserInfo>::insert(inItem);

    if (It.second)
    {
        std::size_t Row = std::distance(this->begin(), It.first);
        beginInsertRows(QModelIndex(), Row, Row);
        endInsertRows();
    }

    return It;
}
//-----------------------------------------------------------------------------
void TConnectedUsersModel::erase(std::map<QTcpSocket*, TUserInfo>::iterator inIt)
{
    std::size_t Row = std::distance(this->begin(), inIt);
    beginRemoveRows(QModelIndex(), Row, Row);
    std::map<QTcpSocket*, TUserInfo>::erase(inIt);
    endRemoveRows();
}
//-----------------------------------------------------------------------------
void TConnectedUsersModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, rowCount());
    std::map<QTcpSocket*, TUserInfo>::clear();
    endRemoveRows();
}
//-----------------------------------------------------------------------------
void TConnectedUsersModel::initColumns()
{
    fColumns[cUserAddres] = tr("Адрес");
    fColumns[cUserLogin] = tr("Логин");
    fColumns[cUserName] = tr("Имя");
    fColumns[cUserType] = tr("Тип");
    fColumns[cUserUuid] = tr("Uuid");
}
//-----------------------------------------------------------------------------
void TConnectedUsersModel::slot_UpdateRow(qint32 inRowNumber)
{
    dataChanged(index(inRowNumber, firstColumnIndex), index(inRowNumber, lastColumnIndex));
}
//-----------------------------------------------------------------------------

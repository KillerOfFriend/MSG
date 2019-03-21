#include "ChatsModel.h"

#include <QDebug>

//-----------------------------------------------------------------------------
TChatsModel::TChatsModel(QObject* inParent) : QAbstractTableModel(inParent)
{
    initColumns();
}
//-----------------------------------------------------------------------------
TChatsModel::~TChatsModel()
{
    clear();
}
//-----------------------------------------------------------------------------
TChatsModel& TChatsModel::operator = (const TChatsModel &inOther)
{
    if (this == &inOther)
         return *this;

    this->fColumns = inOther.fColumns;
    this->clear();

    std::for_each(inOther.begin(), inOther.end(), [&](const std::pair<QUuid, Users::TChatInfo> &Item)
    {
        this->insert(Item);
    });

    this->setParent(inOther.parent());

    return *this;
}
//-----------------------------------------------------------------------------
int TChatsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return this->size();
}
//-----------------------------------------------------------------------------
int TChatsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return fColumnCount;
}
//-----------------------------------------------------------------------------
QVariant TChatsModel::data(const QModelIndex &index, int role) const
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
                case cChatUuid:             { Result = It->second.chatUuid(); break; }
                case cChatName:             { Result = It->second.chatName(); break; }
                case cChatPrivateStatus:    { Result = It->second.chatPrivateStatus(); break; }

                default: { Result = QVariant(); break; }
            }
            break; // case Qt::DisplayRole:
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
QVariant TChatsModel::headerData(int section, Qt::Orientation orientation, int role) const
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
std::pair<std::map<QUuid, Users::TChatInfo>::iterator, bool> TChatsModel::insert(const std::pair<QUuid, Users::TChatInfo> &inValue)
{
    auto It = std::map<QUuid, Users::TChatInfo>::insert(inValue);

    if (It.second)
    {
         qint32 Row = std::distance(this->begin(), It.first);
         beginInsertRows(QModelIndex(), Row, Row);
         endInsertRows();
    }
    else
        qDebug() << "Дубликат вставки! " << inValue.second.chatName();

    return It;
}
//-----------------------------------------------------------------------------
std::map<QUuid, Users::TChatInfo>::iterator TChatsModel::erase(std::map<QUuid, Users::TChatInfo>::iterator inIt)
{
    qint32 Row = std::distance(this->begin(), inIt);

    beginRemoveRows(QModelIndex(), Row, Row);
    auto Res =std::map<QUuid, Users::TChatInfo>::erase(inIt);
    endRemoveRows();

    return Res;
}
//-----------------------------------------------------------------------------
std::map<QUuid, Users::TChatInfo>::size_type TChatsModel::erase(const QUuid &inUuid)
{
    auto It = this->find(inUuid);
    qint32 Row = std::distance(this->begin(), It);

    beginRemoveRows(QModelIndex(), Row, Row);
    size_type Res = std::map<QUuid, Users::TChatInfo>::erase(inUuid);
    endRemoveRows();

    return Res;
}
//-----------------------------------------------------------------------------
void TChatsModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, rowCount());
    std::map<QUuid, Users::TChatInfo>::clear();
    endRemoveRows();
}
//-----------------------------------------------------------------------------
void TChatsModel::initColumns()
{
    fColumns[cChatUuid] = tr("Uuid беседы");
    fColumns[cChatName] = tr("Название беседы");
    fColumns[cChatPrivateStatus] = tr("Приватность");
}
//-----------------------------------------------------------------------------

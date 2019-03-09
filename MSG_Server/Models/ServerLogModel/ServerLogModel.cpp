#include "ServerLogModel.h"

#include <QColor>

//-----------------------------------------------------------------------------
TServerLogModel::TServerLogModel(QObject* inParent) : QAbstractTableModel(inParent)
{
    initColumns();
    fRecords.max_size();
}
//-----------------------------------------------------------------------------
TServerLogModel::~TServerLogModel()
{
    fRecords.clear();
}
//-----------------------------------------------------------------------------
void TServerLogModel::slot_NewMessage(QHostAddress inAddres, QString inMessage)
{
    TRecord NewRecord = { QDateTime::currentDateTime(), inAddres, inMessage };

    if(fRecords.size() == fMaxLines)
    {
        beginRemoveRows(QModelIndex(), 0, 0);
        fRecords.pop_back();
        endRemoveRows();
    }

    beginInsertRows(QModelIndex(), fRecords.size(), fRecords.size());
    fRecords.push_back(NewRecord);
    endInsertRows();
}
//-----------------------------------------------------------------------------
int TServerLogModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return fRecords.size();
}
//-----------------------------------------------------------------------------
int TServerLogModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return fColumnCount;
}
//-----------------------------------------------------------------------------
QVariant TServerLogModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if ( (index.row() < 0 || index.row() >= rowCount()) || (index.column() < 0 || index.column() >= columnCount()) )
        return QVariant();

    QVariant Result = QVariant();

    auto It = fRecords.begin();
    std::advance(It, index.row());

    switch (role)
    {
        case Qt::DisplayRole:
        {
            switch (index.column())
            {
                case cTime: { Result = It->Time.toString("[dd.MM.yyyy] {hh:mm:ss.zzz}"); break; }
                case cAddres:
                {
                    if (It->ClientAddres.isNull())
                        Result = "---";
                    else
                        Result = It->ClientAddres.toString();
                    break;
                }
                case cMessage: { Result = It->Message; break; }
                default: { Result = QVariant(); break; }
            }
            break; // case Qt::DisplayRole:
        }
        case Qt::TextColorRole:
        {
            switch (index.column())
            {
                case cTime:     { Result = QColor(255, 128, 0); break; }
                case cAddres:   { Result = QColor(64, 0, 255); break; }
                case cMessage:  { Result = QColor(0, 0, 0); break; }
                default:        { Result = QVariant(); break; }
            }
            break; // case Qt::TextColorRole:
        }
        case Qt::TextAlignmentRole:
        {
            switch (index.column())
            {
                case cTime:     { Result = int(Qt::AlignLeft | Qt::AlignVCenter); break; }
                case cAddres:   { Result = Qt::AlignCenter; break; }
                case cMessage:  { Result = int(Qt::AlignLeft | Qt::AlignVCenter); break; }
                default:        { Result = QVariant(); break; }
            }
            break;
        }

        default: { Result = QVariant(); break; }
    }
    return Result;
}
//-----------------------------------------------------------------------------
QVariant TServerLogModel::headerData(int section, Qt::Orientation orientation, int role) const
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
void TServerLogModel::initColumns()
{
    fColumns[cTime] = tr("Время");
    fColumns[cAddres] = tr("IP Адрес");
    fColumns[cMessage] = tr("Сообщение");
}
//-----------------------------------------------------------------------------

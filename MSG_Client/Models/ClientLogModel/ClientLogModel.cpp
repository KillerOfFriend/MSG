#include "ClientLogModel.h"

#include <QDateTime>

//-----------------------------------------------------------------------------
TClientLogModel::TClientLogModel(QObject *inParent) : QAbstractListModel(inParent)
{

}
//-----------------------------------------------------------------------------
TClientLogModel::~TClientLogModel()
{
    fMessages.clear();
}
//-----------------------------------------------------------------------------
int TClientLogModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return fMessages.size();
}
//-----------------------------------------------------------------------------
QVariant TClientLogModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if ( (index.row() < 0 || index.row() >= rowCount()) )
        return QVariant();

    QVariant Result = QVariant();

    auto It = fMessages.begin();
    std::advance(It, index.row());

    switch (role)
    {
        case Qt::DisplayRole:
        {
            Result = *It;
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
void TClientLogModel::slot_NewMessage(QString inMessage) // Слод добавит сообщение в модель
{
    beginInsertRows(QModelIndex(), fMessages.size(), fMessages.size());
    fMessages.push_back(QDateTime::currentDateTime().toString("[dd.MM.yyyy] {hh:mm:ss.zzz}") + " " + inMessage);
    endInsertRows();
}
//-----------------------------------------------------------------------------

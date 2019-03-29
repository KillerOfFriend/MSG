#ifndef CHATSPROXYMODEL_H
#define CHATSPROXYMODEL_H

#include <QSortFilterProxyModel>

#include "Models/ChatsModel/ChatsModel.h"

class TChatsProxyModel : public QSortFilterProxyModel
{
public:
    TChatsProxyModel(QObject* inParent = nullptr);
    ~TChatsProxyModel();

    void setSourceModel(QAbstractItemModel* inSourceModel); // Метод задаст модель - источник
    void setFilter(const TChatsModel::eColumns inColumn, const QString inFilter); // Задаём фильтр

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const; // Метод фильтрации

private:
    QString fFilter; // Фильтр
    TChatsModel::eColumns fColumn; // Фильтруемый стольбец
};

#endif // CHATSPROXYMODEL_H

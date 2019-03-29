#ifndef USERSPROXYMODEL_H
#define USERSPROXYMODEL_H

#include <QSortFilterProxyModel>

#include "Models/UsersModel/UsersModel.h"

class TUsersProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    TUsersProxyModel(QObject* inParent = nullptr);
    ~TUsersProxyModel();

    void setSourceModel(QAbstractItemModel* inSourceModel); // Метод задаст модель - источник
    void setFilter(const TUsersModel::eColumns inColumn, const QString inFilter); // Задаём фильтр

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const; // Метод фильтрации

private:
    QString fFilter; // Фильтр
    TUsersModel::eColumns fColumn; // Фильтруемый стольбец

};

#endif // USERSPROXYMODEL_H

#ifndef USERSTYPEMODEL_H
#define USERSTYPEMODEL_H

#include <QAbstractListModel>

#include <set>
#include <map>

#include "othertypes.h"

class TUsersTypeModel : public QAbstractListModel, public std::set<OtherTypes::TUserType, OtherTypes::TUserType>
{
public:
    static const qint32 TypeCodeRole = Qt::UserRole + 1;

    TUsersTypeModel(QObject* inParent = nullptr);
    ~TUsersTypeModel();

    bool initFromDB();
    //-- Методы модели
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    //-- Методы контейнера
    std::pair<std::set<OtherTypes::TUserType>::iterator, bool> insert(OtherTypes::TUserType &inItem);
    void erase(std::set<OtherTypes::TUserType>::iterator inIt);
    void clear();

private:

};

#endif // USERSTYPEMODEL_H

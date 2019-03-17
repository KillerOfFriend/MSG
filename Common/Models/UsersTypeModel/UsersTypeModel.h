#ifndef USERSTYPEMODEL_H
#define USERSTYPEMODEL_H

#include <set>
#include <map>

#include <QAbstractListModel>
#include <QList>

#include "othertypes.h"

class TUsersTypeModel : public QAbstractListModel, public std::set<OtherTypes::TUserType, OtherTypes::TUserType>
{
public:
    static const qint32 TypeCodeRole = Qt::UserRole + 1;

    TUsersTypeModel(QObject* inParent = nullptr);
    ~TUsersTypeModel();

    void fromList(QList<OtherTypes::TUserType> &inList); // Метод инициализирует содержимое из списка
    QList<OtherTypes::TUserType> toList(); // Метод вернёт содержимое в виде списка

    //-- Методы модели
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    //-- Методы контейнера
    std::pair<std::set<OtherTypes::TUserType>::iterator, bool> insert(const OtherTypes::TUserType &inItem);
    void erase(std::set<OtherTypes::TUserType>::iterator inIt);
    void clear();

private:

};

#endif // USERSTYPEMODEL_H

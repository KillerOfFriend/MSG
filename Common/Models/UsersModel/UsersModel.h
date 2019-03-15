#ifndef USERSMODEL_H
#define USERSMODEL_H

#include <map>
#include <array>

#include <QUuid>
#include <QAbstractTableModel>

#include "Classes/UserInfo/UserInfo.h"

class TUsersModel : public QAbstractTableModel, public std::map<QUuid, TUserInfo>
{
    Q_OBJECT
public:
    enum eColumns { cUserLogin = 0, cUserName = 1, cUserType = 2, cUserUuid = 3 };

    TUsersModel(QObject* inParent = nullptr);
    ~TUsersModel();

    TUsersModel& operator = (const TUsersModel &inOther);

    //-- Методы модели
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    //--
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    //-- Методы контейнера
    std::pair<std::map<QUuid, TUserInfo>::iterator, bool> insert(const std::pair<QUuid, TUserInfo> &inValue);
    void clear();

private:
    static const quint8 fColumnCount = 4;
    std::array<QString, fColumnCount> fColumns;

    void initColumns();
};

#endif // USERSMODEL_H

#ifndef USERSMODEL_H
#define USERSMODEL_H

#include <map>
#include <array>

#include <QUuid>
#include <QAbstractTableModel>

#include "Classes/UserInfo/UserInfo.h"

class TUsersModel : public QAbstractTableModel, public std::map<QUuid, Users::TUserInfo>
{
    Q_OBJECT
public:
    enum eColumns { cUserLogin = 0, cUserName = 1, cUserStatus = 2, cUserType = 3, cUserUuid = 4, cUserRegDate = 5,
                    cUserBirthday = 6, cUserIsMale = 7, cUserAvatar = 8 };

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
    std::pair<std::map<QUuid, Users::TUserInfo>::iterator, bool> insert(const std::pair<QUuid, Users::TUserInfo> &inValue);
    std::map<QUuid, Users::TUserInfo>::iterator erase(std::map<QUuid, Users::TUserInfo>::iterator inIt);
    std::map<QUuid, Users::TUserInfo>::size_type erase(const QUuid &inUuid);
    void clear();

private:
    static const quint8 fColumnCount = 9;
    std::array<QString, fColumnCount> fColumns;

    void initColumns();
};

#endif // USERSMODEL_H

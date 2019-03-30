#ifndef USERSMODEL_H
#define USERSMODEL_H

#include <map>
#include <array>

#include <QUuid>
#include <QAbstractTableModel>

#include "Classes/UserInfo/UserInfo.h"

class TUsersModel : public QAbstractTableModel, public std::map<QUuid, Core::UserInfo_Ptr>
{
    Q_OBJECT
public:
    enum eColumns { cUserLogin = 0, cUserName = 1, cUserStatus = 2, cUserType = 3, cUserUuid = 4,
                    cUserRegDate = 5, cUserBirthday = 6, cUserIsMale = 7, cUserAvatar = 8 };

    TUsersModel(QObject* inParent = nullptr);
    TUsersModel(const TUsersModel &inOther);
    ~TUsersModel();

    TUsersModel& operator = (const TUsersModel &inOther);

    //-- Методы модели
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    //--
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    //-- Методы контейнера
    std::pair<std::map<QUuid, Core::UserInfo_Ptr>::iterator, bool> insert(const std::pair<QUuid, Core::UserInfo_Ptr> &inValue);
    //--
    std::map<QUuid, Core::UserInfo_Ptr>::iterator erase(std::map<QUuid, Core::UserInfo_Ptr>::iterator inIt);
    std::map<QUuid, Core::UserInfo_Ptr>::size_type erase(const QUuid &inUuid);
    //--
    void clear();

private:
    static const quint8 fColumnCount = 9;
    std::array<QString, fColumnCount> fColumns;

    void initColumns();
};

#endif // USERSMODEL_H

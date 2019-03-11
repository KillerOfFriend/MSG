#ifndef USERSMODEL_H
#define USERSMODEL_H

#include <array>

#include <QList>
#include <QAbstractTableModel>

#include "Classes/UserInfo/UserInfo.h"

class TUsersModel : public QAbstractTableModel, public QList<TUserInfo>
{
    Q_OBJECT
public:
    enum eColumns { cUserLogin = 0, cUserName = 1, cUserType = 2, cUserUuid = 3 };

    TUsersModel(QObject* inParent = nullptr);
    ~TUsersModel();

    //-- Методы модели
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    //--
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    //-- Методы контейнера
    void push_back(const TUserInfo &inUserInfo);
    void clear();

private:
    static const quint8 fColumnCount = 4;
    std::array<QString, fColumnCount> fColumns;

    void initColumns();
};

#endif // USERSMODEL_H

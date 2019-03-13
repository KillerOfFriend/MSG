#ifndef CONNECTEDUSERS_H
#define CONNECTEDUSERS_H

#include <map>
#include <memory>
#include <array>

#include <QAbstractTableModel>
#include <QTcpSocket>

#include "Classes/UserAccount/UserAccount.h"
#include "Models/UsersTypeModel/UsersTypeModel.h"

class TConnectedUsersModel : public QAbstractTableModel, public std::map<QTcpSocket*, TUserAccount>
{
public:
    enum eColumns { cUserAddres = 0, cUserLogin = 1, cUserName = 2, cUserType = 3, cUserUuid = 4 };

    static const eColumns firstColumnIndex = cUserAddres;
    static const eColumns lastColumnIndex = cUserUuid;

    TConnectedUsersModel(QObject* inParent = nullptr);
    ~TConnectedUsersModel();
    //-- Методы модели
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    //--
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    //-- Методы контейнера
    std::pair<std::map<QTcpSocket*, TUserAccount>::iterator, bool> insert(std::pair<QTcpSocket*, TUserAccount> inItem);
    void erase(std::map<QTcpSocket*, TUserAccount>::iterator inIt);
    void clear();

private:
    static const quint8 fColumnCount = 5;
    std::array<QString, fColumnCount> fColumns;

    std::shared_ptr<TUsersTypeModel> fUserTypes = nullptr;

    void initColumns();

public slots:
    void slot_UpdateRow(qint32 inRowNumber);
};

#endif // CONNECTEDUSERS_H

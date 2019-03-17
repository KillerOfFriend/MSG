#ifndef USERSTYPEMODELDB_H
#define USERSTYPEMODELDB_H

#include <QObject>
#include <QDataStream>

#include "Models/UsersTypeModel/UsersTypeModel.h"

class TUsersTypeModelDB : public TUsersTypeModel
{
    Q_OBJECT
public:
    TUsersTypeModelDB(QObject* inParent = nullptr);
    ~TUsersTypeModelDB();

    bool initFromDB();
};

#endif // USERSTYPEMODELDB_H

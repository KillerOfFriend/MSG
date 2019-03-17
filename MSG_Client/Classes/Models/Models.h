#ifndef MODELS_H
#define MODELS_H

#include <memory>

#include <QObject>

#include "Models/UsersTypeModel/UsersTypeModel.h"

class TModels : public QObject
{
    Q_OBJECT
public:
    explicit TModels(QObject *inParent = nullptr);
    ~TModels();

   void init(); // Метод инициализирует модели класса

    std::shared_ptr<TUsersTypeModel> UserTypeModel();

private:
    std::shared_ptr<TUsersTypeModel> fUserTypeModel = nullptr;

signals:

public slots:
    void slot_InitUserTypes(QList<OtherTypes::TUserType> &inTypeList);

};

#endif // MODELS_H

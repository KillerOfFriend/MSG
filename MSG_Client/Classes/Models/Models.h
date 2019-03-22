#ifndef MODELS_H
#define MODELS_H

#include <memory>

#include <QObject>

#include "Models/UsersTypeModel/UsersTypeModel.h"
#include "Models/ClientLogModel/ClientLogModel.h"

class TModels : public QObject
{
    Q_OBJECT
public:
    explicit TModels(QObject *inParent = nullptr);
    ~TModels();

   void init(); // Метод инициализирует модели класса

    std::shared_ptr<TClientLogModel> LogModel(); // Метод вернёт указатель на модель логов
    std::shared_ptr<TUsersTypeModel> UserTypeModel(); // Метод вернёт указатель на модель типов пользователей

private:
    std::shared_ptr<TClientLogModel> fLogModel = nullptr; // Указатель на модель логов клиента
    std::shared_ptr<TUsersTypeModel> fUserTypeModel = nullptr; // Указатель на модель типов пользователей

signals:

public slots:
    void slot_InitUserTypes(QList<OtherTypes::TUserType> &inTypeList); // Слот задаст список типов пользователей

};

#endif // MODELS_H

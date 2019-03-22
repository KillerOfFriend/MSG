#include "Models.h"

//-----------------------------------------------------------------------------
TModels::TModels(QObject *inParent) : QObject(inParent)
{
    init();
}
//-----------------------------------------------------------------------------
TModels::~TModels()
{

}
//-----------------------------------------------------------------------------
void TModels::init() // Метод инициализирует модели класса
{
    fLogModel = std::make_shared<TClientLogModel>(this);
    fUserTypeModel = std::make_shared<TUsersTypeModel>(this);
}
//-----------------------------------------------------------------------------
std::shared_ptr<TClientLogModel> TModels::LogModel() // Метод вернёт указатель на модель типов пользователей
{ return fLogModel; }
//-----------------------------------------------------------------------------
std::shared_ptr<TUsersTypeModel> TModels::UserTypeModel() // Метод вернёт указатель на модель типов пользователей
{ return fUserTypeModel; }
//-----------------------------------------------------------------------------
void TModels::slot_InitUserTypes(QList<OtherTypes::TUserType> &inTypeList) // Слот задаст список типов пользователей
{ fUserTypeModel->fromList(inTypeList); }
//-----------------------------------------------------------------------------

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
    fUserTypeModel = std::make_shared<TUsersTypeModel>(this);
}
//-----------------------------------------------------------------------------
std::shared_ptr<TUsersTypeModel> TModels::UserTypeModel()
{ return fUserTypeModel; }
//-----------------------------------------------------------------------------
void TModels::slot_InitUserTypes(QList<OtherTypes::TUserType> &inTypeList)
{ fUserTypeModel->fromList(inTypeList); }
//-----------------------------------------------------------------------------

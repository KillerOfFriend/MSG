#include "AbstractCommand.h"

using namespace Core;

//-----------------------------------------------------------------------------
TAbstractCommand::TAbstractCommand(QObject *inParent) : QObject(inParent)
{
    fCommandData = std::make_shared<QByteArray>();
}
//-----------------------------------------------------------------------------
/**
 * @brief TAbstractCommand::TAbstractCommand - Конструктор копирования
 * @param inOther - Копируемый экземпляр класса
 */
TAbstractCommand::TAbstractCommand(TAbstractCommand &inOther) : QObject(inOther.parent())
{
    this->fCommandCode = inOther.fCommandCode;
    this->fCommandType = inOther.fCommandType;
    this->fCommandData = inOther.fCommandData;
}
//-----------------------------------------------------------------------------
TAbstractCommand::~TAbstractCommand()
{
    fCommandCode = Commands::Unknown;
    fCommandType = eCommandType::ctUnknown;
    fCommandData->clear();
}
//-----------------------------------------------------------------------------
/**
 * @brief TAbstractCommand::operator = - Оператор присваивания
 * @param inOther - Копируемый экземпляр класса
 * @return Вернёт ссылку на копию экземпляра
 */
TAbstractCommand& TAbstractCommand::operator =(const TAbstractCommand &inOther)
{
    if (this == &inOther)
         return *this;

    this->fCommandCode = inOther.fCommandCode;
    this->fCommandType = inOther.fCommandType;
    this->fCommandData = inOther.fCommandData;

    this->setParent(inOther.parent());

    return *this;
}
//-----------------------------------------------------------------------------
/**
 * @brief TAbstractCommand::setCommandCode - Метод задаст код команды
 * @param inComandCode - Код команды
 */
void TAbstractCommand::setCommandCode(quint8 inComandCode)
{ fCommandCode = inComandCode; }
//-----------------------------------------------------------------------------
/**
 * @brief TAbstractCommand::commandCode - Метод вернёт код команды
 * @return Вернёт код команды
 */
quint8 TAbstractCommand::commandCode()
{ return fCommandCode; }
//-----------------------------------------------------------------------------
/**
 * @brief TAbstractCommand::setData - Метод задаст данные команды
 * @param inData - Ссылка на новые данные команыд
 */
void TAbstractCommand::setData(QByteArray &inData)
{
    fCommandData->clear();
    (*fCommandData) = inData;
}
//-----------------------------------------------------------------------------
/**
 * @brief TAbstractCommand::setData - Метод задаст данные команды
 * @param inData - Указатель на новые данные команды
 */
void TAbstractCommand::setData(std::shared_ptr<QByteArray> inData)
{
    fCommandData = inData;
}
//-----------------------------------------------------------------------------
/**
 * @brief TAbstractCommand::data - Метод вернёт данные команды
 * @return Вернёт указатель на данные команды
 */
std::shared_ptr<QByteArray> TAbstractCommand::data()
{ return fCommandData; }
//-----------------------------------------------------------------------------
/**
 * @brief TAbstractCommand::commandType - Метод вернёт тип команды
 * @return Вернёт тип команды
 */
TAbstractCommand::eCommandType TAbstractCommand::commandType()
{ return fCommandType; }
//-----------------------------------------------------------------------------

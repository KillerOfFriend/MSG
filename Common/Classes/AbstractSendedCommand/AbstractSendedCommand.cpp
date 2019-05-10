#include "AbstractSendedCommand.h"

using namespace Core;

//-----------------------------------------------------------------------------
TAbstractSendedCommand::TAbstractSendedCommand(QObject* inParent) : TAbstractCommand(inParent)
{
    fCommandType = eCommandType::ctOnSend;
}
//-----------------------------------------------------------------------------
TAbstractSendedCommand::~TAbstractSendedCommand()
{

}
//-----------------------------------------------------------------------------
/**
 * @brief TAbstractSendedCommand::interpreted - Метод интерпритации команды, вызывает send (Метод отправки)
 * @return Вернёт флаг успешности операции отправки
 */
bool TAbstractSendedCommand::interpreted()
{ return send(); }
//-----------------------------------------------------------------------------

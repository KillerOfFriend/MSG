#include "AbstractExecutedCommand.h"

using namespace Core;

//-----------------------------------------------------------------------------
TAbstractExecutedCommand::TAbstractExecutedCommand(QObject *inParent) : TAbstractCommand(inParent)
{
    fCommandType = eCommandType::ctOnExecute; // Устанавливаем тип соманды как исполняемая
}
//-----------------------------------------------------------------------------
TAbstractExecutedCommand::~TAbstractExecutedCommand()
{

}
//-----------------------------------------------------------------------------
/**
 * @brief TAbstractExecutedCommand::interpreted - Метод интерпритации команды, вызывает execute (Метод выполнения)
 * @return Вернёт признак успешности операции выполнения
 */
bool TAbstractExecutedCommand::interpreted()
{ return execute(); }
//-----------------------------------------------------------------------------

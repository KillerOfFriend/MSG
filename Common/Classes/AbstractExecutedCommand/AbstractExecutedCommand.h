#ifndef ABSTRACTEXECUTEDCOMMAND_H
#define ABSTRACTEXECUTEDCOMMAND_H

#include "Classes/AbstractCommand/AbstractCommand.h"

namespace Core
{
    class TAbstractExecutedCommand : public TAbstractCommand
    {
    public:
        TAbstractExecutedCommand(QObject* inParent = nullptr);
        ~TAbstractExecutedCommand();

        virtual bool interpreted() override; // Метод интерпритации вызывает execute (Метод выполнения)

    protected:
        virtual bool execute() = 0; // Абстрактный метод выполнения команды

    };
}

#endif // ABSTRACTEXECUTEDCOMMAND_H

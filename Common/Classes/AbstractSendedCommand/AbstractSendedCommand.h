#ifndef ABSTRACTSENDEDCOMMAND_H
#define ABSTRACTSENDEDCOMMAND_H

#include "Classes/AbstractCommand/AbstractCommand.h"

namespace Core
{
    class TAbstractSendedCommand : public TAbstractCommand
    {
    public:
        TAbstractSendedCommand(QObject* inParent = nullptr);
        ~TAbstractSendedCommand();

        virtual bool interpreted() override; // Метод интерпритации вызывает send (Метод отправки)

    protected:
        virtual bool send() = 0; // Абстрактный метод отправки команды

    };
}

#endif // ABSTRACTSENDEDCOMMAND_H

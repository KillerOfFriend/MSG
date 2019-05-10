#ifndef ABSTRACTAbstractCommand_H
#define ABSTRACTAbstractCommand_H

#include <memory>

#include <QObject>
#include <QByteArray>

#include "comandes.h"

namespace Core
{
    class TAbstractCommand : public QObject
    {
        Q_OBJECT
    public:
        enum class eCommandType // Типы команд
        {
            ctUnknown = 0,      // Не известный тип, не должно такого быть
            ctOnSend = 1,       // Команда на отправку
            ctOnExecute = 2     // Команда на выполнение
        };

        explicit TAbstractCommand(QObject *inParent = nullptr);
        TAbstractCommand(TAbstractCommand &inOther);
        ~TAbstractCommand();

        TAbstractCommand &operator =(const TAbstractCommand &inOther);

        void setCommandCode(quint8 inComandCode); // Метод задаст код команды
        quint8 commandCode(); // Метод вернёт код команды

        void setData(QByteArray &inData); // Метод задаст данные команды
        void setData(std::shared_ptr<QByteArray> inData); // Метод задаст данные команды
        std::shared_ptr<QByteArray> data(); // Метод вернёт данные команды

        eCommandType commandType(); // Метод вернёт тип команды

        virtual bool interpreted() = 0; // Мeтеод интерпритирует команду

    protected:
        quint8 fCommandCode = Commands::Unknown; // Код команды
        eCommandType fCommandType = eCommandType::ctUnknown; // Тип команды, по умолчанию неизвестный
        std::shared_ptr<QByteArray> fCommandData = nullptr; // Данные команды, упакованные в QByteArray
    };
}



#endif // ABSTRACTAbstractCommand_H

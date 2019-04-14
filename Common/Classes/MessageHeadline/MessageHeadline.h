#ifndef MESSAGEHEADLINE_H
#define MESSAGEHEADLINE_H

#include <QObject>
#include <QDataStream>
#include <QVersionNumber>

#include "comandes.h"

namespace Core
{
    class TMessageHeadline : public QObject
    {
        Q_OBJECT
    public:
        explicit TMessageHeadline(QObject *inParent = nullptr);
        TMessageHeadline(quint8 inCommand, QObject *inParent = nullptr);
        TMessageHeadline(const TMessageHeadline &inOther);
        TMessageHeadline();

        TMessageHeadline &operator =(const TMessageHeadline &inOther);

        void setMessageSize(quint64 inMessageSize); // Метод задаст длинну сообщения
        quint64 messageSize() const; // Метод вернёт длинну сообщения

        void setModuleVersion(QVersionNumber inVersion); // Метод задаст версию ПО
        QVersionNumber moduleVersion() const; // Метод вернёт версию ПО

        void setCommand(quint8 inCommand); // Метод задаст сообщению команду
        quint8 command() const; // Метод вернёт команду сообщения

    private:
        quint64 fMsgSize = 0; // Длинна сообщения
        QVersionNumber fVersion; // Верисия софта

        quint8 fCommand = Commands::Unknown; // Команда

        friend QDataStream& operator <<(QDataStream&, const TMessageHeadline&);
        friend QDataStream& operator >>(QDataStream&, TMessageHeadline&);
    };
}



#endif // MESSAGEHEADLINE_H

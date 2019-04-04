#ifndef MESSAGEHEADLINE_H
#define MESSAGEHEADLINE_H

#include <QObject>
#include <QDataStream>
#include <QVersionNumber>

namespace Core
{
    class TMessageHeadline : public QObject
    {
        Q_OBJECT
    public:
        explicit TMessageHeadline(QObject *inParent = nullptr);
        TMessageHeadline(const TMessageHeadline &inOther);
        TMessageHeadline();

        TMessageHeadline &operator =(const TMessageHeadline &inOther);

        void setMessageSize(quint64 inMessageSize); // Метод задаст длинну сообщения
        quint64 messageSize() const; // Метод вернёт длинну сообщения

        void setModuleVersion(QVersionNumber inVersion); // Метод задаст версию ПО
        QVersionNumber moduleVersion(); // Метод вернёт версию ПО

    private:
        quint64 fMsgSize = 0; // Длинна сообщения
        QVersionNumber fVersion; // Верисия софта

        friend QDataStream& operator <<(QDataStream&, const TMessageHeadline&);
        friend QDataStream& operator >>(QDataStream&, TMessageHeadline&);
    };
}



#endif // MESSAGEHEADLINE_H

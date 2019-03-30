#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include <memory>

#include <QUuid>
#include <QObject>
#include <QDateTime>
#include <QByteArray>
#include <QDataStream>

namespace Core
{
    namespace MSGType // Типы сообщений
    {
        static const quint8 Unknown = 0; // Неизвестный тип
        static const quint8 Text = 1;    // Текстовое сообщение
        static const quint8 File = 2;    // Файл
        static const quint8 Image = 3;   // Пикча
    }

    class TChatMessage;

    typedef std::shared_ptr<TChatMessage> ChatMessage_Ptr;

    class TChatMessage : public QObject
    {
        Q_OBJECT
    public:
        explicit TChatMessage(QObject *inParent = nullptr);
        TChatMessage(const TChatMessage &inOther);
        ~TChatMessage();

        TChatMessage &operator =(const TChatMessage &inOther);

        void setMessageUuid(const QUuid inUuid); // Метод задаст Uuid сообщения
        QUuid messageUuid() const; // Метод вернёт Uuid сообщения

        void setMessageTimeCode(const QDateTime inTimeCode); // Метод задаст таймкод сообщения
        QDateTime messageTimeCode() const; // Метод вернёт таймкод сообщения

        void setMessageType(const quint8 inType); // Метод задаст тип сообщения
        quint8 messageType() const; // Метод вернёт тип сообщения

        void setMessageData(const QByteArray &inData); // Метод задаст данные сообщения (текст, файл, картинку...)
        QByteArray messageData() const; // Метод вернёт данные сообщения (текст, файл, картинку...)

        void setMessageSender(const QUuid inSenderUuid); // Метод задаст отпровителя сообщения
        QUuid messageSender() const; // Метод вернёт отпровителя сообщения

        friend QDataStream& operator <<(QDataStream&, const TChatMessage&);
        friend QDataStream& operator >>(QDataStream&, TChatMessage&);

    private:
        QUuid fUuid = QUuid(0); // Uuid сообщения
        QDateTime fTimeCode; // Тaймкод сообщения
        quint8 fType = MSGType::Unknown; // Тип сообщения
        QByteArray fData; // Данные сообщения
        QUuid fSenderUuid = QUuid(0); // Uuid отправителя

    };
}

#endif // CHATMESSAGE_H

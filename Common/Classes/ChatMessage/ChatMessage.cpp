#include "ChatMessage.h"

using namespace Core;

//-----------------------------------------------------------------------------
/**
 * @brief TChatMessage::TChatMessage - Конструктор по умолчанию
 * @param inParent - Предок
 */
TChatMessage::TChatMessage(QObject *inParent) : QObject(inParent)
{

}
//-----------------------------------------------------------------------------
/**
 * @brief TChatMessage - Копирующий конструктор
 * @param inOther - Копируемый объект
 */
TChatMessage::TChatMessage(const TChatMessage &inOther) : QObject(inOther.parent())
{
    this->fUuid = inOther.fUuid;
    this->fTimeCode = inOther.fTimeCode;
    this->fType = inOther.fType;
    this->fData = inOther.fData;
    this->fSenderUuid = inOther.fSenderUuid;
}
//-----------------------------------------------------------------------------
/**
 * @brief TChatMessage::~TChatMessage - Диструктор оп умолчанию
 */
TChatMessage::~TChatMessage()
{
    fData.clear();
}
//-----------------------------------------------------------------------------
/**
 * @brief operator = Опиратор приравнивания
 * @param inOther - Копируемый объект
 * @return Вернёт указатель на новый объект
 */
TChatMessage& TChatMessage::operator =(const TChatMessage &inOther)
{
    if (this == &inOther)
         return *this;

    this->fUuid = inOther.fUuid;
    this->fTimeCode = inOther.fTimeCode;
    this->fType = inOther.fType;
    this->fData = inOther.fData;
    this->fSenderUuid = inOther.fSenderUuid;

    return *this;
}
//-----------------------------------------------------------------------------
/**
 * @brief TChatMessage::setMessageUuid - Метод задаст Uuid сообщения
 * @param inUuid - Новое значение Uuid сообщения
 */
void TChatMessage::setMessageUuid(const QUuid inUuid)
{ fUuid = inUuid; }
//-----------------------------------------------------------------------------
/**
 * @brief TChatMessage::messageUuid - Метод вернёт Uuid сообщения
 * @return Вернёт значение Uuid сообщения
 */
QUuid TChatMessage::messageUuid() const
{ return fUuid; }
//-----------------------------------------------------------------------------
/**
 * @brief TChatMessage::setMessageTimeCode - Метод задаст таймкод сообщения
 * @param inTimeCode - Новое значение таймкода сообщения
 */
void TChatMessage::setMessageTimeCode(const QDateTime inTimeCode)
{ fTimeCode = inTimeCode; }
//-----------------------------------------------------------------------------
/**
 * @brief TChatMessage::messageTimeCode - Метод вернёт таймкод сообщения
 * @return Вернёт значение таймкода сообщения
 */
QDateTime TChatMessage::messageTimeCode() const
{ return fTimeCode; }
//-----------------------------------------------------------------------------
/**
 * @brief TChatMessage::setMessageType - Метод задаст тип сообщения
 * @param inType - Новое значение типа сообщения
 */
void TChatMessage::setMessageType(const quint8 inType)
{ fType = inType; }
//-----------------------------------------------------------------------------
/**
 * @brief TChatMessage::messageType - Метод вернёт тип сообщения
 * @return Вернёт значение типа сообщения
 */
quint8 TChatMessage::messageType() const
{ return fType; }
//-----------------------------------------------------------------------------
/**
 * @brief TChatMessage::setMessageData - Метод задаст данные сообщения (текст, файл, картинку...)
 * @param inData - Новые данные сообщения
 */
void TChatMessage::setMessageData(const QByteArray &inData)
{ fData = inData; }
//-----------------------------------------------------------------------------
/**
 * @brief TChatMessage::messageData - Метод вернёт данные сообщения (текст, файл, картинку...)
 * @return Вернёт данные сообщения
 */
QByteArray TChatMessage::messageData() const
{ return fData; }
//-----------------------------------------------------------------------------
/**
 * @brief TChatMessage::setMessageSender - Метод задаст отпровителя сообщения
 * @param inSenderUuid - Новое значение Uuid отправителя
 */
void TChatMessage::setMessageSender(const QUuid inSenderUuid)
{ fSenderUuid = inSenderUuid; }
//-----------------------------------------------------------------------------
/**
 * @brief TChatMessage::messageSender - Метод вернёт отпровителя сообщения
 * @return Вернёт значение Uuid отправителя
 */
QUuid TChatMessage::messageSender() const
{ return fSenderUuid; }
//-----------------------------------------------------------------------------
namespace Core
{   // Во избежании затупов со стороны компиллера, требуется оборачивать реализацию в тот же неймспейс принудительно
    QDataStream& operator <<(QDataStream &outStream, const TChatMessage &ChatMessage)
    {
        outStream << ChatMessage.fUuid;
        outStream << ChatMessage.fTimeCode;
        outStream << ChatMessage.fType;
        outStream << ChatMessage.fData;
        outStream << ChatMessage.fSenderUuid;

        return outStream;
    }
    //-----------------------------------------------------------------------------
    QDataStream& operator >>(QDataStream &inStream, TChatMessage &ChatMessage)
    {
        inStream >> ChatMessage.fUuid;
        inStream >> ChatMessage.fTimeCode;
        inStream >> ChatMessage.fType;
        inStream >> ChatMessage.fData;
        inStream >> ChatMessage.fSenderUuid;

        return inStream;
    }
}
//-----------------------------------------------------------------------------

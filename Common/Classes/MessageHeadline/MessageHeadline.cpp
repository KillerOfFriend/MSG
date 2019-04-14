#include "MessageHeadline.h"

using namespace Core;

//-----------------------------------------------------------------------------
TMessageHeadline::TMessageHeadline(QObject *inParent) : QObject(inParent)
{
    fVersion = QVersionNumber(1,0,0);
}
//-----------------------------------------------------------------------------
TMessageHeadline::TMessageHeadline(quint8 inCommand, QObject *inParent) : QObject(inParent)
{
   fCommand = inCommand; // Задаём команду
   fVersion = QVersionNumber(1,0,0);
}
//-----------------------------------------------------------------------------
TMessageHeadline::TMessageHeadline(const TMessageHeadline &inOther) : QObject(inOther.parent())
{
    this->fMsgSize = inOther.fMsgSize;
    this->fVersion = inOther.fVersion;
    this->fCommand = inOther.fCommand;
}
//-----------------------------------------------------------------------------
TMessageHeadline::TMessageHeadline()
{
    fMsgSize = 0;
}
//-----------------------------------------------------------------------------
TMessageHeadline& TMessageHeadline::operator =(const TMessageHeadline &inOther)
{
    if (this == &inOther)
         return *this;

    this->fMsgSize = inOther.fMsgSize;
    this->fVersion = inOther.fVersion;
    this->fCommand = inOther.fCommand;

    return *this;
}
//-----------------------------------------------------------------------------
/**
 * @brief TMessageHeadline::setMessageSize - Метод задаст длинну сообщения
 * @param inMessageSize - Новое значение длинны сообщения
 */
void TMessageHeadline::setMessageSize(quint64 inMessageSize)
{ fMsgSize = inMessageSize; }
//-----------------------------------------------------------------------------
/**
 * @brief TMessageHeadline::messageSize - Метод вернёт длинну сообщения
 * @return Вернёт значение длинны сообщения
 */
quint64 TMessageHeadline::messageSize() const
{ return fMsgSize; }
//-----------------------------------------------------------------------------
/**
 * @brief TMessageHeadline::setModuleVersion - Метод задаст версию ПО
 * @param inVersion - Новое значение версии ПО
 */
void TMessageHeadline::setModuleVersion(QVersionNumber inVersion)
{ fVersion = inVersion; }
//-----------------------------------------------------------------------------
/**
 * @brief TMessageHeadline::moduleVersion - Метод вернёт версию ПО
 * @return Вернёт значение версии ПО
 */
QVersionNumber TMessageHeadline::moduleVersion() const
{ return fVersion; }
//-----------------------------------------------------------------------------
/**
 * @brief setCommand -  Метод задаст сообщению команду
 * @param inCommand - Новая команда
 */
void TMessageHeadline::setCommand(quint8 inCommand)
{ fCommand = inCommand; }
//-----------------------------------------------------------------------------
/**
 * @brief TMessageHeadline::command - Метод вернёт команду сообщения
 * @return Вернёт команду сообщения
 */
quint8 TMessageHeadline::command() const
{ return fCommand; }
//-----------------------------------------------------------------------------

namespace Core
{
    // Во избежании затупов со стороны компиллера, требуется оборачивать реализацию в тот же неймспейс принудительно
    QDataStream& operator <<(QDataStream &outStream, const TMessageHeadline &inMessageHeadline)
    {
        outStream << inMessageHeadline.fMsgSize;
        outStream << inMessageHeadline.fVersion;
        outStream << inMessageHeadline.fCommand;

        return outStream;
    }
    //-----------------------------------------------------------------------------
    QDataStream& operator >>(QDataStream &inStream, TMessageHeadline &inMessageHeadline)
    {
        inStream >> inMessageHeadline.fMsgSize;
        inStream >> inMessageHeadline.fVersion;
        inStream >> inMessageHeadline.fCommand;

        return inStream;
    }
}


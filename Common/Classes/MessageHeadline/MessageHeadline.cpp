#include "MessageHeadline.h"

using namespace Core;

//-----------------------------------------------------------------------------
TMessageHeadline::TMessageHeadline(QObject *inParent) : QObject(inParent)
{

}
//-----------------------------------------------------------------------------
TMessageHeadline::TMessageHeadline(const TMessageHeadline &inOther) : QObject(inOther.parent())
{
    this->fMsgSize = inOther.fMsgSize;
    this->fVersion = inOther.fVersion;
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
QVersionNumber TMessageHeadline::moduleVersion()
{ return fVersion; }
//-----------------------------------------------------------------------------


namespace Core
{
    // Во избежании затупов со стороны компиллера, требуется оборачивать реализацию в тот же неймспейс принудительно
    QDataStream& operator <<(QDataStream &outStream, const TMessageHeadline &inTMessageHeadline)
    {
        outStream << inTMessageHeadline.fMsgSize;
        outStream << inTMessageHeadline.fVersion;

        return outStream;
    }
    //-----------------------------------------------------------------------------
    QDataStream& operator >>(QDataStream &inStream, TMessageHeadline &inTMessageHeadline)
    {
        inStream >> inTMessageHeadline.fMsgSize;
        inStream >> inTMessageHeadline.fVersion;

        return inStream;
    }
}


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


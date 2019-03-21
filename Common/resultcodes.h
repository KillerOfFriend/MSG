#ifndef RESULTCODES_H
#define RESULTCODES_H

#include <QtGlobal>
#include <limits>

namespace Res
{
    const qint32 rUnknown = 0;
    const qint32 rTimeOut = std::numeric_limits<quint32>::max();

    namespace CreateClient
    {
        const qint32 ccCreated = 1;
        const qint32 ccAlredyExist = 2;
    }

    namespace CanAut
    {
        const qint32 caAuthorizationTrue = 1;
        const qint32 caIncorrectPass = 2;
        const qint32 caAuthorizationFalse = 3;
        const qint32 caUserInfoError = 4;
    }

    namespace GetUserTypes
    {
        const qint32 gtOK = 1;
        const qint32 gtFail = 2;
    }

    namespace FindUsers
    {
        const qint32 fuUsersFound = 1;
        const qint32 fuUsersNotFound = 2;
    }

    namespace AddContact
    {
        const qint32 acCreated = 1;
        const qint32 acAlredyExist = 2;
    }

//    namespace GetContacts
//    {
//        const qint32 gcUsersFound = 1;
//        const qint32 gcUsersNotFound = 2;
//    }

    namespace DeleteContact
    {
        const qint32 dcContactRemove = 1;
        const qint32 dcContactNotFound = 2;
    }

    namespace CreateChat
    {
        const qint32 ccCreated = 1;
        const qint32 ccAlredyExist = 2;
    }

    namespace AddUsetToChat
    {
        const qint32 autcAdded = 1;
        const qint32 autcAlredyExist = 2;
    }
}

#endif // RESULTCODES_H

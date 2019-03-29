#ifndef RESULTCODES_H
#define RESULTCODES_H

#include <QtGlobal>
#include <limits>

namespace Res
{
    const quint8 rUnknown = 0;
    const quint8 rTimeOut = std::numeric_limits<quint8>::max();

    namespace CreateClient
    {
        const quint8 ccCreated = 1;
        const quint8 ccAlredyExist = 2;
    }

    namespace CanAut
    {
        const quint8 caAuthorizationTrue = 1;
        const quint8 caIncorrectPass = 2;
        const quint8 caAuthorizationFalse = 3;
        const quint8 caUserInfoError = 4;
        const quint8 caUserAlredyOnline = 5;
    }

    namespace GetUserTypes
    {
        const quint8 gtOK = 1;
        const quint8 gtFail = 2;
    }

    namespace FindUsers
    {
        const quint8 fuUsersFound = 1;
        const quint8 fuUsersNotFound = 2;
    }

    namespace AddContact
    {
        const quint8 acCreated = 1;
        const quint8 acAlredyExist = 2;
    }

    namespace DeleteContact
    {
        const quint8 dcContactRemove = 1;
        const quint8 dcContactNotFound = 2;
    }

    namespace CreateChat
    {
        const quint8 ccCreated = 1;
        const quint8 ccAlredyExist = 2;
    }

    namespace AddUserToChat
    {
        const quint8 autcAdded = 1;
        const quint8 autcAlredyExist = 2;
    }

    namespace DeleteUserFromChat
    {
        const quint8 dufcSuccess = 1;
        const quint8 dufcFail = 2;
        const quint8 dufcNotInside = 3;
    }

    namespace ILeaveFromChat
    {   // Выход = удаление из беседы, поэтому присваиваем значения
        const quint8 lfcSuccess = DeleteUserFromChat::dufcSuccess;
        const quint8 lfcFail = DeleteUserFromChat::dufcFail;
    }

    namespace DeleteChat
    {
        const quint8 dcSuccess = 1;
        const quint8 dcNotFound = 2;
    }
}

#endif // RESULTCODES_H

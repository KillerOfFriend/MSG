#ifndef COMANDES_H
#define COMANDES_H

#include <qglobal.h>

namespace Commands
{
    const quint8 Unknown =          0; // Не корректная команда
    const quint8 CreateUser =       1; // Регистрация пользователя
    const quint8 Authorization =    2; // Авторизация
    const quint8 FindUsers =        3; // Поиск пользователей
    const quint8 AddContact =       4; // Добавление контакта
    const quint8 GetContacts =      5; // Запрос списка контактов
}

#endif // COMANDES_H

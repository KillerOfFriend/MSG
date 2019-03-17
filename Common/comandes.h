#ifndef COMANDES_H
#define COMANDES_H

#include <QtGlobal>

namespace Commands
{
    const quint8 Unknown =              0; // Не корректная команда
    const quint8 CreateUser =           1; // Регистрация пользователя
    const quint8 Authorization =        2; // Авторизация
    const quint8 GetUserTypes =         3; // Запрос списка типов пользователей
    const quint8 FindUsers =            4; // Поиск пользователей
    const quint8 AddContact =           5; // Добавление контакта
//    const quint8 GetContacts =          6; // Запрос списка контактов
    const quint8 DeleteContact =        7; // Удаление контакта
    const quint8 ContactChangeStatus =  8; // Контакт изменил свой статус
}

#endif // COMANDES_H

#ifndef COMANDES_H
#define COMANDES_H

#include <QtGlobal>

namespace Commands
{
    const quint8 Unknown =              0;  // Не корректная команда
    const quint8 CreateUser =           1;  // Регистрация пользователя
    const quint8 Authorization =        2;  // Авторизация
    const quint8 GetUserTypes =         3;  // Запрос списка типов пользователей
    const quint8 FindUsers =            4;  // Поиск пользователей
    const quint8 AddContact =           5;  // Добавление контакта
    const quint8 DeleteContact =        6;  // Удаление контакта
    const quint8 ContactChangeStatus =  7;  // Контакт изменил свой статус
    const quint8 CreateChat =           8;  // Создание новой беседы
    const quint8 InviteToChat =         9;  // Приглашение в новый чат
    const quint8 AddUserToChat =        10; // Добавление пользователя в беседу
    const quint8 ILeaveFromChat =       11; // Выход пользователя из чата
    const quint8 DeleteUserFromChat =   12; // Удаление пользователя из беседы
}

#endif // COMANDES_H

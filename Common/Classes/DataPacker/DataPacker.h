#ifndef DATAPACKER_H
#define DATAPACKER_H

#include <QObject>
#include <QDataStream>
#include <QDebug>

#include "Classes/MessageHeadline/MessageHeadline.h"

namespace Core
{
    class TDataPacker : public QObject
    {
        Q_OBJECT
    public:
        explicit TDataPacker(QObject *inParent = nullptr);
        ~TDataPacker();

        template<typename... Args> bool makePackage(const quint8 &inCommand, QDataStream &inStream, const Args... inArgs) // Метод упаковывает все поданные параметры в поток
        {
            // Для начала нужно расчитать размер сообщения что бы записать его в заголовок
            // Для этого формируем сообщение с пустым заголовком

            bool Result = true; // Флаг успешности выполнения
            TMessageHeadline MessageHandel(inCommand, this); // Создаём заголовок без указания размера сообщения

            Result &= writer(inStream, MessageHandel); // Пишем заголовок во временный поток
            auto Ress = { writer(inStream, inArgs)... }; // Распоковываем все параметры inArgs во временный поток

            // Сообщение в потоке сформировано
            MessageHandel.setMessageSize(inStream.device()->size()); // Задаём полученный размер сообщения
            inStream.device()->seek(0); // Возвращаемся в самое начало потока
            Result &= writer(inStream, MessageHandel); // Пишем заголовок во временный поток (Поверх старого)
            //qDebug() << "[ЗАПАКОВКА ЗАВЕРШЕНА]" << inStream.device()->size() << inCommand;

            for ( const bool& Res : Ress ) // Проверяем удалось ли записать все параметры
            { Result &= Res; }

            return Result; // Возвращаем флаг успешности операции
        }

    private:
        template<typename T> bool writer(QDataStream &inStream, const T &inValue) // Метод запишет полученный inValue в поток inStream
        {
            bool Result = true; // Флаг успешности выполнения

            try
            {
                if (std::is_same<T, float>::value) // Если тип float
                    inStream.setFloatingPointPrecision(QDataStream::SinglePrecision); // Устанавливаем точность
                else
                if (std::is_same<T, double>::value) // Если тип double
                    inStream.setFloatingPointPrecision(QDataStream::DoublePrecision); // Устанавливаем точность

                inStream << inValue; // Пишим данные в поток
            }
            catch (...) // Если произошла ошибка
            { Result = false; } // Сбрасываем флаг

            return Result; // Возвращаем флаг успешности операции
        }
    };
}

#endif // DATAPACKER_H

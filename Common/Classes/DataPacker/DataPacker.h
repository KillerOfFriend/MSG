#ifndef DATAPACKER_H
#define DATAPACKER_H

#include <QDebug>
#include <QObject>
#include <QByteArray>
#include <QDataStream>

#include "Classes/MessageHeadline/MessageHeadline.h"

namespace Core
{
    class TDataPacker : public QObject
    {
        Q_OBJECT
    public:
        explicit TDataPacker(QObject *inParent = nullptr);
        ~TDataPacker();

        //--
        template<typename... Args>
        bool makeData(QByteArray &outData, const Args... inArgs) // Метод упаковывает все поданные параметры в поток БЕЗ ЗАГОЛОВКА
        {
            QDataStream Stream(&outData, QIODevice::WriteOnly); // Оборачиваем данные в поток
            return makeData(Stream, inArgs...); // Запаковываем
        }
        //--
        template<typename... Args>
        bool makeData(QDataStream &outStream, const Args... inArgs) // Метод упаковывает все поданные параметры в поток БЕЗ ЗАГОЛОВКА
        {
            if (!outStream.device())
                return false;

            bool Result = true; // Флаг успешности выполнения
            auto Ress = { writer(outStream, inArgs)... }; // Распоковываем все параметры inArgs во временный поток

            for ( const bool& Res : Ress ) // Проверяем удалось ли записать все параметры
            { Result &= Res; }

            return Result; // Возвращаем флаг успешности операции
        }
        //----
        template<typename... Args>
        bool makePackage(const quint8 &inCommand, QByteArray &outData, const Args... inArgs) // Метод упаковывает все поданные параметры в поток И ДОБАВИТ ЗАГОЛОВОК В НАЧАЛО
        {
            QDataStream Stream(&outData, QIODevice::WriteOnly); // Оборачиваем данные в поток
            return makePackage(inCommand, Stream, inArgs...); // Запаковываем
        }
        //--
        template<typename... Args>
        bool makePackage(const quint8 &inCommand, QDataStream &outStream, const Args... inArgs) // Метод упаковывает все поданные параметры в поток И ДОБАВИТ ЗАГОЛОВОК В НАЧАЛО
        {
            if (!outStream.device())
                return false;

            // Для начала нужно расчитать размер сообщения что бы записать его в заголовок
            // Для этого формируем сообщение с пустым заголовком

            bool Result = true; // Флаг успешности выполнения
            TMessageHeadline MessageHandel(inCommand, this); // Создаём заголовок без указания размера сообщения

            Result &= writer(outStream, MessageHandel); // Пишем заголовок во временный поток
            auto Ress = { writer(outStream, inArgs)... }; // Распоковываем все параметры inArgs во временный поток

            // Сообщение в потоке сформировано
            MessageHandel.setMessageSize(outStream.device()->size()); // Задаём полученный размер сообщения
            outStream.device()->seek(0); // Возвращаемся в самое начало потока
            Result &= writer(outStream, MessageHandel); // Пишем заголовок во временный поток (Поверх старого)
            //qDebug() << "[ЗАПАКОВКА ЗАВЕРШЕНА]" << outStream.device()->size() << inCommand;

            for ( const bool& Res : Ress ) // Проверяем удалось ли записать все параметры
            { Result &= Res; }

            return Result; // Возвращаем флаг успешности операции
        }
        //--
    private:
        template<typename T>
        bool writer(QDataStream &outStream, const T &inValue) // Метод запишет полученный inValue в поток outStream
        {
            bool Result = true; // Флаг успешности выполнения

            try
            {
                if (std::is_same<T, float>::value) // Если тип float
                    outStream.setFloatingPointPrecision(QDataStream::SinglePrecision); // Устанавливаем точность
                else
                if (std::is_same<T, double>::value) // Если тип double
                    outStream.setFloatingPointPrecision(QDataStream::DoublePrecision); // Устанавливаем точность

                outStream << inValue; // Пишим данные в поток
            }
            catch (...) // Если произошла ошибка
            { Result = false; } // Сбрасываем флаг

            return Result; // Возвращаем флаг успешности операции
        }
    };
}

#endif // DATAPACKER_H

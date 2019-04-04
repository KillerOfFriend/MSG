#include "Connection.h"

#include "Classes/MessageHeadline/MessageHeadline.h"

using namespace Core;

//-----------------------------------------------------------------------------
TConnection::TConnection(QObject *inParent) : QObject(inParent)
{

}
//-----------------------------------------------------------------------------
TConnection::TConnection(const TConnection &inOther) : QObject(inOther.parent())
{
    this->fSocket = inOther.fSocket;
    this->fIsReading = inOther.fIsReading;
    this->fSize = inOther.fSize;
    this->fData = inOther.fData;
}
//-----------------------------------------------------------------------------
TConnection::~TConnection()
{
    fIsReading = false;
    fSize = 0;
    fData.clear();
}
//-----------------------------------------------------------------------------
TConnection& TConnection::operator =(const TConnection &inOther)
{
    if (this == &inOther)
         return *this;

    this->fSocket = inOther.fSocket;
    this->fIsReading = inOther.fIsReading;
    this->fSize = inOther.fSize;
    this->fData = inOther.fData;

    return *this;
}
//-----------------------------------------------------------------------------
/**
 * @brief TConnection::setSocket - Мтод задаст сокет соединения
 * @param inSocket - Указатель на новый сокет соединения
 */
void TConnection::setSocket(QTcpSocket* inSocket)
{
    if (!fIsReading) // Только если не идёт чтение
        fSocket = inSocket;
}
//-----------------------------------------------------------------------------
/**
 * @brief TConnection::socket - Метод вернёт сокет соединения
 * @return
 */
QTcpSocket* TConnection::socket()
{ return fSocket; }
//-----------------------------------------------------------------------------
bool TConnection::isReading() // Метод вернёт флаг чтения (идёт чтение \ не идёт чтение)
{ return fIsReading; }
//-----------------------------------------------------------------------------
void TConnection::freeData(bool inForcibly) // Метод очистит уже прочитанные данные соеденения
{
    if (fIsReading && !inForcibly) // Если чтение идёт но флаг принудительного удаления не взведён
        return; // Ничего не делать

    fSize = 0;
    fData.clear();
    fIsReading = false;
}
//-----------------------------------------------------------------------------
void TConnection::readData() // Метод читает данные из сокета
{
    if (!fSocket)
        return;

    if (!isReading()) // Если чтение ещё не начато
    {
        freeData(true); // Вызываем очищение (принудительно)

        QDataStream inStream(fSocket); // Оборачиваем сокет в поток
        TMessageHeadline MessageHeadline(this); // Создаём экземпляр заголовка сообщения

        inStream >> MessageHeadline; // Читаем заголовок сообщения

        if (MessageHeadline.messageSize() == 0) // Если резмер полученного сообщение 0
            sig_ComandCorupted(); // Шлём сигнал о некорректной команде
        else // Заголовак получен
        {
            fIsReading = true; // Взводим флаг чтения
            fSize = MessageHeadline.messageSize(); // Запоминаем размер сообщения
            fData = fSocket->readAll(); // Читаем постепившие данные
        }

        if (checkReadFinihs()) // Если чтение завершено
        {
            sig_ComandReadyToExecute(fData); // Шлём сигнал о завершении чтения и поток данных на выполнение
            fIsReading = false;
        }
    }
    else // Если чтение уже начато
    {
        fData += fSocket->readAll(); // Читаем поступившие данные

        if (checkReadFinihs()) // Если чтение завершено
        {
            sig_ComandReadyToExecute(fData); // Шлём сигнал о завершении чтения и поток данных на выполнение
            fIsReading = false;
        }
    }
}
//-----------------------------------------------------------------------------
bool TConnection::checkReadFinihs() // Метод проверит завершилось ли соедениение
{ return fSize >= fData.size(); }
//-----------------------------------------------------------------------------

#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>
#include <QDataStream>

namespace Core
{
    class TConnection : public QObject
    {
        Q_OBJECT
    public:
        explicit TConnection(QObject *inParent = nullptr);
        TConnection(const TConnection &inOther);
        ~TConnection();

        TConnection &operator =(const TConnection &inOther);

        void setSocket(QTcpSocket* inSocket); // Мтод задаст сокет соединения
        QTcpSocket* socket(); // Метод вернёт сокет соединения

        QByteArray* socketData(); // Метод вернёт указатель на считанные данные

        bool isReading(); // Метод вернёт флаг чтения (идёт чтение \ не идёт чтение)
        void freeData(bool inForcibly = false); // Метод очистит уже прочитанные данные соеденения
        void readData(); // Метод читает данные из сокета

    private:
        QTcpSocket* fSocket = nullptr; // Сокет соединения

        bool fIsReading = false; // Флаг чтения данных
        quint64 fSize = 0; // Размер читаемого сообщения

        QByteArray fData; // Считанне данные

        bool checkReadFinihs(); // Метод проверит завершилось ли соедениение

    signals:
        void sig_ComandReadyToExecute(QTcpSocket* &inSocket); // Сигнал, сообщающий о завершении чтения и готовности команды к выполнению
        void sig_ComandCorupted(); // Сигнал, сообщающий о получении повреждённой команды
    };
}

#endif // CONNECTION_H

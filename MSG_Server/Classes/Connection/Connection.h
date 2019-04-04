#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QByteArray>

class TConnection : public QObject
{
    Q_OBJECT
public:
    explicit TConnection(QObject *inParent = nullptr);
    ~TConnection();



private:
    bool fIsReading = false; // Флаг чтения данных
    quint64 fSize = 0; // Размер читаемого сообщения

    QByteArray fData; // Считанне данные

};

#endif // CONNECTION_H

#ifndef DATAMODULE_H
#define DATAMODULE_H

#include <memory>

#include <QObject>
#include <QString>
#include <QApplication>
#include <QSqlDatabase>

#include "Classes/MSGServer/MSGServer.h"
#include "Models/ServerLogModel/ServerLogModel.h"

class TDM : public QObject
{
    Q_OBJECT
public:
    static TDM& Instance();
    QSqlDatabase& DB(); // Метод вернёт ссылку на класс базы данных
    std::shared_ptr<TMSGServer> Server(); // Метод вернёт указатель на объект-сервер
    std::shared_ptr<TServerLogModel> LogModel(); // Метод вернёт указатель на модель логов

private:
    QSqlDatabase fDB; /// Объект базы данных
    std::shared_ptr<TMSGServer> fServer = nullptr;
    std::shared_ptr<TServerLogModel> fLogModel = nullptr;

    explicit TDM(QObject *inParent = nullptr);
    ~TDM();

    TDM(TDM const&) = delete;
    TDM& operator= (TDM const&) = delete;

    void init(); /// Мотод инициализирует классы модуля данных
    void initDB(); /// Метод инициализирует базу данных

signals:

public slots:

};


#endif // DATAMODULE_H

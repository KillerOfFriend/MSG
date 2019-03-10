#ifndef DATAMODULE_H
#define DATAMODULE_H

#include <memory>

#include <QObject>
#include <QString>

#include "Classes/MSGServer/MSGServer.h"
#include "Models/ServerLogModel/ServerLogModel.h"

class TDM : public QObject
{
    Q_OBJECT
public:
    static TDM& Instance();

    std::shared_ptr<TMSGServer> Server(); // Метод вернёт указатель на объект-сервер
    std::shared_ptr<TServerLogModel> LogModel(); // Метод вернёт указатель на модель логов

private:
    std::shared_ptr<TMSGServer> fServer = nullptr;
    std::shared_ptr<TServerLogModel> fLogModel = nullptr;

    explicit TDM(QObject *inParent = nullptr);
    ~TDM();

    TDM(TDM const&) = delete;
    TDM& operator= (TDM const&) = delete;

    void init(); /// Мотод инициализирует классы модуля данных

signals:

public slots:

};


#endif // DATAMODULE_H

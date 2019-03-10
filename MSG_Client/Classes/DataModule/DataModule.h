#ifndef DATAMODULE_H
#define DATAMODULE_H

#include <memory>

#include <QObject>
#include <QThread>

#include "Classes/MSGClient/MSGClient.h"
#include "Classes/UserAccount/UserAccount.h"
#include "Classes/FrameController/FrameController.h"

class TDM : public QObject
{
    Q_OBJECT
public:
    static TDM& Instance();

    std::shared_ptr<TMSGClient> Client();
    std::shared_ptr<TUserAccount> UserAccount();
    std::shared_ptr<TFrameController> FrameController();

private:
    explicit TDM(QObject *inParent = nullptr);
    ~TDM();

    std::shared_ptr<TMSGClient> fClient = nullptr;
    std::shared_ptr<TUserAccount> fUserAccount = nullptr;
    std::shared_ptr<TFrameController> fFrameController = nullptr;


    TDM(TDM const&) = delete;
    TDM& operator= (TDM const&) = delete;

    void init(); /// Мотод инициализирует классы модуля данных

signals:

public slots:

};


#endif // DATAMODULE_H

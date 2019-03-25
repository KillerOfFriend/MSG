#ifndef DATAMODULE_H
#define DATAMODULE_H

#include <memory>

#include <QObject>
#include <QThread>

#include "Classes/MSGClient/MSGClient.h"
#include "Classes/UserAccount/UserAccount.h"
#include "Classes/FrameController/FrameController.h"
#include "Classes/Models/Models.h"

class TDM : public QObject
{
    Q_OBJECT
public:
    static TDM& Instance();

    std::shared_ptr<TMSGClient> Client();
    std::shared_ptr<Users::TUserAccount> UserAccount();
    std::shared_ptr<TFrameController> FrameController();
    std::shared_ptr<TModels> Models();

private:
    explicit TDM(QObject *inParent = nullptr);
    ~TDM();

    std::shared_ptr<TMSGClient> fClient = nullptr;
    std::shared_ptr<Users::TUserAccount> fUserAccount = nullptr;
    std::shared_ptr<TFrameController> fFrameController = nullptr;
    std::shared_ptr<TModels> fModels = nullptr;

    TDM(TDM const&) = delete;
    TDM& operator= (TDM const&) = delete;

    void init(); /// Мотод инициализирует классы модуля данных

signals:

public slots:
    void slot_SetUserAccount(Users::TUserAccount &inUserAccount);

};


#endif // DATAMODULE_H

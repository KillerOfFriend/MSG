#include "ClientMainWindow.h"
#include "ConnectDialog.h"

#include <QApplication>
#include <QDialogButtonBox>

#include "Classes/Settings/Settings.h"
#include "Classes/DataModule/DataModule.h"

int main(int argc, char *argv[])
{
    int WorkResult = 0;
    QApplication a(argc, argv);

    TDM &DM = TDM::Instance();

    if (!DM.Client())
    {
        WorkResult = 0;
        qDebug() << "[ФАТАЛЬНАЯ ОШИБКА] Провал инициализации. Аварийное завершение.";
    }
    else
    {   // Выводим окно авторизации
        TConnectDialog ConnectDialog;
        ConnectDialog.setWindowModality(Qt::WindowModal);
        ConnectDialog.exec();

        if (!DM.Client()->isConnected())
        {
            WorkResult = 0;
            qDebug() << "[ФАТАЛЬНАЯ ОШИБКА] Авторизация не пройдена. Аварийное завершение.";
        }
        else
        {
            qDebug() << "[УСПЕХ] Готов к работе.";

            TClientMainWindow w;
            w.show();
            WorkResult = a.exec();
        }
    }

    return WorkResult;
}

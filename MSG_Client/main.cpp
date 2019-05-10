#include "ClientMainWindow.h"

#include <QApplication>
#include <QDialogButtonBox>

#include "Dialogs/ConnectDialog/ConnectDialog.h"
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
    {
        QIcon AppIcon;

        #ifdef Q_OS_WIN32
        AppIcon = QIcon(":/Resurse/Ico/Images/Ico/Icons8-Windows-8-Network-Windows-Client.ico");
        #endif

        #ifdef Q_OS_UNIX
        AppIcon = QIcon(":/Resurse/Ico/Images/Ico/Icons8-Windows-8-Network-Linux-Client.ico");
        #endif

        #ifdef Q_OS_MACX
        AppIcon = QIcon(":/Resurse/Ico/Images/Ico/Icons8-Windows-8-Network-Mac-Client.ico");
        #endif


        // Выводим окно авторизации
        TConnectDialog ConnectDialog;
        ConnectDialog.setWindowIcon(AppIcon);
        ConnectDialog.setWindowModality(Qt::WindowModal);
        ConnectDialog.exec();

        if (!DM.Client()->isConnected())
        {
            WorkResult = 0;
            qDebug() << "[ФАТАЛЬНАЯ ОШИБКА] Авторизация не пройдена. Аварийное завершение.";
        }
        else
        {
            //Дополнительная инициализация
            if (!DM.Client()->getUserTypes())
            {
                WorkResult = 0;
                qDebug() << "[ФАТАЛЬНАЯ ОШИБКА] Не удалось запросить списток типов пользователей. Аварийное завершение.";
            }
            else
            {
                qDebug() << "[УСПЕХ] Готов к работе.";

                TClientMainWindow w;
                w.setWindowIcon(AppIcon);
                w.show();
                WorkResult = a.exec();
            }
        }
    }

    return WorkResult;
}

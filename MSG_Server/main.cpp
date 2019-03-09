#include "ServerMainWindow.h"
#include <QApplication>
#include "Classes/DataModule/DataModule.h"

int main(int argc, char *argv[])
{
    int WorkResult = 0;
    QApplication a(argc, argv);

    TDM& DM = TDM::Instance(); // Инициализируем модель данных

    if ( !DM.DB().isOpen() || ( !DM.Server() || !DM.Server()->isActive() ) )
    {
        qDebug() << "[ФАТАЛЬНАЯ ОШИБКА] Аварийное завершение.";
        WorkResult = 0;
    }
    else
    {
        qDebug() << "[УСПЕХ] Готов к работе.";

        TServerMainWindow w;
        w.show();

        WorkResult = a.exec();
    }

    return WorkResult;
}

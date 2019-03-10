#include "ServerMainWindow.h"
#include <QApplication>

#include "Classes/DB/DB.h"
#include "Classes/DataModule/DataModule.h"

int main(int argc, char *argv[])
{
    int WorkResult = 0;
    QApplication a(argc, argv);

    TDB& DB = TDB::Instance(); // Инициализируем базу данных
    TDM& DM = TDM::Instance(); // Инициализируем модуль данных

    if ( !DB.DB().isOpen() || ( !DM.Server() || !DM.Server()->isActive() ) )
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

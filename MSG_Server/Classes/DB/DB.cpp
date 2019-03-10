#include "DB.h"

#include <QDebug>
#include <QFile>
#include <QApplication>

#include "Classes/Settings/Settings.h"

//-----------------------------------------------------------------------------
TDB::TDB(QObject *inParent) : QObject(inParent)
{
    initDB(); // Инициализируем класс БД
}
//-----------------------------------------------------------------------------
TDB::~TDB()
{
    fDB.close();
    qDebug() << "[ИНФО] Соединение с базой данных разорвано.";
}
//-----------------------------------------------------------------------------
TDB& TDB::Instance()
{
    static TDB DM;
    return DM;
}
//-----------------------------------------------------------------------------
QSqlDatabase& TDB::DB() // Метод вернёт ссылку на класс базы данных
{ return fDB; }
//-----------------------------------------------------------------------------
void TDB::initDB() // Метод инициализирует базу данных
{
//    if (!QFile::exists(QApplication::applicationDirPath() + "/Settings.ini"))
//    {
//        QFile SettingsFile(QApplication::applicationDirPath() + "/Settings.ini");
//        SettingsFile.open(QIODevice::WriteOnly);
//        SettingsFile.close();

//        initDB();
//    }
//    else
//    {
//        QSettings Settings(QApplication::applicationDirPath() + "/Settings.ini", QSettings::IniFormat);

//        if (!Settings.contains("DATABASE/ConnectTypeDB"))
//            Settings.setValue("DATABASE/ConnectTypeDB", "QPSQL");
//        fDB = QSqlDatabase::addDatabase( Settings.value("DATABASE/ConnectTypeDB", "QPSQL").toString() );

//        if (!Settings.contains("DATABASE/HostName"))
//            Settings.setValue("DATABASE/HostName", "127.0.0.1");
//        fDB.setHostName( Settings.value("DATABASE/HostName", "127.0.0.1").toString() );

//        if (!Settings.contains("DATABASE/PortDB"))
//            Settings.setValue("DATABASE/PortDB", "5432");
//        fDB.setPort( Settings.value("DATABASE/PortDB", "5432").toInt() );

//        if (!Settings.contains("DATABASE/NameDB"))
//            Settings.setValue("DATABASE/NameDB", "MSGDB");
//        fDB.setDatabaseName( Settings.value("DATABASE/NameDB", "MSGDB").toString() );

//        if (!Settings.contains("DATABASE/UserName"))
//            Settings.setValue("DATABASE/UserName", "postgres");
//        fDB.setUserName( Settings.value("DATABASE/UserName", "postgres").toString() );

//        if (!Settings.contains("DATABASE/UserPassword"))
//            Settings.setValue("DATABASE/UserPassword", "postgres");
//        fDB.setPassword( Settings.value("DATABASE/UserPassword", "postgres").toString() );

//        Settings.sync();
    TSettings& Settings = TSettings::Instance();

    fDB = QSqlDatabase::addDatabase( Settings.dataBaseType() ); // Устанавливаем тип БД
    fDB.setHostName( Settings.dataBaseHost().toString() );      // Задаём хост БД
    fDB.setPort( Settings.dataBasePort() );                     // Задаём порт БД
    fDB.setDatabaseName( Settings.dataBaseName() );             // Задаём имя БД
    fDB.setUserName( Settings.dataBaseUser() );                 // Задаём пользователя БД
    fDB.setPassword( Settings.dataBasePassword() );             // Задаём пароль

    if (!fDB.open())
        qDebug() << "[ОШИБКА] Не удалось получить доступ к базе данных!";
    else
        qDebug() << "[УСПЕХ] Доступ к базе данных получен.";
//    }
}
//-----------------------------------------------------------------------------

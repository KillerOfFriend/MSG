#include "DataModule.h"
#include <QFile>
#include <QDebug>
#include <QSettings>

//-----------------------------------------------------------------------------
TDM::TDM(QObject *inParent) : QObject(inParent)
{
    init();
}
//-----------------------------------------------------------------------------
TDM::~TDM()
{
    fDB.close();
    qDebug() << "[ИНФО] Соединение с базой данных разорвано.";
}
//-----------------------------------------------------------------------------
TDM& TDM::Instance()
{
    static TDM DM;
    return DM;
}
//-----------------------------------------------------------------------------
QSqlDatabase& TDM::DB() // Метод вернёт ссылку на класс базы данных
{ return fDB; }
//-----------------------------------------------------------------------------
std::shared_ptr<TMSGServer> TDM::Server() // Метод вернёт указатель на объект-сервер
{ return fServer; }
//-----------------------------------------------------------------------------
std::shared_ptr<TServerLogModel> TDM::LogModel() // Метод вернёт указатель на модель логов
{ return fLogModel; }
//-----------------------------------------------------------------------------
void TDM::init() // Мотод инициализирует классы модуля данных
{
    initDB();

    fServer.reset(new TMSGServer(this));
    fLogModel.reset(new TServerLogModel(this));

    connect(fServer.get(), &TMSGServer::sig_LogMessage, fLogModel.get(), &TServerLogModel::slot_NewMessage);
}
//-----------------------------------------------------------------------------
void TDM::initDB() // Метод инициализирует базу данных
{
    if (!QFile::exists(QApplication::applicationDirPath() + "/Settings.ini"))
    {
        QFile SettingsFile(QApplication::applicationDirPath() + "/Settings.ini");
        SettingsFile.open(QIODevice::WriteOnly);
        SettingsFile.close();

        initDB();
    }
    else
    {
        QSettings Settings(QApplication::applicationDirPath() + "/Settings.ini", QSettings::IniFormat);

        if (!Settings.contains("DATABASE/ConnectTypeDB"))
            Settings.setValue("DATABASE/ConnectTypeDB", "QPSQL");
        fDB = QSqlDatabase::addDatabase( Settings.value("DATABASE/ConnectTypeDB", "QPSQL").toString() );

        if (!Settings.contains("DATABASE/HostName"))
            Settings.setValue("DATABASE/HostName", "127.0.0.1");
        fDB.setHostName( Settings.value("DATABASE/HostName", "127.0.0.1").toString() );

        if (!Settings.contains("DATABASE/PortDB"))
            Settings.setValue("DATABASE/PortDB", "5432");
        fDB.setPort( Settings.value("DATABASE/PortDB", "5432").toInt() );

        if (!Settings.contains("DATABASE/NameDB"))
            Settings.setValue("DATABASE/NameDB", "MSGDB");
        fDB.setDatabaseName( Settings.value("DATABASE/NameDB", "MSGDB").toString() );

        if (!Settings.contains("DATABASE/UserName"))
            Settings.setValue("DATABASE/UserName", "postgres");
        fDB.setUserName( Settings.value("DATABASE/UserName", "postgres").toString() );

        if (!Settings.contains("DATABASE/UserPassword"))
            Settings.setValue("DATABASE/UserPassword", "postgres");
        fDB.setPassword( Settings.value("DATABASE/UserPassword", "postgres").toString() );

        Settings.sync();

        if (!fDB.open())
            qDebug() << "[ОШИБКА] Не удалось получить доступ к базе данных!";
        else
            qDebug() << "[УСПЕХ] Доступ к базе данных получен.";
    }
}
//-----------------------------------------------------------------------------

#include "Settings.h"
#include <QFile>

//-----------------------------------------------------------------------------
TSettings::TSettings(QObject *inParent) : QObject(inParent)
{ init(); }
//-----------------------------------------------------------------------------
TSettings::~TSettings()
{
    if (fSettings)
        fSettings->sync();
}
//-----------------------------------------------------------------------------
TSettings& TSettings::Instance()
{
    static TSettings Inst;
    return Inst;
}
//-----------------------------------------------------------------------------
void TSettings::setServerAddress(QHostAddress inServerAddress) /// Метод задаст адрес сервера
{
    if (fSettings)
        fSettings->setValue("SERVER/Host", inServerAddress.toString());
}
//-----------------------------------------------------------------------------
QHostAddress TSettings::ServerAddress() /// Метод вернёт адрес сервера
{
    QHostAddress Result;

    if (!fSettings)
        Result = QHostAddress();
    else
    {
        if (!fSettings->contains("SERVER/Host"))
            fSettings->setValue("SERVER/Host", "127.0.0.1");

        Result = QHostAddress(fSettings->value("SERVER/Host").toString());
    }

    return Result;
}
//-----------------------------------------------------------------------------
void TSettings::setServerPort(quint16 inServerPort) /// Метод задаст порт сервера
{
    if (fSettings)
        fSettings->setValue("SERVER/Port", inServerPort);
}
//-----------------------------------------------------------------------------
quint16 TSettings::ServerPort() /// Метод вернёт порт сервера
{
    quint16 Result;

    if (!fSettings)
        Result = 0;
    else
    {
        if (!fSettings->contains("SERVER/Port"))
            fSettings->setValue("SERVER/Port", "7876");

        Result = fSettings->value("SERVER/Port").toUInt();
    }

    return Result;
}
//-----------------------------------------------------------------------------
void TSettings::setSaveLogin(bool inSaveLogin) /// Метод задаст флаг "Хранить логи
{
    if (fSettings)
        fSettings->setValue("LOCAL/SaveLogin", inSaveLogin);

    if (!inSaveLogin)
        setLogin(QString());
}
//-----------------------------------------------------------------------------
bool TSettings::SaveLogin() /// Метод вернёт флаг "Хранить логин"
{
    bool Result;

    if (!fSettings)
        Result = false;
    else
    {
        if (!fSettings->contains("LOCAL/SaveLogin"))
            fSettings->setValue("LOCAL/SaveLogin", false);

        Result = fSettings->value("LOCAL/SaveLogin").toBool();
    }

    return Result;
}
//-----------------------------------------------------------------------------
void TSettings::setLogin(QString inLogin) /// Метод задаст хранимый логин
{
    if (fSettings)
        fSettings->setValue("LOCAL/Login", inLogin);
}
//-----------------------------------------------------------------------------
QString TSettings::Login() /// Метод вернёт хранимый логин
{
    QString Result;

    if (!fSettings)
        Result = "";
    else
    {
        if (!fSettings->contains("LOCAL/Login"))
            fSettings->setValue("LOCAL/Login", "");

        Result = fSettings->value("LOCAL/Login").toString();
    }

    return Result;
}
//-----------------------------------------------------------------------------
void TSettings::init() /// Метод инициализирует класс
{
    try
    {
        if (!QFile::exists(fSettingsPath))
        {
            QFile FileSettings(fSettingsPath);
            FileSettings.open(QIODevice::WriteOnly);
            FileSettings.close();
        }

        fSettings.reset(new QSettings(fSettingsPath, QSettings::IniFormat));
    }
    catch(...)
    {
        fSettings.reset(nullptr);
        qDebug() << "[ОШИБКА] Не удалось считать файл конфигураций!";
    }
}
//-----------------------------------------------------------------------------

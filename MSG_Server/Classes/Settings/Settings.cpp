#include "Settings.h"

#include <QFile>
#include <QDebug>
#include <QNetworkInterface>

//-----------------------------------------------------------------------------
TSettings::TSettings(QObject *inParent) : QObject(inParent)
{ init(); }
//-----------------------------------------------------------------------------
TSettings::~TSettings()
{
    if (fSettings)
    {
        fSettings->sync();
        qDebug() << "[ИНФО] Настройки сохранены.";
    }
}
//-----------------------------------------------------------------------------
TSettings& TSettings::Instance()
{
    static TSettings Inst;
    return Inst;
}
//-----------------------------------------------------------------------------
/**
 * @brief TSettings::setServerPort - Метод задаст порт сервера
 * @param inServerPort - Порт сервера
 */
void TSettings::setServerPort(quint16 inServerPort)
{
    if (fSettings)
        fSettings->setValue("SERVER/Port", inServerPort);
}
//-----------------------------------------------------------------------------
/**
 * @brief TSettings::ServerPort - Метод вернёт порт сервера
 * @return Вернёт порт сервера
 */
quint16 TSettings::ServerPort()
{
    quint16 Result;

    if (!fSettings)
        Result = 7876;
    else
    {
        if (!fSettings->contains("SERVER/Port"))
            fSettings->setValue("SERVER/Port", 7876);

        Result = fSettings->value("SERVER/Port").toUInt();
    }

    return Result;
}
//-----------------------------------------------------------------------------
/**
 * @brief TSettings::setDataBaseType - Метод задаст тип БД
 * @param inType - Тип БД
 */
void TSettings::setDataBaseType(QString inType)
{
    if (fSettings)
        fSettings->setValue("DATABASE/ConnectType", inType);
}
//-----------------------------------------------------------------------------
/**
 * @brief TSettings::dataBaseType - Метод вернёт тип БД
 * @return Вернёт тип БД
 */
QString TSettings::dataBaseType()
{
    QString Result;

    if (!fSettings)
        Result = "QPSQL";
    else
    {
        if (!fSettings->contains("DATABASE/ConnectType"))
            fSettings->setValue("DATABASE/ConnectType", "QPSQL");

        Result = fSettings->value("DATABASE/ConnectType").toString();
    }

    return Result;
}
//-----------------------------------------------------------------------------
/**
 * @brief TSettings::setDataBaseHost - Метод задаст адрес хоста БД
 * @param inHost - Адрес хотса БД
 */
void TSettings::setDataBaseHost(QHostAddress inHost)
{
    if (fSettings)
        fSettings->setValue("DATABASE/HostName", inHost.toString());
}
//-----------------------------------------------------------------------------
/**
 * @brief TSettings::dataBaseHost - Метод вернёт адрес хоста БД
 * @return Вернёт адрес хоста БД
 */
QHostAddress TSettings::dataBaseHost()
{
    QHostAddress Result;

    if (!fSettings)
        Result = QHostAddress("127.0.0.1");
    else
    {
        if (!fSettings->contains("DATABASE/Host"))
            fSettings->setValue("DATABASE/Host", "127.0.0.1");

        Result = QHostAddress(fSettings->value("DATABASE/Host").toString());
    }

    return Result;
}
//-----------------------------------------------------------------------------
/**
 * @brief TSettings::setDataBasePort - Метод задаст порт БД
 * @param inPort - Порт БД
 */
void TSettings::setDataBasePort(qint32 inPort)
{
    if (fSettings)
        fSettings->setValue("DATABASE/Port", inPort);
}
//-----------------------------------------------------------------------------
/**
 * @brief TSettings::dataBasePort - Метод вернёт порт БД
 * @return Вернёт порт БД
 */
quint32 TSettings::dataBasePort()
{
    quint32 Result;

    if (!fSettings)
        Result = 5432;
    else
    {
        if (!fSettings->contains("DATABASE/Port"))
            fSettings->setValue("DATABASE/Port", 5432);

        Result = fSettings->value("DATABASE/Port").toUInt();
    }

    return Result;
}
//-----------------------------------------------------------------------------
/**
 * @brief TSettings::setDataBaseName - Метод задаст имя БД
 * @param inName - Имя БД
 */
void TSettings::setDataBaseName(QString inName)
{
    if (fSettings)
        fSettings->setValue("DATABASE/Name", inName);
}
//-----------------------------------------------------------------------------
/**
 * @brief TSettings::dataBaseName - Метод вернёт имя БД
 * @return Вернёт имя БД
 */
QString TSettings::dataBaseName()
{
    QString Result;

    if (!fSettings)
        Result = "MSGDB";
    else
    {
        if (!fSettings->contains("DATABASE/Name"))
            fSettings->setValue("DATABASE/Name", "MSGDB");

        Result = fSettings->value("DATABASE/Name").toString();
    }

    return Result;
}
//-----------------------------------------------------------------------------
/**
 * @brief TSettings::setDataBaseUser - Метод задаст пользователья БД
 * @param inUser - Пользователь БД
 */
void TSettings::setDataBaseUser(QString inUser)
{
    if (fSettings)
        fSettings->setValue("DATABASE/User", inUser);
}
//-----------------------------------------------------------------------------
/**
 * @brief TSettings::dataBaseUser - Метод вернёт пользователья БД
 * @return Вернёт пользователя БД
 */
QString TSettings::dataBaseUser()
{
    QString Result;

    if (!fSettings)
        Result = "postgres";
    else
    {
        if (!fSettings->contains("DATABASE/User"))
            fSettings->setValue("DATABASE/User", "postgres");

        Result = fSettings->value("DATABASE/User").toString();
    }

    return Result;
}
//-----------------------------------------------------------------------------
/**
 * @brief TSettings::setDataBasePassword - Метод задаст пароль БД
 * @param inPassword - Пароль БД
 */
void TSettings::setDataBasePassword(QString inPassword)
{
    if (fSettings)
        fSettings->setValue("DATABASE/Password", inPassword);
}
//-----------------------------------------------------------------------------
/**
 * @brief TSettings::dataBasePassword - Метод вернёт пароль БД
 * @return Вернёт пароль БД
 */
QString TSettings::dataBasePassword()
{
    QString Result;

    if (!fSettings)
        Result = "postgres";
    else
    {
        if (!fSettings->contains("DATABASE/Password"))
            fSettings->setValue("DATABASE/Password", "postgres");

        Result = fSettings->value("DATABASE/Password").toString();
    }

    return Result;
}
//-----------------------------------------------------------------------------
/**
 * @brief TSettings::init - Метод инициализирует класс
 */
void TSettings::init()
{
    try
    {
        bool SettingsFileOK = true; // Флаг состояния файла конфигураций

        if (!QFile::exists(fSettingsPath)) // Если файла не существует
        {
            QFile FileSettings(fSettingsPath); // Инициализируем файл настроек

            if (!FileSettings.open(QIODevice::WriteOnly)) // Если файл не удалось создать
            {
                SettingsFileOK = false;
                qDebug() << "[ОШИБКА] Не удалось создать файл конфигураций!";
            }
            else // Если файл создан
            {
                FileSettings.close(); // Закрываем созданный файл (Пустой)
                SettingsFileOK = true;
                qDebug() << "[ИНФО] Создан файл конфигураций.";
            }
        } else SettingsFileOK = true; // Файл существует

        if (SettingsFileOK) // Если файл найден\создан
            fSettings.reset(new QSettings(fSettingsPath, QSettings::IniFormat)); // Инициализируем настройки
    }
    catch(...)
    {
        fSettings.reset(nullptr);
        qDebug() << "[ОШИБКА] Не удалось считать файл конфигураций!";
    }
}
//-----------------------------------------------------------------------------

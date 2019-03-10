#ifndef SETTINGS_H
#define SETTINGS_H

#include <memory>

#include <QObject>
#include <QSettings>
#include <QHostAddress>
#include <QApplication>

class TSettings : public QObject
{
    Q_OBJECT
public:
    static TSettings& Instance();

    void setServerPort(quint16 inServerPort); // Метод задаст порт сервера
    quint16 ServerPort(); // Метод вернёт порт сервера

    void setDataBaseType(QString inType); // Метод задаст тип БД
    QString dataBaseType(); // Метод вернёт тип БД

    void setDataBaseHost(QHostAddress inHost); // Метод задаст адрес хоста БД
    QHostAddress dataBaseHost(); // Метод вернёт адрес хоста БД

    void setDataBasePort(qint32 inPort); // Метод задаст порт БД
    quint32 dataBasePort(); // Метод вернёт порт БД

    void setDataBaseName(QString inName); // Метод задаст имя БД
    QString dataBaseName(); // Метод вернёт имя БД

    void setDataBaseUser(QString inUser); // Метод задаст пользователья БД
    QString dataBaseUser(); // Метод вернёт пользователья БД

    void setDataBasePassword(QString inPassword); // Метод задаст пароль БД
    QString dataBasePassword(); // Метод вернёт пароль БД

private:
    const QString fSettingsPath = QApplication::applicationDirPath() + "/Settings.ini";
    std::unique_ptr<QSettings> fSettings = nullptr; // Файл конфигураций

    void init(); // Метод инициализирует класс

    explicit TSettings(QObject *inParent = nullptr);
    ~TSettings();

    TSettings(TSettings const&) = delete;
    TSettings& operator= (TSettings const&) = delete;

signals:

public slots:
};

#endif // SETTINGS_H

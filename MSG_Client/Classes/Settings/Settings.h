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

    void setServerAddress(QHostAddress inServerAddress); /// Метод задаст адрес сервера
    QHostAddress ServerAddress(); /// Метод вернёт адрес сервера

    void setServerPort(quint16 inServerPort); /// Метод задаст порт сервера
    quint16 ServerPort(); /// Метод вернёт порт сервера

    void setSaveLogin(bool inSaveLogin); /// Метод задаст флаг "Хранить логи
    bool SaveLogin(); /// Метод вернёт флаг "Хранить логин"

    void setLogin(QString inLogin); /// Метод задаст хранимый логин
    QString Login(); /// Метод вернёт хранимый логин

private:
    const QString fSettingsPath = QApplication::applicationDirPath() + "/Settings.ini";
    std::unique_ptr<QSettings> fSettings = nullptr; /// Файл конфигураций

    void init(); /// Метод инициализирует класс

    explicit TSettings(QObject *inParent = nullptr);
    ~TSettings();

    TSettings(TSettings const&) = delete;
    TSettings& operator= (TSettings const&) = delete;

signals:

public slots:
};

#endif // SETTINGS_H

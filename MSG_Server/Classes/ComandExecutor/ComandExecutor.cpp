#include "ComandExecutor.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>

#include "comandes.h"
#include "resultcodes.h"
#include "Classes/DataModule/DataModule.h"

//-----------------------------------------------------------------------------
TComandExecutor::TComandExecutor(QObject *inParent) : QObject(inParent)
{

}
//-----------------------------------------------------------------------------
void TComandExecutor::executCommand(QTcpSocket* inClientSender)
{
    if (!inClientSender)
        return;

    QDataStream inStream(inClientSender); // Оборачиваем его в поток данных

    QByteArray ReturningData; // Возвращаемый результат
    QDataStream outStream(&ReturningData, QIODevice::WriteOnly);

    quint8 Command; // Команда от клиента
    inStream >> Command; // Получаем команду

    switch (Command) // Проверяем команду
    {
        case Commands::CreateUser: // Регистрация пользователя
        {
            sig_LogMessage(inClientSender->localAddress(), "Получен запрос на создание пользователя");

            qint32 Resuslt = creteUser(inStream); // Регистрация пользователя
            outStream << Command << Resuslt; // Пишем в результат команду и результат обработки
            sig_LogMessage(inClientSender->localAddress(), "Отправка ответа о создании пользователя");

            break;
        }
        case Commands::Authorization: // Авторизация пользователя
        {
            sig_LogMessage(inClientSender->localAddress(), "Получен запрос на авторизацию");

            std::pair<qint32, QUuid> Resuslt = canAuthorization(inStream); // Обрабатываем возможность авторизации

            switch (Resuslt.first) // Проверяем результат обработки
            {
                case Res::CanAut::caAuthorizationFalse: // Пользователь не найден (авторизация не возможна)
                {
                    outStream << Command << Resuslt.first; // Пишем в результат команду и результат обработки
                    sig_LogMessage(inClientSender->localAddress(), "Отказ в авторизации, пользователь не найден");
                    break;
                }
                case Res::CanAut::caIncorrectPass: // Пользователь найден, но пароль не верный (авторизация не возможна)
                {
                    outStream << Command << Resuslt.first; // Пишем в результат команду и результат обработки
                    sig_LogMessage(inClientSender->localAddress(), "Отказ в авторизации, не верный пароль");
                    break;
                }
                case Res::CanAut::caAuthorizationTrue: // Вользователь найден (авторизация возможна)
                {
                    TUserInfo UserInfo = getUserInfo(Resuslt.second);

                    if (UserInfo.userUuid() != Resuslt.second)
                    {
                        outStream << Command << Res::CanAut::caUserInfoError; // Пишем в результат команду и результат обработки
                        sig_LogMessage(inClientSender->localAddress(), "Ошибка чтения данных записи");
                    }
                    else
                    {
                        outStream << Command << Resuslt.first << UserInfo; // Пишем в результат команду и результат обработки
                        sigSetUserInfo(inClientSender, UserInfo); // Авторизируем пользователя
                        sig_LogMessage(inClientSender->localAddress(), "Авторизация разрешена");
                    }
                    break;
                }
                default: // Вернулось нипойми что
                {
                    outStream << Command << Res::rUnknown; // Пишем в результат команду и результат обработки
                    sig_LogMessage(inClientSender->localAddress(), "Неизвестная ошибка");
                    break;
                }
            }

            break;
        }

        default: sig_LogMessage(inClientSender->localAddress(), "Получена неизвестная команда");
    }

    inClientSender->write(ReturningData); // Возвращаем результат
    //inClientSender->waitForBytesWritten();
}
//-----------------------------------------------------------------------------
qint32 TComandExecutor::creteUser(QDataStream &inDataStream)
{
    quint32 Result = 0;

    QString Login = ReadStringFromStream(inDataStream);

    QByteArray PasswordHash;
    inDataStream >> PasswordHash;

    QSqlQuery Query(TDM::Instance().DB());

    if(!Query.prepare("SELECT * FROM create_user(:in_login, :in_password, :in_name)"))
        qDebug() << "[ОШИБКА]: " + Query.lastError().text();
    else
    {
        Query.bindValue(":in_login", Login.toUtf8());
        Query.bindValue(":in_password", QString(PasswordHash).toUtf8());
        Query.bindValue(":in_name", Login.toUtf8());

        if (!Query.exec())
            qDebug() << "[ОШИБКА]: " + Query.lastError().text();
        else
        {
            while (Query.next()) // Вернётся только 1 запись
                Result = Query.value("create_user").toInt();
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
std::pair<qint32, QUuid> TComandExecutor::canAuthorization(QDataStream &inDataStream) // Метод авторизирует пользователя
{
    std::pair<qint32, QUuid> Result;

    QString Login = ReadStringFromStream(inDataStream);

    QByteArray PasswordHash;
    inDataStream >> PasswordHash;

    QSqlQuery Query(TDM::Instance().DB());

    if(!Query.prepare("SELECT * FROM can_user_authorization(:in_login, :in_password)"))
        qDebug() << "[ОШИБКА]: " + Query.lastError().text();
    else
    {
        Query.bindValue(":in_login", Login.toUtf8());
        Query.bindValue(":in_password", QString(PasswordHash).toUtf8());

        if (!Query.exec())
            qDebug() << "[ОШИБКА]: " + Query.lastError().text();
        else
        {
            while (Query.next()) // Вернётся только 1 запись
            {
                Result.first = Query.value("result_code").toInt();
                Result.second = Query.value("uuid_found_user").toByteArray();
            }
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
TUserInfo TComandExecutor::getUserInfo(QUuid inUserUuid)
{
    TUserInfo Result;

    QSqlQuery Query(TDM::Instance().DB());

    if(!Query.prepare("SELECT * FROM get_user_info(:in_uuid)"))
        qDebug() << "[ОШИБКА]: " + Query.lastError().text();
    else
    {
        Query.bindValue(":in_uuid", inUserUuid);

        if (!Query.exec())
            qDebug() << "[ОШИБКА]: " + Query.lastError().text();
        else
        {
            while (Query.next()) // Вернётся только 1 запись
            {
                Result.setUserUuid(Query.value("found_user_uuid").toUuid());
                Result.setUserType(Query.value("found_user_type").toUInt());
                Result.setUserLogin(QString::fromUtf8(Query.value("found_user_login").toByteArray()));
                Result.setUserName(QString::fromUtf8(Query.value("found_user_name").toByteArray()));
            }
        }
    }

    return Result;
}
//-----------------------------------------------------------------------------
QString TComandExecutor::ReadStringFromStream(QDataStream &inDataStream)
{
    QByteArray ByteBuf;
    inDataStream >> ByteBuf;

    return QString::fromUtf8(ByteBuf); // Помещаем буфер в результат (с раскодировкой из UTF8)
}
//-----------------------------------------------------------------------------

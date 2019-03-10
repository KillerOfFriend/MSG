#ifndef DB_H
#define DB_H

#include <QObject>
#include <QSqlDatabase>

class TDB : public QObject
{
    Q_OBJECT
public:
    static TDB& Instance();
    QSqlDatabase& DB(); // Метод вернёт ссылку на класс базы данных

private:
    QSqlDatabase fDB; // Объект базы данных

    explicit TDB(QObject* inParent = nullptr);
    ~TDB();

    TDB(TDB const&) = delete;
    TDB& operator= (TDB const&) = delete;

    void initDB(); // Метод инициализирует базу данных
};

#endif // DB_H

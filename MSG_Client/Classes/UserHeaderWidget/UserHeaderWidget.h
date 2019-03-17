#ifndef USERHEADERWIDGET_H
#define USERHEADERWIDGET_H

#include <memory>

#include <QWidget>
#include <QImage>

namespace Ui {
class TUserHeaderWidget;
}

class TUserHeaderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TUserHeaderWidget(QWidget* inParent = nullptr);
    ~TUserHeaderWidget();

public slots:
    void slot_SetUserAvatar(QImage inAvatar, bool inUserIsMale = true); // Метод установит аватар
    void slot_SetUserName(QString inName); // Метод установит имя пользователя
    void slot_SetUserLogin(QString inLogin); // Метод установит логин пользователя

private:
    Ui::TUserHeaderWidget *ui;

    void initFont(); // Метод инициализирует стиль текста
};

#endif // USERHEADERWIDGET_H

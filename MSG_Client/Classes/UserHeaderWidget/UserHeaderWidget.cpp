#include "UserHeaderWidget.h"
#include "ui_UserHeaderWidget.h"

#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

//-----------------------------------------------------------------------------
TUserHeaderWidget::TUserHeaderWidget(QWidget *inParent) :
    QWidget(inParent),
    ui(new Ui::TUserHeaderWidget)
{
    ui->setupUi(this);

    initFont();
}
//-----------------------------------------------------------------------------
TUserHeaderWidget::~TUserHeaderWidget()
{
    delete ui;
}
//-----------------------------------------------------------------------------
/**
 * @brief initFont - Метод инициализирует стиль текста
 */
void TUserHeaderWidget::initFont()
{
    QString StyleSheet = "QLabel { color : #000000; }";
    QFont NameFont = QApplication::font();
    NameFont.setPointSize(10);
    NameFont.setBold(true);

    ui->UserNameLabel->setStyleSheet(StyleSheet);
    ui->UserNameLabel->setFont(NameFont);

    StyleSheet = "QLabel { color : #696969; }";
    QFont LoginFont = QApplication::font();
    LoginFont.setPointSize(10);
    LoginFont.setItalic(true);

    ui->UserLoginLabel->setStyleSheet(StyleSheet);
    ui->UserLoginLabel->setFont(LoginFont);
}
//-----------------------------------------------------------------------------
/**
 * @brief TUserHeaderWidget::setAvatar - Метод установит аватар
 * @param inAvatar - Аватар пользователя
 * @param inUserIsMale - Флаг половой пренадлежности пользователя (по умолчанию [М])
 */
void TUserHeaderWidget::slot_SetUserAvatar(QImage inAvatar, bool inUserIsMale)
{
    QSize AvatarSize(ui->UserAvatarView->size().width(), ui->UserAvatarView->size().height()); // Задаём позицию отрисовки верхнего угла аватара

    ui->UserAvatarView->setScene(new QGraphicsScene(this));

    if (inAvatar.isNull()) // Если аватар не инициализирован
    {
        if (inUserIsMale) // Проверяем половую пренадлежность пользователя
            inAvatar = QImage(QImage(":/Resurse/Other/Images/Other/AvaMale.png").scaled(AvatarSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation)); // Загружаем дефолтный мужской аватар
        else
            inAvatar = QImage(":/Resurse/Other/Images/Other/AvaFemale.png").scaled(AvatarSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation); // Загружаем дефолтный женский аватар
    }
    else
        if(inAvatar.size() != AvatarSize) // Если аватар не соответствует размерам
            inAvatar = inAvatar.scaled(AvatarSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation); // Подгоняем его

    ui->UserAvatarView->scene()->addItem(new QGraphicsPixmapItem(QPixmap::fromImage(inAvatar))); // Помещаем аватар на сцену
}
//-----------------------------------------------------------------------------
/**
 * @brief TUserHeaderWidget::setName - Метод установит имя пользователя
 * @param inName - Имя пользователя
 */
void TUserHeaderWidget::slot_SetUserName(QString inName)
{ ui->UserNameLabel->setText(inName); }
//-----------------------------------------------------------------------------
/**
 * @brief TUserHeaderWidget::setLogin - Метод установит логин пользователя
 * @param inLogin - Логин пользователя
 */
void TUserHeaderWidget::slot_SetUserLogin(QString inLogin)
{ ui->UserLoginLabel->setText('<' + inLogin + '>'); }
//-----------------------------------------------------------------------------

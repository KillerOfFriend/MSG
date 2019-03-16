#include "UserItemDelegate.h"
 
#include "Models/UsersModel/UsersModel.h"
 
TUserItemDelegate::TUserItemDelegate()
{
}
 
TUserItemDelegate::~TUserItemDelegate()
{
}
 
QSize TUserItemDelegate::sizeHint(const QStyleOptionViewItem &  option, const QModelIndex & index) const
{
    if(!index.isValid())
        return QSize();

    QString UserName = index.sibling(index.row(), TUsersModel::cUserName).data().toString(); // Получаем имя пользователя из модели
    QString UserLogin = '<' + index.sibling(index.row(), TUsersModel::cUserLogin).data().toString() + '>'; // Получаем логин пользователя из модели
 
    QFont UserNameFont = QApplication::font(); // Инициализируем стиль для имени пользователя
    UserNameFont.setBold(true); // Разрешаем "Жирный"
    QFont UserLoginFont = QApplication::font(); // Инициализируем стиль для логина пользователя
    UserLoginFont.setItalic(true); // Резрешаем курсив

    QFontMetrics FMUserName(UserNameFont);
    QFontMetrics FMUserLogin(UserLoginFont);
 
    /* No need for x,y here. we only need to calculate the height given the width.
     * Note that the given height is 0. That is because boundingRect() will return
     * the suitable height if the given geometry does not fit. And this is exactly
     * what we want.
     */

    // Инициализируем размеры надписи имени пользователя
    QRect UserNameRect = FMUserName.boundingRect(option.rect.left() + fAvatarSize.width(), option.rect.top() + fIndent,
                                  option.rect.width() - fAvatarSize.width(), 0,
                                  Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, UserName);
    // Инициализируем размеры надписи логина пользователя
    QRect UserLoginRect = FMUserLogin.boundingRect(UserNameRect.left(), UserNameRect.bottom() + fIndent,
                                     option.rect.width() - fAvatarSize.width(), 0,
                                     Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, UserLogin);
 
    // Опциональная ширина, Высотанадписи именю пользоваьеля + Высота надписи логина пользователя
    QSize Size(option.rect.width(), UserNameRect.height() + UserLoginRect.height() +  fIndent * 3);

    if(Size.height() < fAvatarSize.height()) // Если расчётная высота меньше высоты аватара
        Size.setHeight(fAvatarSize.height()); // Устанавливаем высоту равную высоте аватара
 
    return Size; // Возвращаем размер айтема
}
 
void TUserItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid())
        return;
 
    painter->save();
 
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());

    QString UserName = index.sibling(index.row(), TUsersModel::cUserName).data().toString(); // Получаем имя пользователя из модели
    QString UserLogin = '<' + index.sibling(index.row(), TUsersModel::cUserLogin).data().toString() + '>'; // Получаем логин пользователя из модели
    QImage UserAvatar(index.sibling(index.row(), TUsersModel::cUserAvatar).data().toByteArray()); // Получаем аватар пользователя из модели

    if (UserAvatar.isNull()) // Если аватар не инициализирован (пуст)
    {
        bool IsMale = index.sibling(index.row(), TUsersModel::cUserIsMale).data().toBool(); // Проверяем половую пренадлежность

        if (IsMale)
            UserAvatar = QImage(":/Resurse/Other/Images/Other/AvaMale.png").scaled(fAvatarSize); // Загружаем дефолтный мужской аватар
        else
            UserAvatar = QImage(":/Resurse/Other/Images/Other/AvaFemale.png").scaled(fAvatarSize); // Загружаем дефолтный женский аватар
    }


    QFont UserNameFont = QApplication::font(); // Инициализируем стиль для имени пользователя
    UserNameFont.setBold(true); // Разрешаем "Жирный"
    QFont UserLoginFont = QApplication::font(); // Инициализируем стиль для логина пользователя
    UserLoginFont.setItalic(true); // Резрешаем курсив

    QFontMetrics FMUserName(UserNameFont);
    QFontMetrics FMUserLogin(UserLoginFont);

    /*
     * The x,y coords are not (0,0) but values given by 'option'. So, calculate the
     * rects again given the x,y,w.
     * Note that the given height is 0. That is because boundingRect() will return
     * the suitable height if the given geometry does not fit. And this is exactly
     * what we want.
     */    

    // Инициализируем размеры надписи имени пользователя
    QRect UserNameRect = FMUserName.boundingRect(option.rect.left() + fAvatarSize.width(), option.rect.top() + fIndent,
                                  option.rect.width() - fAvatarSize.width(), 0,
                                  Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, UserName);
    // Инициализируем размеры надписи логина пользователя
    QRect UserLoginRect = FMUserLogin.boundingRect(UserNameRect.left(), UserNameRect.bottom() + fIndent,
                                     option.rect.width() - fAvatarSize.width(), 0,
                                     Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, UserLogin);
 
    QPoint AvatarStartDraw(option.rect.left(), option.rect.top());
    QRect AvatarRect(AvatarStartDraw, fAvatarSize);
    painter->drawImage(AvatarRect, UserAvatar);
 
    painter->setPen(Qt::black);
    painter->setFont(UserNameFont);
    painter->drawText(UserNameRect, Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, UserName);
 
    painter->setPen(Qt::gray);
    painter->setFont(UserLoginFont);
    painter->drawText(UserLoginRect, Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, UserLogin);

    painter->restore();
}

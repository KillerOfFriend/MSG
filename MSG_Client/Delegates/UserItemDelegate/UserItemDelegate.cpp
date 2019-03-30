#include "UserItemDelegate.h"

#include <QApplication>

#include "Models/UsersModel/UsersModel.h"
 
//-----------------------------------------------------------------------------
TUserItemDelegate::TUserItemDelegate()
{

}
 //-----------------------------------------------------------------------------
TUserItemDelegate::~TUserItemDelegate()
{

}
//-----------------------------------------------------------------------------
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

    // Устанавливаем позицию отрисовки аватара
    QPoint AvatarStartDraw(option.rect.left(), option.rect.top());
    QRect AvatarRect(AvatarStartDraw, fAvatarSize);

    // Инициализируем размеры надписи имени пользователя
    QRect UserNameRect = FMUserName.boundingRect(AvatarRect.left() + AvatarRect.width() + 1, option.rect.top() + fIndent,
                                                 option.rect.width() - (fAvatarSize.width() + fStatusSize.width()), 0,
                                                 Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, UserName);
    // Инициализируем размеры надписи логина пользователя
    QRect UserLoginRect = FMUserLogin.boundingRect(UserNameRect.left(), UserNameRect.bottom() + fIndent,
                                                 UserNameRect.width(), 0,
                                                 Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, UserLogin);
 
    // Опциональная ширина, Высота надписи имени пользователя + Высота надписи логина пользователя
    QSize Size(option.rect.width(), UserNameRect.height() + UserLoginRect.height() +  fIndent * 3);

    if(Size.height() < fAvatarSize.height()) // Если расчётная высота меньше высоты аватара
        Size.setHeight(fAvatarSize.height()); // Устанавливаем высоту равную высоте аватара
 
    return Size; // Возвращаем размер айтема
}
//-----------------------------------------------------------------------------
void TUserItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid())
        return;
 
    painter->save();
 
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());

    QString UserName = index.sibling(index.row(), TUsersModel::cUserName).data().toString(); // Получаем имя пользователя из модели
    QString UserLogin = '<' + index.sibling(index.row(), TUsersModel::cUserLogin).data().toString() + '>'; // Получаем логин пользователя из модели
    QImage UserAvatar = QImage::fromData(index.sibling(index.row(), TUsersModel::cUserAvatar).data().toByteArray()); // Получаем аватар пользователя из модели
    QImage UserStatus; // Отображение статуса пользователя (Инициализируется позже)

    if (UserAvatar.isNull()) // Если аватар не инициализирован (пуст)
    {
        bool IsMale = index.sibling(index.row(), TUsersModel::cUserIsMale).data().toBool(); // Проверяем половую пренадлежность

        if (IsMale)
            UserAvatar = QImage(":/Resurse/Other/Images/Other/AvaMale.png").scaled(fAvatarSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation); // Загружаем дефолтный мужской аватар
        else
            UserAvatar = QImage(":/Resurse/Other/Images/Other/AvaFemale.png").scaled(fAvatarSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation); // Загружаем дефолтный женский аватар
    }

    switch (index.sibling(index.row(), TUsersModel::cUserStatus).data().toInt())
    {
        case Core::UserStatus::usOffline: { UserStatus = QImage(":/Resurse/UserStatus/Images/UserStatus/UserOffline.png").scaled(fStatusSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation); break; };
        case Core::UserStatus::usOnline: { UserStatus = QImage(":/Resurse/UserStatus/Images/UserStatus/UserOnline.png").scaled(fStatusSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation); break; };

        default: { UserStatus = QImage(":/Resurse/UserStatus/Images/UserStatus/UserUnknown.png").scaled(fStatusSize); break; };
    }


    QFont UserNameFont = QApplication::font(); // Инициализируем стиль для имени пользователя
    UserNameFont.setBold(true); // Разрешаем "Жирный"
    QFont UserLoginFont = QApplication::font(); // Инициализируем стиль для логина пользователя
    UserLoginFont.setItalic(true); // Резрешаем курсив

    QFontMetrics FMUserName(UserNameFont);
    QFontMetrics FMUserLogin(UserLoginFont);

    // Устанавливаем позицию отрисовки аватара
    QPoint AvatarStartDraw(option.rect.left(), option.rect.top());
    QRect AvatarRect(AvatarStartDraw, fAvatarSize);

    // Инициализируем размеры надписи имени пользователя
    QRect UserNameRect = FMUserName.boundingRect(AvatarRect.left() + AvatarRect.width() + 1, option.rect.top() + fIndent,
                                                 option.rect.width() - (fAvatarSize.width() + fStatusSize.width()), 0,
                                                 Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, UserName);
    // Инициализируем размеры надписи логина пользователя
    QRect UserLoginRect = FMUserLogin.boundingRect(UserNameRect.left(), UserNameRect.bottom() + fIndent,
                                                 UserNameRect.width(), 0,
                                                 Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, UserLogin);

    // Устанавливаем позицию отрисовки статуса
    QPoint StatusDrawStart(option.rect.width() - (fStatusSize.width() + fIndent), option.rect.top() + (option.rect.height() / 2.0 - fStatusSize.height() / 2.0));
    QRect StatusRect(StatusDrawStart, fStatusSize);

    painter->drawImage(AvatarRect, UserAvatar);
 
    painter->setPen(Qt::black);
    painter->setFont(UserNameFont);
    painter->drawText(UserNameRect, Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, UserName);
 
    painter->setPen(Qt::gray);
    painter->setFont(UserLoginFont);
    painter->drawText(UserLoginRect, Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, UserLogin);

    painter->drawImage(StatusRect, UserStatus);

    painter->restore();
}
//-----------------------------------------------------------------------------

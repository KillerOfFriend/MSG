#include "ChatItemDelegate.h"

#include <QApplication>

#include "Classes/DataModule/DataModule.h"
#include "Classes/ChatInfo/ChatInfo.h"
#include "Models/ChatsModel/ChatsModel.h"

//-----------------------------------------------------------------------------
TChatItemDelegate::TChatItemDelegate()
{

}
//-----------------------------------------------------------------------------
TChatItemDelegate::~TChatItemDelegate()
{

}
//-----------------------------------------------------------------------------
QSize TChatItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    if(!index.isValid())
        return QSize();

    QString ChatName = index.sibling(index.row(), TChatsModel::cChatName).data().toString(); // Получаем имя беседы

    QFont ChatNameFont = QApplication::font(); // Инициализируем стиль для имени беседы
    ChatNameFont.setBold(true); // Разрешаем "Жирный"

    QFontMetrics FMUserName(ChatNameFont);

    // Устанавливаем позицию отрисовки первью беседы
    QPoint PreviewStartDraw(option.rect.left(), option.rect.top());
    QRect PreviewRect(PreviewStartDraw, fPreviewSize);

    // Инициализируем размеры надписи имени беседы
    QRect ChatNameRect = FMUserName.boundingRect(PreviewRect.left() + PreviewRect.width() + 1, option.rect.top() + fIndent,
                                                 option.rect.width() - (fPreviewSize.width() + fPreviewSize.width()), 0,
                                                 Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, ChatName);

    // Опциональная ширина, Высота надписи именю пользоваьеля
    QSize Size(option.rect.width(), ChatNameRect.height() + fIndent * 3);

    if(Size.height() < fPreviewSize.height()) // Если расчётная высота меньше высоты аватара
        Size.setHeight(fPreviewSize.height()); // Устанавливаем высоту равную высоте аватара

    return Size; // Возвращаем размер айтема
}
//-----------------------------------------------------------------------------
void TChatItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(!index.isValid())
        return;

    painter->save();

    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());

    QString ChatName = index.sibling(index.row(), TChatsModel::cChatName).data().toString(); // Получаем имя беседы
    QImage ChatPreview = createPreview(index.sibling(index.row(), TChatsModel::cChatUuid).data().toUuid()); // Создаём превью беседы

    QFont ChatNameFont = QApplication::font(); // Инициализируем стиль для имени беседы
    ChatNameFont.setBold(true); // Разрешаем "Жирный"

    QFontMetrics FMUserName(ChatNameFont);

    // Устанавливаем позицию отрисовки первью беседы
    QPoint PreviewStartDraw(option.rect.left(), option.rect.top());
    QRect PreviewRect(PreviewStartDraw, fPreviewSize);

    // Инициализируем размеры надписи имени беседы
    QRect ChatNameRect = FMUserName.boundingRect(PreviewRect.left() + PreviewRect.width() + 1, option.rect.top() + fIndent,
                                                 option.rect.width() - (fPreviewSize.width() + fPreviewSize.width()), 0,
                                                 Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, ChatName);

    painter->drawImage(PreviewRect, ChatPreview);

    painter->setPen(Qt::black);
    painter->setFont(ChatNameFont);
    painter->drawText(ChatNameRect, Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, ChatName);


    painter->restore();
}
//-----------------------------------------------------------------------------
QImage TChatItemDelegate::createPreview(QUuid inChatUuid) const // Метод создаст превью беседы
{
    auto FindChatRes = TDM::Instance().Client()->userAccount()->chats()->find(inChatUuid);
    if (FindChatRes == TDM::Instance().Client()->userAccount()->chats()->end())
        return QImage();

    Core::ChatInfo_Ptr chat = FindChatRes->second;

    QImage Result = QImage(fPreviewSize, QImage::Format_ARGB32); // Результирующее изображение превью беседы
    Result.fill(Qt::transparent); // Заливаем буфер "прозрачным цветом"
    QPainter Painter; // Отрисовщик

    quint8 AvaCount = std::min(static_cast<std::size_t>(fPreviewAvaCount), FindChatRes->second->clients()->size() - 1); // Ищим минимальное кол-во аватаров на превью
    quint8 AvaRow = 0;
    quint8 AvaCol = 0;

    Painter.begin(&Result); // Начинаем отрисовку результирующего изображения
    // =>
    auto LastUserIt = FindChatRes->second->clients()->begin(); // Получаем итератор на первого порльзователя беседы
    std::advance(LastUserIt, AvaCount + 1); // Сдвигаем его на последнего отображаемого + 1
    // Перебираем отображаемых на превью пользователей
    std::for_each(FindChatRes->second->clients()->begin(), LastUserIt, [&] (const std::pair<QUuid, Core::UserInfo_Ptr> &ChatUser)
    {
        QImage UserAvatar; // Аватар пользователя

        if (!ChatUser.second->userAvatar().isNull()) // Если у пользователя задан аватар
            UserAvatar = ChatUser.second->userAvatar().scaled(fPreviewAvaSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation); // Получаем аватар пользователя

        if (UserAvatar.isNull()) // Если не удалось получить аватар пользователя
        {   // Требеутся подгрузить дефолтный
            if (ChatUser.second->userIsMale())
                UserAvatar = QImage(":/Resurse/Other/Images/Other/AvaMale.png").scaled(fPreviewAvaSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation); // Загружаем дефолтный мужской аватар
            else
                UserAvatar = QImage(":/Resurse/Other/Images/Other/AvaFemale.png").scaled(fPreviewAvaSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation); // Загружаем дефолтный женский аватар
        }

        Painter.drawImage(AvaCol * fPreviewAvaSize.height(), AvaRow * fPreviewAvaSize.width(), UserAvatar); // Отрисовываем аватар пользователя

        AvaCol++; // Переходим к следующему столбцу
        if (AvaCol == fPreviewColCount) // Если столбец больше дозволенного
        {
            AvaCol = 0; // Возвращаемся к нулевому столбцу
            AvaRow++; // Переходим к следующей строке
        }
    });
    // =>
    Painter.end(); // Отрисовка завершена

    return Result;
}
//-----------------------------------------------------------------------------

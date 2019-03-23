#include "ChatItemDelegate.h"

#include <QApplication>

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
    QImage Result;
    QPainter Painter(&Result);
    //Painter.drawImage(0, 0, );

    return Result;
}
//-----------------------------------------------------------------------------

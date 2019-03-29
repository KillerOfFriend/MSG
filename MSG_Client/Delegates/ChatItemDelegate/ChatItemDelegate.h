#ifndef CHATITEMDELEGATE_H
#define CHATITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QUuid>

class TChatItemDelegate : public QStyledItemDelegate
{
public:
    TChatItemDelegate();
    ~TChatItemDelegate();

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index ) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    const QSize fPreviewSize = QSize(64, 64); // Размеры превью беседы
    const int fIndent = 5; // Отступ

    const quint8 fPreviewRowCount = 2; // Количество строк в превью
    const quint8 fPreviewColCount = 2; // Количество столбцов в превью
    const quint8 fPreviewAvaCount = fPreviewRowCount * fPreviewColCount; // Количество пользователей на превью беседы (По умаолчанию 4)
    const QSize fPreviewAvaSize = QSize(fPreviewSize.width() / fPreviewRowCount, fPreviewSize.height() / fPreviewColCount); // Размер аватара пользователя на превью

    QImage createPreview(QUuid inChatUuid) const; // Метод создаст превью беседы
};

#endif // CHATITEMDELEGATE_H

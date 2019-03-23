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
    const QSize fPreviewSize = QSize(64, 64); // Размеры картинки беседы
    const int fIndent = 5; // Отступ

    QImage createPreview(QUuid inChatUuid) const; // Метод создаст превью беседы
};

#endif // CHATITEMDELEGATE_H

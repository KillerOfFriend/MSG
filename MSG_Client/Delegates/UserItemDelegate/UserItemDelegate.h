#ifndef USERITEMDELEGATE_H
#define USERITEMDELEGATE_H
 
#include <QStyledItemDelegate>
#include <QPainter>
 
class TUserItemDelegate : public QStyledItemDelegate
{
public:
    TUserItemDelegate();
    ~TUserItemDelegate();
 
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index ) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
 
private:
    const QSize fAvatarSize = QSize(64, 64); // Размеры картинки аватара
    const QSize fStatusSize = QSize(16, 16); // Размеры картинки статуса
    const int fIndent = 5; // Отступ
};
 
#endif // USERITEMDELEGATE_H

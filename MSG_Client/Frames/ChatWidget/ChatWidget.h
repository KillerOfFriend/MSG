#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include <QUuid>

namespace Ui {
class TChatWidget;
}

class TChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TChatWidget(QWidget *inParent = nullptr);
    ~TChatWidget();

    void setChatUuid(QUuid inChatUuid); // Метод задаст Uuid беседы
    QUuid chatUuid() const; // Метод вернёт Uuid беседы

private:
    Ui::TChatWidget *ui;

    QUuid fChatUuid; // Uuid беседы
};

#endif // CHATWIDGET_H

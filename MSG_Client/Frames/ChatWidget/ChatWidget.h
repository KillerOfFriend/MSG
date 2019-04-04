#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include <QUuid>

#include "Classes/ChatMessage/ChatMessage.h"

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

    void sendMessage(Core::TChatMessage &inMessage); // Метод отправит сообщение

protected:
    bool eventFilter(QObject *inObject, QEvent *inEvent); // Фильтр событий
private slots:
    void on_btnSendTextMessage_clicked();
    void on_btnSendFile_clicked();
    void on_btnSendImage_clicked();
};

#endif // CHATWIDGET_H

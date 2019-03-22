#include "ChatWidget.h"
#include "ui_ChatWidget.h"

//-----------------------------------------------------------------------------
TChatWidget::TChatWidget(QWidget *inParent) :
    QWidget(inParent),
    ui(new Ui::TChatWidget)
{
    ui->setupUi(this);
}
//-----------------------------------------------------------------------------
TChatWidget::~TChatWidget()
{
    delete ui;
}
//-----------------------------------------------------------------------------
void TChatWidget::setChatUuid(QUuid inChatUuid) // Метод задаст Uuid беседы
{ fChatUuid = inChatUuid; }
//-----------------------------------------------------------------------------
QUuid TChatWidget::chatUuid() const // Метод вернёт Uuid беседы
{ return fChatUuid; }
//-----------------------------------------------------------------------------

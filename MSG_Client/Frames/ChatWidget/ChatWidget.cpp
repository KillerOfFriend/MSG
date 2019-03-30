#include "ChatWidget.h"
#include "ui_ChatWidget.h"

//-----------------------------------------------------------------------------
/**
 * @brief TChatWidget::TChatWidget - Конструктор по умолчанию
 * @param inParent - Предок
 */
TChatWidget::TChatWidget(QWidget *inParent) :
    QWidget(inParent),
    ui(new Ui::TChatWidget)
{
    ui->setupUi(this);
    ui->ChatMessagePlainTextEdit->installEventFilter(this); // Перехватываем боработку событий
}
//-----------------------------------------------------------------------------
/**
 * @brief TChatWidget::~TChatWidget - Деструктор
 */
TChatWidget::~TChatWidget()
{
    delete ui;
}
//-----------------------------------------------------------------------------
/**
 * @brief TChatWidget::setChatUuid - Метод задаст Uuid беседы данного виджета
 * @param inChatUuid - Uuid беседы
 */
void TChatWidget::setChatUuid(QUuid inChatUuid)
{ fChatUuid = inChatUuid; }
//-----------------------------------------------------------------------------
/**
 * @brief TChatWidget::chatUuid - Метод вернёт Uuid беседы
 * @return Вернёт Uuid беседы
 */
QUuid TChatWidget::chatUuid() const
{ return fChatUuid; }
//-----------------------------------------------------------------------------
/**
 * @brief TChatWidget::eventFilter - Фильтр событий
 * @param inObject - Объект\генератор события
 * @param inEvent - Событие
 * @return Вернёт признак обработки события
 */
bool TChatWidget::eventFilter(QObject *inObject, QEvent *inEvent)
{
    if (inObject == ui->ChatMessagePlainTextEdit && inEvent->type() == QEvent::KeyPress) // Если ChatMessagePlainTextEdit генерирует событие нажатия кнопки
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(inEvent); // Приводим полученое событие к событию нажатия кнопки
        if (keyEvent->key() == Qt::Key_Return) // Если нажатая кнопка это ENTER
        {
            // Special tab handling
            qDebug("Enter Key Pressed...");
            return true; // Взводим флаг обработанности события
        }
        else
            return QWidget::eventFilter(inObject, inEvent); // В противном случаи отправляем событие на обработку предку
    }
    else
        return QWidget::eventFilter(inObject, inEvent); // В противном случаи отправляем событие на обработку предку
}

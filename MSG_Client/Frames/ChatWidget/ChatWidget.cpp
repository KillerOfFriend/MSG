#include "ChatWidget.h"
#include "ui_ChatWidget.h"

#include "Classes/DataModule/DataModule.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QImage>
#include <QFile>

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

    //QRegion CirculeButtomRegion(QRect(0,0,30,30), QRegion::Ellipse);
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
 * @brief TChatWidget::sendMessage - Метод отправит сообщение
 * @param inMessage - Сформированное сообщение
 */
void TChatWidget::sendMessage(Core::TChatMessage &inMessage)
{
    // Защита от дурака
    if (inMessage.messageType() == Core::MSGType::Unknown) // Если тип сообщения неизвестен (НЕ ДОЛЖНО ТАКОГО БЫТЬ!)
        QMessageBox::critical(this, tr("Ошибка"), tr("Попытка отправки несформированного сообщения!"));
    else // Тип сообщения корректен
    {
        if (!TDM::Instance().Client()->sendMessage(fChatUuid, inMessage)) // Отправляем сообщение
            QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось отправить сообщение!"));
        else
            ui->ChatMessagePlainTextEdit->clear(); // Очищаем поле ввода
    }
}
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
            on_btnSendTextMessage_clicked(); // Вызываем отправку сообщения
            return true; // Взводим флаг обработанности события
        }
        else
            return QWidget::eventFilter(inObject, inEvent); // В противном случаи отправляем событие на обработку предку
    }
    else
        return QWidget::eventFilter(inObject, inEvent); // В противном случаи отправляем событие на обработку предку
}
//-----------------------------------------------------------------------------
void TChatWidget::on_btnSendTextMessage_clicked()
{
    Core::TChatMessage ChatMessage(this); // Создаём новое сообщение

    // Идентификатор задаст БД ChatMessage.setMessageUuid(QUuid::createUuid()); // Задаём сообщению идентификатор
    ChatMessage.setMessageType(Core::MSGType::Text); // Задаём тип сообщения "Текст"
    ChatMessage.setMessageData(ui->ChatMessagePlainTextEdit->toPlainText().toUtf8()); // Задаём текст сообщения
    ChatMessage.setMessageSender(TDM::Instance().Client()->userAccount()->userInfo()->userUuid()); // Задаём Uuid отправителя
    ChatMessage.setMessageTimeCode(QDateTime::currentDateTime()); // Задаём таймкод

    sendMessage(ChatMessage); // Отправляем сформированное сообщение
}
//-----------------------------------------------------------------------------
void TChatWidget::on_btnSendFile_clicked()
{
    QString FileFilter = tr("Все файлы (*.*)");
    QFile SendingFile(QFileDialog::getOpenFileName(this, tr("Выбор файла для отправки"), "", FileFilter));

    if (SendingFile.exists()) // Если выбранный файл существует (или вообще выбран)
    {
        if(!SendingFile.open(QIODevice::ReadOnly)) // Файл не открылся
            QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось открыть файл: ") + SendingFile.fileName());
        else // Файл открылся
        {
            Core::TChatMessage ChatMessage(this); // Создаём новое сообщение
            ChatMessage.setMessageType(Core::MSGType::File); // Задаём тип сообщения "Текст"
            ChatMessage.setMessageData(SendingFile.readAll()); // Загружаем файл
            ChatMessage.setMessageSender(TDM::Instance().Client()->userAccount()->userInfo()->userUuid()); // Задаём Uuid отправителя
            ChatMessage.setMessageTimeCode(QDateTime::currentDateTime()); // Задаём таймкод

            SendingFile.close(); // Закрываем файл

            sendMessage(ChatMessage); // Отправляем сформированное сообщение
        }
    }
}
//-----------------------------------------------------------------------------
void TChatWidget::on_btnSendImage_clicked()
{
    QString ImageFilter = tr("BMP (*.bmp);;PNG (*.png);;JPEG (*.jpg *.jpeg);;TIFF (*.tif)");
    QFile SendingFile(QFileDialog::getOpenFileName(this, tr("Выбор изображения для отправки"), "", ImageFilter));

    if (SendingFile.exists()) // Если выбранный файл существует (или вообще выбран)
    {
        QImage LoadTest; // Преременная типа изображения для проверки открытия изображения

        if(!SendingFile.open(QIODevice::ReadOnly) || !LoadTest.loadFromData(SendingFile.readAll())) // Файл не открылся или не считался корректно
            QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось открыть изображение: ") + SendingFile.fileName());
        else // Файл открылся
        {
            Core::TChatMessage ChatMessage(this); // Создаём новое сообщение
            ChatMessage.setMessageType(Core::MSGType::Image); // Задаём тип сообщения "Текст"
            ChatMessage.setMessageData(SendingFile.readAll()); // Загружаем файл
            ChatMessage.setMessageSender(TDM::Instance().Client()->userAccount()->userInfo()->userUuid()); // Задаём Uuid отправителя
            ChatMessage.setMessageTimeCode(QDateTime::currentDateTime()); // Задаём таймкод

            SendingFile.close(); // Закрываем файл

            sendMessage(ChatMessage); // Отправляем сформированное сообщение
        }
    }
}
//-----------------------------------------------------------------------------

#include "MainFrame.h"
#include "ui_MainFrame.h"

#include <QMessageBox>

#include "resultcodes.h"
#include "Classes/DataModule/DataModule.h"
#include "Dialogs/UserViewDialog/UserViewDialog.h"

//-----------------------------------------------------------------------------
TfmeMainFrame::TfmeMainFrame(QWidget *inParent) :
    QWidget(inParent),
    ui(new Ui::TfmeMainFrame)
{
    ui->setupUi(this);

    init();
    Link();
}
//-----------------------------------------------------------------------------
TfmeMainFrame::~TfmeMainFrame()
{
    delete ui;
}
//-----------------------------------------------------------------------------
/**
 * @brief TfmeMainFrame::init - Метод инициализирует фрейм
 */
void TfmeMainFrame::init()
{
    TDM& DM = TDM::Instance();
    ui->UserLoginLabel->setText("<html><head/><body><p><span style=\" font-weight:600; color:#5500ff;\">" + DM.UserAccount()->UserInfo.userLogin() + "</span></p></body></html>");
    ui->UserNameLabel->setText("<html><head/><body><p><span style=\" font-weight:600; color:#ffaa00;\">" + DM.UserAccount()->UserInfo.userName() + "</span></p></body></html>");

    ui->ContactsListView->setModel(DM.UserAccount()->Contacts().get());
    fFoundUsers.reset(new TUsersModel(this));
    ui->ContactsFindListView->setModel(fFoundUsers.get());
}
//-----------------------------------------------------------------------------
/**
 * @brief TfmeMainFrame::Link - Метод слинкует сигналы со слотами
 */
void TfmeMainFrame::Link()
{
    connect(TDM::Instance().Client()->ComandExecutor.get(), &TComandExecutor::sig_FindUsersResult, this, &TfmeMainFrame::slot_FindUsersRes);
    connect(TDM::Instance().Client()->ComandExecutor.get(), &TComandExecutor::sig_AddContactResult, this, &TfmeMainFrame::slot_AddContactRes);

    connect(ui->ContactsFindLineEdit, &QLineEdit::returnPressed, this, &TfmeMainFrame::slot_FindUsers);
}
//-----------------------------------------------------------------------------
// SLOTS
//-----------------------------------------------------------------------------
/**
 * @brief TfmeMainFrame::slot_UserViewDialogResult - Слот, реагирует на результат диалога просмотра пользователя
 * @param inUserInfo - Просматриваемый пользователь
 * @param inResult - результат работы диалога
 */
void TfmeMainFrame::slot_UserViewDialogResult(const TUserInfo &inUserInfo, qint32 inResult)
{
    TDM& DM = TDM::Instance();

    switch(inResult)
    {
        case TUserViewDialog::rbAdd: { DM.Client()->addContact(DM.UserAccount()->UserInfo.userUuid(), inUserInfo.userUuid()); break; };
        case TUserViewDialog::rbRemove: { break; };
        case TUserViewDialog::rbSendMsg: { break; }
        default: break;
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TfmeMainFrame::slot_FindUsers - Слот реагирует на запрос поиска пользователей
 */
void TfmeMainFrame::slot_FindUsers()
{
    QLineEdit* Sender = qobject_cast<QLineEdit*>(QObject::sender());

    if (Sender)
    {
        if (Sender->text().isEmpty())
            fFoundUsers->clear();
        else TDM::Instance().Client()->findUsers(Sender->text());
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TfmeMainFrame::slot_FindUsersRes - Слот, получающий результат поиска пользователей
 * @param inUsers - Найденые пользователи
 */
void TfmeMainFrame::slot_FindUsersRes(const QList<TUserInfo> &inUsers)
{
    fFoundUsers->clear();
    (*fFoundUsers) << inUsers;
    fFoundUsers->dataChanged(fFoundUsers->index(0,0), fFoundUsers->index(fFoundUsers->rowCount(),0));
}
//-----------------------------------------------------------------------------
/**
 * @brief TfmeMainFrame::slot_AddContactRes - Слот, получающий результат добавления контакта
 * @param inResult - Результат выполнения
 */
void TfmeMainFrame::slot_AddContactRes(qint32 inResult)
{
    switch (inResult)
    {
        case Res::AddContact::acCreated:
        {
            QMessageBox::information(this, tr("Сообщение"), tr("Контакт успешно добавлен"));
            TDM &DM = TDM::Instance();
            DM.Client()->getContacts(DM.UserAccount()->UserInfo.userUuid()); // Запрашиваем список контактов
            break;
        };
        case Res::AddContact::acAlredyExist: { QMessageBox::warning(this, tr("Предупреждение"), tr("Контакт уже существует!")); break; };
        default: { QMessageBox::critical(this, tr("Ошибка"), tr("Произошла непредвиденная ошибка!")); break; };
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TfmeMainFrame::on_ContactsFindListView_doubleClicked(const QModelIndex &index)
{
    auto OtherUserIt = fFoundUsers->begin();
    std::advance(OtherUserIt, index.row()); // Получаем итератор на выбранного пользователя

    TUserViewDialog UserViewDialog((*OtherUserIt), this); // Инициализируем диалог просмотра пользователя
    UserViewDialog.setModal(true); // Задаём подальность

    auto ContactsPtr = TDM::Instance().UserAccount()->Contacts();
    auto FindRes = std::find(ContactsPtr->begin(), ContactsPtr->end(), (*OtherUserIt)); // Ищим пользователя в списке контактов

    if (FindRes != ContactsPtr->end()) // Если найден
        UserViewDialog.setButtons(TUserViewDialog::eResButtons(TUserViewDialog::rbRemove | TUserViewDialog::rbSendMsg)); // Разрешаем удалить и написать
    else // Если не найден
        UserViewDialog.setButtons(TUserViewDialog::rbAdd); // Разрешаем добавить

    connect(&UserViewDialog, &TUserViewDialog::sig_Result, this, &TfmeMainFrame::slot_UserViewDialogResult);
    UserViewDialog.exec(); // Вызываем диалог
    disconnect(&UserViewDialog, &TUserViewDialog::sig_Result, this, &TfmeMainFrame::slot_UserViewDialogResult);

}
//-----------------------------------------------------------------------------

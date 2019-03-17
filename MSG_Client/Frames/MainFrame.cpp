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

    ui->UserNameLabel->setText("<html><head/><body><p><span style=\" font-weight:600; color:#ffaa00;\">" + DM.UserAccount()->userInfo()->userName() + "</span></p></body></html>");
    ui->UserLoginLabel->setText("<html><head/><body><p><span style=\" font-weight:600; color:#5500ff;\">" + DM.UserAccount()->userInfo()->userLogin() + "</span></p></body></html>");

    fUserListDelegate.reset(new TUserItemDelegate()); // Инициализируем делегат отображения
    fFoundUsers.reset(new TUsersModel(this)); // Инициализируем модель найденых юзеров

    fUserProxyModel.reset(new TUsersProxyModel(this)); // Инициализация фильтров
    fFounUsersProxyModel.reset(new QSortFilterProxyModel(this));

    fUserProxyModel->setSourceModel(DM.UserAccount()->contacts().get()); // Задаём модель-источник фильтру списку контактов
    fFounUsersProxyModel->setSourceModel(fFoundUsers.get()); // Задаём модель-источник фильтру списка найденых пользователей

    ui->ContactsListView->setItemDelegate(fUserListDelegate.get()); // Задаём делегат отображения
    ui->ContactsFindListView->setItemDelegate(fUserListDelegate.get()); // Задаём делегат отображения

    ui->ContactsListView->setModel(fUserProxyModel.get()); // Задаём фильтрующую модель контактов
    ui->ContactsFindListView->setModel(fFounUsersProxyModel.get()); // Задаём модель найденых пользователей

    fUserProxyModel->sort(TUsersModel::eColumns::cUserName); // Сортировка списка контактов по имени
    fFounUsersProxyModel->sort(TUsersModel::eColumns::cUserName); // Сортировка списка найденых пользователей по имени
}
//-----------------------------------------------------------------------------
/**
 * @brief TfmeMainFrame::Link - Метод слинкует сигналы со слотами
 */
void TfmeMainFrame::Link()
{
    connect(TDM::Instance().Client().get(), &TMSGClient::sig_FindUsersResult, this, &TfmeMainFrame::slot_FindUsersRes);
    connect(TDM::Instance().Client().get(), &TMSGClient::sig_AddContactResult, this, &TfmeMainFrame::slot_AddContactRes);
    connect(TDM::Instance().Client().get(), &TMSGClient::sig_DeleteContactResult, this, &TfmeMainFrame::slot_DeleteContactRes);

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
void TfmeMainFrame::slot_UserViewDialogResult(const Users::TUserInfo &inUserInfo, qint32 inResult)
{
    TDM& DM = TDM::Instance();

    switch(inResult)
    {
        case TUserViewDialog::rbAdd:
    {
        if (!DM.Client()->addContact(DM.UserAccount()->userInfo()->userUuid(), inUserInfo.userUuid()))
            QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось добавить контакт!"));
        break;
    };
        case TUserViewDialog::rbRemove:
        {
            if (!DM.Client()->deleteContact(DM.UserAccount()->userInfo()->userUuid(), inUserInfo.userUuid()))
                QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось удалить контакт!"));
            break;
        };
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
        else
            if (!TDM::Instance().Client()->findUsers(Sender->text()))
                QMessageBox::critical(this, tr("Ошибка"), tr("Поиск контактов не удался!"));
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TfmeMainFrame::slot_FindUsersRes - Слот, получающий результат поиска пользователей
 * @param inUsers - Найденые пользователи
 */
void TfmeMainFrame::slot_FindUsersRes(const QList<Users::TUserInfo> &inUsers)
{
    fFoundUsers->clear();

    std::for_each(inUsers.begin(), inUsers.end(), [&](const Users::TUserInfo &Info)
    {
        fFoundUsers->insert(std::make_pair(Info.userUuid(), Info));
    });

    fFoundUsers->dataChanged(fFoundUsers->index(0,0), fFoundUsers->index(fFoundUsers->rowCount(),0));
}
//-----------------------------------------------------------------------------
/**
 * @brief TfmeMainFrame::slot_AddContactRes - Слот, получающий результат добавления контакта
 * @param inResult - Результат выполнения
 */
void TfmeMainFrame::slot_AddContactRes(qint32 inResult, Users::TUserInfo &inContactInfo)
{
    switch (inResult)
    {
        case Res::AddContact::acCreated:
        {
            TDM::Instance().UserAccount()->contacts()->insert(std::make_pair(inContactInfo.userUuid(), inContactInfo)); // Добавляем контакт в список
            QMessageBox::information(this, tr("Сообщение"), tr("Контакт успешно добавлен"));
            break;
        };
        case Res::AddContact::acAlredyExist: { QMessageBox::warning(this, tr("Предупреждение"), tr("Контакт уже существует!")); break; };
        default: { QMessageBox::critical(this, tr("Ошибка"), tr("Произошла непредвиденная ошибка!")); break; };
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TfmeMainFrame::slot_DeleteContactRes - Слот, получающий результат удаления контакта
 * @param inResult - Результат выполнения
 */
void TfmeMainFrame::slot_DeleteContactRes(qint32 inResult, QUuid &inContactUuid)
{
    switch (inResult)
    {
        case Res::DeleteContact::dcContactRemove:
        {
            TDM::Instance().UserAccount()->contacts()->erase(inContactUuid);
            QMessageBox::information(this, tr("Сообщение"), tr("Контакт успешно удалён"));
            break;
        };
        case Res::DeleteContact::dcContactNotFound: {QMessageBox::warning(this, tr("Предупреждение"), tr("Не удалось найти пользователя!")); break; }
        default: { QMessageBox::critical(this, tr("Ошибка"), tr("Что то пошло не так!")); break; };
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TfmeMainFrame::on_ContactsFindListView_doubleClicked(const QModelIndex &index)
{
    auto OtherUserIt = fFoundUsers->begin();
    QModelIndex UserIndex = fFounUsersProxyModel->mapToSource(index); // Полученый индекс фильтрующей модели приводим к индексу модели источника
    std::advance(OtherUserIt, UserIndex.row()); // Получаем итератор на выбранного пользователя

    TUserViewDialog UserViewDialog(OtherUserIt->second, this); // Инициализируем диалог просмотра пользователя
    UserViewDialog.setModal(true); // Задаём подальность

    TDM& DM = TDM::Instance();
    auto FindRes = DM.UserAccount()->contacts()->find(OtherUserIt->second.userUuid()); // Ищим пользователя в списке контактов

    if (FindRes != DM.UserAccount()->contacts()->end()) // Если найден
        UserViewDialog.setButtons(TUserViewDialog::eResButtons(TUserViewDialog::rbRemove | TUserViewDialog::rbSendMsg)); // Разрешаем удалить и написать
    else // Если не найден
        UserViewDialog.setButtons(TUserViewDialog::rbAdd); // Разрешаем добавить

    connect(&UserViewDialog, &TUserViewDialog::sig_Result, this, &TfmeMainFrame::slot_UserViewDialogResult);
    UserViewDialog.exec(); // Вызываем диалог
    disconnect(&UserViewDialog, &TUserViewDialog::sig_Result, this, &TfmeMainFrame::slot_UserViewDialogResult);
}
//-----------------------------------------------------------------------------
void TfmeMainFrame::on_ContactsListView_doubleClicked(const QModelIndex &index)
{
    auto OtherUserIt = TDM::Instance().UserAccount()->contacts()->begin();
    QModelIndex ContactIndex = fUserProxyModel->mapToSource(index); // Полученый индекс фильтрующей модели приводим к индексу модели источника
    std::advance(OtherUserIt, ContactIndex.row()); // Получаем итератор на выбраный контакт

    TUserViewDialog UserViewDialog(OtherUserIt->second, this); // Инициализируем диалог просмотра пользователя
    UserViewDialog.setModal(true); // Задаём подальность

    UserViewDialog.setButtons(TUserViewDialog::eResButtons(TUserViewDialog::rbRemove | TUserViewDialog::rbSendMsg)); // Разрешаем удалить и написать

    connect(&UserViewDialog, &TUserViewDialog::sig_Result, this, &TfmeMainFrame::slot_UserViewDialogResult);
    UserViewDialog.exec(); // Вызываем диалог
    disconnect(&UserViewDialog, &TUserViewDialog::sig_Result, this, &TfmeMainFrame::slot_UserViewDialogResult);
}
//-----------------------------------------------------------------------------
void TfmeMainFrame::on_ContactsFilterLineEdit_textChanged(const QString &arg1)
{
    fUserProxyModel->setFilter(TUsersModel::eColumns::cUserName, arg1);
}
//-----------------------------------------------------------------------------

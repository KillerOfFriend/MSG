#include "MainFrame.h"
#include "ui_MainFrame.h"

#include <QMessageBox>
#include <QMenu>

#include "resultcodes.h"
#include "Classes/DataModule/DataModule.h"
#include "Dialogs/UserViewDialog/UserViewDialog.h"
#include "Frames/ChatWidget/ChatWidget.h"
#include "Dialogs/UserListDialog/UserListDialog.h"

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

    ui->UserHeaderWidget->slot_SetUserAvatar(DM.UserAccount()->userInfo()->userAvatar(), DM.UserAccount()->userInfo()->userIsMale());
    ui->UserHeaderWidget->slot_SetUserName(DM.UserAccount()->userInfo()->userName());
    ui->UserHeaderWidget->slot_SetUserLogin(DM.UserAccount()->userInfo()->userLogin());

    ui->LogInfoListView->setModel(DM.Models()->LogModel().get()); // Задаём модель логов клиента

    fUserListDelegate.reset(new TUserItemDelegate()); // Инициализируем делегат отображения пользователей
    fChatListDelegate.reset(new TChatItemDelegate()); // Инициализируем делегат отображения бесед
    fFoundUsers.reset(new TUsersModel(this)); // Инициализируем модель найденых юзеров

    fUserProxyModel.reset(new TUsersProxyModel(this)); // Инициализация фильтров
    fFounUsersProxyModel.reset(new QSortFilterProxyModel(this));

    fUserProxyModel->setSourceModel(DM.UserAccount()->contacts().get()); // Задаём модель-источник фильтру списку контактов
    fFounUsersProxyModel->setSourceModel(fFoundUsers.get()); // Задаём модель-источник фильтру списка найденых пользователей

    ui->ContactsListView->setItemDelegate(fUserListDelegate.get()); // Задаём делегат отображения контактов
    ui->ContactsFindListView->setItemDelegate(fUserListDelegate.get()); // Задаём делегат отображения пользователей

    ui->ContactsListView->setModel(fUserProxyModel.get()); // Задаём фильтрующую модель контактов
    ui->ContactsFindListView->setModel(fFounUsersProxyModel.get()); // Задаём модель найденых пользователей

    ui->ChatListView->setItemDelegate(fChatListDelegate.get()); //  Задаём делегат отображения бесед
    ui->ChatListView->setModel(DM.UserAccount()->chats().get()); // Задаём модель данных бесед

    fUserProxyModel->sort(TUsersModel::eColumns::cUserName); // Сортировка списка контактов по имени
    fFounUsersProxyModel->sort(TUsersModel::eColumns::cUserName); // Сортировка списка найденых пользователей по имени

    ui->ChatListView->setContextMenuPolicy(Qt::CustomContextMenu);
}
//-----------------------------------------------------------------------------
/**
 * @brief TfmeMainFrame::Link - Метод слинкует сигналы со слотами
 */
void TfmeMainFrame::Link()
{
    connect(ui->LogInfoListView->model(), &QAbstractItemModel::rowsInserted, ui->LogInfoListView, &QListView::scrollToBottom);

    connect(TDM::Instance().Client().get(), &TMSGClient::sig_FindUsersResult, this, &TfmeMainFrame::slot_FindUsersRes);
    connect(ui->ChatTabWidget, &QTabWidget::tabCloseRequested, this, &TfmeMainFrame::slot_CloseTab);

    connect(ui->ContactsFindLineEdit, &QLineEdit::returnPressed, this, &TfmeMainFrame::slot_FindUsers);
}
//-----------------------------------------------------------------------------
// SLOTS
//-----------------------------------------------------------------------------
/**
 * @brief slot_CloseTab - Слот, реагирует на сигнал закрытия вкладки
 * @param inTabIndex - Индекс вкладки
 */
void TfmeMainFrame::slot_CloseTab(qint32 inTabIndex)
{
    if (inTabIndex != 0) // Самая аервая вкладки информационная, её закрывать нельзя
    {
        TChatWidget* ChatWidget = qobject_cast<TChatWidget*>(ui->ChatTabWidget->widget(inTabIndex)); // Получаем виджет по индексу и приводим к чатвиджету

        if (ChatWidget != nullptr) // Если приведение успешно
        {
            fOpenChatTabs.erase(ChatWidget->chatUuid()); // Удаляем данные о вкладке по Uuid чата
            ui->ChatTabWidget->removeTab(inTabIndex); // Удаляем вкладку
        }
    }
}
//-----------------------------------------------------------------------------
/**
 * @brief TfmeMainFrame::slot_UserViewDialogResult - Слот, реагирует на результат диалога просмотра пользователя
 * @param inUserInfo - Просматриваемый пользователь
 * @param inResult - результат работы диалога
 */
void TfmeMainFrame::slot_UserViewDialogResult(const Users::UserInfo_Ptr inUserInfo, qint32 inResult)
{
    TDM& DM = TDM::Instance();

    switch(inResult)
    {
        case TUserViewDialog::rbAdd:
        {
            if (!DM.Client()->addContact(DM.UserAccount()->userInfo()->userUuid(), inUserInfo->userUuid()))
                QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось добавить контакт!"));
            break;
        };
        case TUserViewDialog::rbRemove:
        {
            if (!DM.Client()->deleteContact(DM.UserAccount()->userInfo()->userUuid(), inUserInfo->userUuid()))
                QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось удалить контакт!"));
            break;
        };
        case TUserViewDialog::rbSendMsg:
        {
            QUuid ChatUuid; // Uuid беседы
            // Формируем Uuid приватной беседы
            // Uuid прививатной беседы формируется из суммы байтовых последовательностей самого клиента и клиента, которуму пишется
            // ВАЖНО! для правильной генерации Uuid с обеих сторон соединения, последовательность должна быть от меньшего к большему
            if (DM.UserAccount()->userInfo()->userUuid() < inUserInfo->userUuid())
                ChatUuid = QUuid(DM.UserAccount()->userInfo()->userUuid().toByteArray() + inUserInfo->userUuid().toByteArray());
            else ChatUuid = QUuid(inUserInfo->userUuid().toByteArray() + DM.UserAccount()->userInfo()->userUuid().toByteArray());

            auto FindCharRes = DM.UserAccount()->chats()->find(ChatUuid); // Ищим беседу по UUid
            if (FindCharRes == DM.UserAccount()->chats()->end()) // Если беседа не найдена
            {
                Users::TChatInfo NewChat; // Новая беседа
                NewChat.setChatUuid(ChatUuid); // Задаём Uuid новой беседы
                NewChat.setChatName(DM.UserAccount()->userInfo()->userName() + "|" + inUserInfo->userName());
                NewChat.setChatPrivateStatus(true); // Помечаем беседу как приватную

                NewChat.addUser(DM.UserAccount()->userInfo());  // Добавляем "Себя"
                NewChat.addUser(inUserInfo); // Добавляем пользователя

                DM.Client()->createChat(NewChat); // Шлём команду на создание приватной беседы
            }
            else // Если беседа найдена
            {
                auto FindRes = fOpenChatTabs.find(ChatUuid);
                if (FindRes != fOpenChatTabs.end())
                    ui->ChatTabWidget->setCurrentIndex(FindRes->second);
            }

            break;
        };
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
        fFoundUsers->insert(std::make_pair(Info.userUuid(), std::make_shared<Users::TUserInfo>(Info)));
    });

    fFoundUsers->dataChanged(fFoundUsers->index(0,0), fFoundUsers->index(fFoundUsers->rowCount(),0));
}
//-----------------------------------------------------------------------------
void TfmeMainFrame::slot_ChatAddNew() // Слот вызывает добавление ногвой беседы
{
//    TUserListDialog UserListDialog(this);
//    UserListDialog.exec();

//    if (UserListDialog.result() == QDialog::DialogCode::Accepted) // Если в диалоге было подтвреждение
//    {
//        Users::TChatInfo NewChat; // Новая беседа
//        NewChat.setChatUuid(QUuid::createUuid()); // Генерируем Uuid новой беседы
//        NewChat.setChatPrivateStatus(false); // Помечаем беседу как публичную

//        TDM &DM = TDM::Instance();

//        QString ChatName = DM.UserAccount()->userInfo()->userName(); // Создаём имя беседы
//        NewChat.addUser(DM.UserAccount()->userInfo()->userUuid()); // Добавляем в беседу себя

//        QList<QUuid> Users = UserListDialog.selectedUsersUuids(); // Получаем список Uuid'ов выбранных пользователей

//        std::for_each(Users.begin(), Users.end(), [&](const QUuid &UserUuid) // Перебераем список выбранных пользователей
//        {
//            auto FindRes = DM.UserAccount()->contacts()->find(UserUuid);
//            ChatName += QString("|" + FindRes->second.userName()); // Формируем имя беседы

//            NewChat.addUser(UserUuid); // Добавляем пользователя в беседу
//        });

//        NewChat.setChatName(ChatName); // Задаём имя беседы

//        DM.Client()->createChat(NewChat); // Шлём команду на создание публичной беседы
//    }
}
//-----------------------------------------------------------------------------
void TfmeMainFrame::slot_ChatDeleteCurrent() // Слот вызывает удаление выбранной беседы
{
    int DeleteChat = 1;
}
//-----------------------------------------------------------------------------
void TfmeMainFrame::slot_ChatOpenCurrent() // Слот вызывает открытие выбранной беседы
{
    int OpenChat = 2;
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
    auto FindRes = DM.UserAccount()->contacts()->find(OtherUserIt->second->userUuid()); // Ищим пользователя в списке контактов

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
void TfmeMainFrame::on_ChatListView_doubleClicked(const QModelIndex &index)
{
    QModelIndex ChatUuidIndex = index.sibling(index.row(), TChatsModel::eColumns::cChatUuid); // Перемещаем индекс на "поле" Uuid беседы

    if (!ChatUuidIndex.isValid())
        return;

    qint32 TabIndex = 0;
    auto It = TDM::Instance().UserAccount()->chats()->find(ChatUuidIndex.data().toUuid());

    if (It != TDM::Instance().UserAccount()->chats()->end())
    {
        auto FindRes = fOpenChatTabs.find(It->second->chatUuid()); // Ищим среди открытых вкладок беседу

        if (FindRes != fOpenChatTabs.end()) // Если вкладка уже существует
            TabIndex = FindRes->second; // Запоминаем номер открытой вкладки
        else // Если вкладки с этой беседой нет
        {
            TChatWidget* NewChatWidget = new TChatWidget(ui->ChatTabWidget); // Создаём виджет чата
            NewChatWidget->setChatUuid(It->second->chatUuid()); // Задаём ему Uuid

            TabIndex = ui->ChatTabWidget->addTab(NewChatWidget, It->second->chatName()); // Создаём новую вкладку и получаем её индекс
            fOpenChatTabs.insert(std::make_pair(It->second->chatUuid(), TabIndex)); // Добавляем индекс новой вкладки в список открытых
        }
    }

    ui->ChatTabWidget->setCurrentIndex(TabIndex); // Делаем вкладку активной
}
//-----------------------------------------------------------------------------
void TfmeMainFrame::on_ChatListView_customContextMenuRequested(const QPoint &pos)
{
    QPoint globalPos = ui->ChatListView->mapToGlobal(pos);

    QMenu ChatMenu; // Создаём меню
    QModelIndex Index = ui->ChatListView->indexAt(pos); // Пытаемся получить индекс на нажатый айтем

    QAction* AddChatAction = ChatMenu.addAction(tr("Добавить беседу"), this, SLOT(slot_ChatAddNew()));
    AddChatAction->setIcon(QIcon(":/Resurse/ChatsPopupMenu/Images/ChatsPopupMenu/NewChat.png"));

    if (Index.isValid()) // Если попали по айтему добавляем кнопки на удаление и открытие
    {
        QAction* DeleteChatAction = ChatMenu.addAction(tr("Удалить беседу"), this, SLOT(slot_ChatDeleteCurrent()));
        DeleteChatAction->setIcon(QIcon(":/Resurse/ChatsPopupMenu/Images/ChatsPopupMenu/DeleteChat.png"));
        QAction* OpenChatAction = ChatMenu.addAction(tr("Открыть и написать"), this, SLOT(slot_ChatOpenCurrent()));
        OpenChatAction->setIcon(QIcon(":/Resurse/ChatsPopupMenu/Images/ChatsPopupMenu/SendMessageToChat.png"));
    }

    ChatMenu.exec(globalPos);
}
//-----------------------------------------------------------------------------

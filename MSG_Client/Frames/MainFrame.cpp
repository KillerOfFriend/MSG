#include "MainFrame.h"
#include "ui_MainFrame.h"

#include <QMessageBox>
#include <QInputDialog>
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

    ui->UserHeaderWidget->slot_SetUserAvatar(DM.Client()->userAccount()->userInfo()->userAvatar(), DM.Client()->userAccount()->userInfo()->userIsMale());
    ui->UserHeaderWidget->slot_SetUserName(DM.Client()->userAccount()->userInfo()->userName());
    ui->UserHeaderWidget->slot_SetUserLogin(DM.Client()->userAccount()->userInfo()->userLogin());

    ui->LogInfoListView->setModel(DM.Models()->LogModel().get()); // Задаём модель логов клиента

    fUserListDelegate.reset(new TUserItemDelegate()); // Инициализируем делегат отображения пользователей
    fChatListDelegate.reset(new TChatItemDelegate()); // Инициализируем делегат отображения бесед
    fFoundUsers.reset(new TUsersModel(this)); // Инициализируем модель найденых юзеров

    fUsersProxyModel.reset(new TUsersProxyModel(this)); // Инициализация фильтров
    fChatsProxyModel.reset(new TChatsProxyModel(this));
    fFounUsersProxyModel.reset(new QSortFilterProxyModel(this));

    fUsersProxyModel->setSourceModel(DM.Client()->userAccount()->contacts().get()); // Задаём модель-источник фильтру списку контактов
    fChatsProxyModel->setSourceModel(DM.Client()->userAccount()->chats().get());
    fFounUsersProxyModel->setSourceModel(fFoundUsers.get()); // Задаём модель-источник фильтру списка найденых пользователей

    ui->ContactsListView->setItemDelegate(fUserListDelegate.get()); // Задаём делегат отображения контактов
    ui->ContactsFindListView->setItemDelegate(fUserListDelegate.get()); // Задаём делегат отображения пользователей

    ui->ContactsListView->setModel(fUsersProxyModel.get()); // Задаём фильтрующую модель контактов
    ui->ContactsFindListView->setModel(fFounUsersProxyModel.get()); // Задаём модель найденых пользователей

    ui->ChatListView->setItemDelegate(fChatListDelegate.get()); //  Задаём делегат отображения бесед
    ui->ChatListView->setModel(fChatsProxyModel.get()); // Задаём модель данных бесед

    fUsersProxyModel->sort(TUsersModel::eColumns::cUserName); // Сортировка списка контактов по имени
    fFounUsersProxyModel->sort(TUsersModel::eColumns::cUserName); // Сортировка списка найденых пользователей по имени
    // Настраиваем политики меню
    ui->ContactsListView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->ChatListView->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->ContactsTabWidget->setCurrentIndex(eContactsTabs::ctContacts); // Устанавливаем текущую вкладку
}
//-----------------------------------------------------------------------------
/**
 * @brief TfmeMainFrame::Link - Метод слинкует сигналы со слотами
 */
void TfmeMainFrame::Link()
{
    connect(ui->LogInfoListView->model(), &QAbstractItemModel::rowsInserted, ui->LogInfoListView, &QListView::scrollToBottom); // Автопрокрутка логов

    connect(TDM::Instance().Client().get(), &TMSGClient::sig_FindUsersResult, this, &TfmeMainFrame::slot_FindUsersRes); // Получение результатов поиска контактов
    connect(TDM::Instance().Client()->userAccount()->chats().get(), &TChatsModel::sig_chatDeleting, this, &TfmeMainFrame::slot_CloseChat); // Закрытие беседы (удалённой)

    connect(ui->ChatTabWidget, &QTabWidget::tabCloseRequested, this, &TfmeMainFrame::slot_CloseTab); // Закрытие вкладки беседы (крестом)
    connect(ui->ContactsFindLineEdit, &QLineEdit::returnPressed, this, &TfmeMainFrame::slot_FindUsers); // Вызов поиска контактов
}
//-----------------------------------------------------------------------------
// SLOTS
//-----------------------------------------------------------------------------
/**
 * @brief TfmeMainFrame::slot_CloseChat - Слот, реагирующий на закрытие беседы (Как правило удалённой)
 * @param inChatUuid - Uuid закрываемой беседы
 */
void TfmeMainFrame::slot_CloseChat(QUuid inChatUuid)
{
    auto FindRes = fOpenChatTabs.find(inChatUuid); // Ищим открытую беседу (вкладку)

    if (FindRes != fOpenChatTabs.end()) // Если открытая беседа найдена
        slot_CloseTab(ui->ChatTabWidget->indexOf(FindRes->second)); // Вызываем закрытие вкладки беседы
}
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
void TfmeMainFrame::slot_UserViewDialogResult(const Core::UserInfo_Ptr inUserInfo, qint32 inResult)
{
    TDM& DM = TDM::Instance();

    switch(inResult)
    {
        case TUserViewDialog::rbAdd:
        {
            if (!DM.Client()->addContact(DM.Client()->userAccount()->userInfo()->userUuid(), inUserInfo->userUuid()))
                QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось добавить контакт!"));
            break;
        };
        case TUserViewDialog::rbRemove:
        {
            if (!DM.Client()->deleteContact(DM.Client()->userAccount()->userInfo()->userUuid(), inUserInfo->userUuid()))
                QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось удалить контакт!"));
            break;
        };
        case TUserViewDialog::rbSendMsg:
        {
            QUuid ChatUuid; // Uuid беседы
            // Формируем Uuid приватной беседы
            // Uuid прививатной беседы формируется из суммы байтовых последовательностей самого клиента и клиента, которуму пишется
            // ВАЖНО! для правильной генерации Uuid с обеих сторон соединения, последовательность должна быть от меньшего к большему
            if (DM.Client()->userAccount()->userInfo()->userUuid() < inUserInfo->userUuid())
                ChatUuid = QUuid(DM.Client()->userAccount()->userInfo()->userUuid().toByteArray() + inUserInfo->userUuid().toByteArray());
            else ChatUuid = QUuid(inUserInfo->userUuid().toByteArray() + DM.Client()->userAccount()->userInfo()->userUuid().toByteArray());

            auto FindCharRes = DM.Client()->userAccount()->chats()->find(ChatUuid); // Ищим беседу по UUid
            if (FindCharRes == DM.Client()->userAccount()->chats()->end()) // Если беседа не найдена
            {
                Core::TChatInfo NewChat; // Новая беседа
                NewChat.setChatUuid(ChatUuid); // Задаём Uuid новой беседы
                NewChat.setChatName(DM.Client()->userAccount()->userInfo()->userName() + "|" + inUserInfo->userName());
                NewChat.setChatPrivateStatus(true); // Помечаем беседу как приватную

                NewChat.addUser(DM.Client()->userAccount()->userInfo());  // Добавляем "Себя"
                NewChat.addUser(inUserInfo); // Добавляем пользователя

                if(!DM.Client()->createChat(NewChat)) // Шлём команду на создание приватной беседы
                    QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось создать приватную беседу!"));
            }
            else // Если беседа найдена
            {
                auto FindRes = fOpenChatTabs.find(ChatUuid);
                if (FindRes != fOpenChatTabs.end())
                    ui->ChatTabWidget->setCurrentWidget(FindRes->second);
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
void TfmeMainFrame::slot_FindUsersRes(const QList<Core::TUserInfo> &inUsers)
{
    fFoundUsers->clear();

    std::for_each(inUsers.begin(), inUsers.end(), [&](const Core::TUserInfo &Info)
    {
        fFoundUsers->insert(std::make_pair(Info.userUuid(), std::make_shared<Core::TUserInfo>(Info)));
    });

    fFoundUsers->dataChanged(fFoundUsers->index(0,0), fFoundUsers->index(fFoundUsers->rowCount(),0));
}
//-----------------------------------------------------------------------------
void TfmeMainFrame::slot_ChatAddNew() // Слот вызывает добавление новой беседы
{
    QString ChatName = QInputDialog::getText(this, tr("Новая беседа"), tr("Укажите имя новой беседы"));
    if (ChatName.isEmpty())
        return;

    TUserListDialog UserListDialog(this);
    UserListDialog.exec();

    if (UserListDialog.result() == QDialog::DialogCode::Accepted) // Если в диалоге было подтвреждение
    {
        Core::TChatInfo NewChat; // Новая беседа
        NewChat.setChatUuid(QUuid::createUuid()); // Генерируем Uuid новой беседы
        NewChat.setChatPrivateStatus(false); // Помечаем беседу как публичную

        TDM &DM = TDM::Instance();

        //QString ChatName = DM.UserAccount()->userInfo()->userName(); // Создаём имя беседы
        NewChat.addUser(DM.Client()->userAccount()->userInfo()); // Добавляем в беседу себя

        QList<Core::UserInfo_Ptr> Users = UserListDialog.selectedUsers(); // Получаем список Uuid'ов выбранных пользователей

        std::for_each(Users.begin(), Users.end(), [&](const Core::UserInfo_Ptr &User) // Перебераем список выбранных пользователей
        {
            //ChatName += QString("|" + User->userName()); // Формируем имя беседы

            NewChat.addUser(User); // Добавляем пользователя в беседу
        });

        NewChat.setChatName(ChatName); // Задаём имя беседы

        if (!DM.Client()->createChat(NewChat)) // Шлём команду на создание публичной беседы
            QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось создать публичную беседу!"));
    }
}
//-----------------------------------------------------------------------------
void TfmeMainFrame::slot_ChatDeleteCurrent() // Слот вызывает удаление выбранной беседы
{
    TDM &DM = TDM::Instance();

    QModelIndex CurrentSelectChat = ui->ChatListView->currentIndex();
    QModelIndex UuidChatIndex = CurrentSelectChat.sibling(CurrentSelectChat.row(), TChatsModel::cChatUuid);

    if (UuidChatIndex.isValid()) // Елси индекс на Uuid валидный
        if (!DM.Client()->leaveFromChat(UuidChatIndex.data().toUuid())) // Посылаем команду на удаление беседы
            QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось удалить беседу!"));
}
//-----------------------------------------------------------------------------
void TfmeMainFrame::slot_ChatOpenCurrent() // Слот вызывает открытие выбранной беседы
{
    QModelIndex CurrentSelectChat = ui->ChatListView->currentIndex();
    QModelIndex ChatUuidIndex = CurrentSelectChat.sibling(CurrentSelectChat.row(), TChatsModel::eColumns::cChatUuid); // Перемещаем индекс на "поле" Uuid беседы

    if (!ChatUuidIndex.isValid())
        return;

    QWidget* TabWidget = nullptr;
    auto It = TDM::Instance().Client()->userAccount()->chats()->find(ChatUuidIndex.data().toUuid());

    if (It != TDM::Instance().Client()->userAccount()->chats()->end())
    {
        auto FindRes = fOpenChatTabs.find(It->second->chatUuid()); // Ищим среди открытых вкладок беседу

        if (FindRes != fOpenChatTabs.end()) // Если вкладка уже существует
            TabWidget = FindRes->second; // Запоминаем номер открытой вкладки
        else // Если вкладки с этой беседой нет
        {
            TChatWidget* NewChatWidget = new TChatWidget(ui->ChatTabWidget); // Создаём виджет чата
            NewChatWidget->setChatUuid(It->second->chatUuid()); // Задаём ему Uuid

            qint32 NewTabIndex = ui->ChatTabWidget->addTab(NewChatWidget, It->second->chatName()); // Создаём новую вкладку и получаем её индекс
            TabWidget = ui->ChatTabWidget->widget(NewTabIndex);
            fOpenChatTabs.insert(std::make_pair(It->second->chatUuid(), TabWidget)); // Добавляем новую вкладку в список открытых
        }
    }

    ui->ChatTabWidget->setCurrentWidget(TabWidget); // Делаем вкладку активной
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
    auto FindRes = DM.Client()->userAccount()->contacts()->find(OtherUserIt->second->userUuid()); // Ищим пользователя в списке контактов

    if (FindRes != DM.Client()->userAccount()->contacts()->end()) // Если найден
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
    auto OtherUserIt = TDM::Instance().Client()->userAccount()->contacts()->begin();
    QModelIndex ContactIndex = fUsersProxyModel->mapToSource(index); // Полученый индекс фильтрующей модели приводим к индексу модели источника
    std::advance(OtherUserIt, ContactIndex.row()); // Получаем итератор на выбраный контакт

    TUserViewDialog UserViewDialog(OtherUserIt->second, this); // Инициализируем диалог просмотра пользователя
    UserViewDialog.setModal(true); // Задаём подальность

    UserViewDialog.setButtons(TUserViewDialog::eResButtons(TUserViewDialog::rbRemove | TUserViewDialog::rbSendMsg)); // Разрешаем удалить и написать

    connect(&UserViewDialog, &TUserViewDialog::sig_Result, this, &TfmeMainFrame::slot_UserViewDialogResult);
    UserViewDialog.exec(); // Вызываем диалог
    disconnect(&UserViewDialog, &TUserViewDialog::sig_Result, this, &TfmeMainFrame::slot_UserViewDialogResult);
}
//-----------------------------------------------------------------------------
void TfmeMainFrame::on_ChatListView_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)
    slot_ChatOpenCurrent();
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
void TfmeMainFrame::on_ContactsFilterLineEdit_textChanged(const QString &arg1)
{
    fUsersProxyModel->setFilter(TUsersModel::eColumns::cUserName, arg1);
}
//-----------------------------------------------------------------------------
void TfmeMainFrame::on_ChatFilterLineEdit_textChanged(const QString &arg1)
{
    fChatsProxyModel->setFilter(TChatsModel::eColumns::cChatName, arg1);
}
//-----------------------------------------------------------------------------


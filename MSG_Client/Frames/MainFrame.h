#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <memory>
#include <map>

#include <QWidget>
#include <QList>
#include <QSortFilterProxyModel>

#include "Classes/UserHeaderWidget/UserHeaderWidget.h"
#include "Models/UsersModel/UsersModel.h"
#include "Models/UsersModel/UsersProxyModel.h"
#include "Models/ChatsModel/ChatsProxyModel.h"
#include "Delegates/UserItemDelegate/UserItemDelegate.h"
#include "Delegates/ChatItemDelegate/ChatItemDelegate.h"

namespace Ui {
class TfmeMainFrame;
}

class TfmeMainFrame : public QWidget
{
    Q_OBJECT

public:
    enum eContactsTabs : quint32 // Вкладки
    {
        ctContacts = 0,         // Контакты
        ctChats = 1,            // Беседы
        ctFindContacts = 2      // Поиск контактов
    };

    explicit TfmeMainFrame(QWidget *inParent = nullptr);
    ~TfmeMainFrame();

private:
    std::unique_ptr<TUserItemDelegate> fUserListDelegate = nullptr;
    std::unique_ptr<TChatItemDelegate> fChatListDelegate = nullptr;

    std::unique_ptr<TUsersProxyModel> fUsersProxyModel = nullptr;
    std::unique_ptr<TChatsProxyModel> fChatsProxyModel = nullptr;

    std::unique_ptr<TUsersModel> fFoundUsers = nullptr;

    std::unique_ptr<QSortFilterProxyModel> fFounUsersProxyModel = nullptr;

    std::map<QUuid, QWidget*> fOpenChatTabs; // Контейнер связывает Uuid беседы и вкладку

    void init(); // Метод инициализирует фрейм
    void Link(); // Метод слинкует сигналы со слотами

public slots:
    void slot_CloseChat(QUuid inChatUuid); // Слот, реагирующий на закрытие беседы (Как правило удалённой)

private slots:
    void slot_CloseTab(qint32 inTabIndex); // Слот, реагирует на сигнал закрытия вкладки
    void slot_UserViewDialogResult(const Users::UserInfo_Ptr inUserInfo, qint32 inResult); // Слот, реагирует на результат диалога просмотра пользователя

    void slot_FindUsers(); // Слот реагирует на запрос поиска пользователей
    void slot_FindUsersRes(const QList<Users::TUserInfo> &inUsers); // Слот, получающий результат поиска пользователей

    void slot_ChatAddNew(); // Слот вызывает добавление новой беседы
    void slot_ChatDeleteCurrent(); // Слот вызывает удаление выбранной беседы
    void slot_ChatOpenCurrent(); // Слот вызывает открытие выбранной беседы

    void on_ContactsFindListView_doubleClicked(const QModelIndex &index);

    void on_ContactsListView_doubleClicked(const QModelIndex &index);

    void on_ContactsFilterLineEdit_textChanged(const QString &arg1);

    void on_ChatListView_doubleClicked(const QModelIndex &index);

    void on_ChatListView_customContextMenuRequested(const QPoint &pos);

    void on_ChatFilterLineEdit_textChanged(const QString &arg1);

private:
    Ui::TfmeMainFrame *ui;
};

#endif // MAINFRAME_H

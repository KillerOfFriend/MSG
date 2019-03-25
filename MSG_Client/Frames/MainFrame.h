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
#include "Delegates/UserItemDelegate/UserItemDelegate.h"
#include "Delegates/ChatItemDelegate/ChatItemDelegate.h"

namespace Ui {
class TfmeMainFrame;
}

class TfmeMainFrame : public QWidget
{
    Q_OBJECT

public:
    explicit TfmeMainFrame(QWidget *inParent = nullptr);
    ~TfmeMainFrame();

private:
    std::unique_ptr<TUserItemDelegate> fUserListDelegate = nullptr;
    std::unique_ptr<TChatItemDelegate> fChatListDelegate = nullptr;
    std::unique_ptr<TUsersProxyModel> fUserProxyModel = nullptr;
    std::unique_ptr<TUsersModel> fFoundUsers = nullptr;
    std::unique_ptr<QSortFilterProxyModel> fFounUsersProxyModel = nullptr;

    std::map<QUuid, qint32> fOpenChatTabs; // Контейнер связывает Uuid беседы и вкладку

    void init(); // Метод инициализирует фрейм
    void Link(); // Метод слинкует сигналы со слотами

private slots:
    void slot_CloseTab(qint32 inTabIndex); // Слот, реагирует на сигнал закрытия вкладки
    void slot_UserViewDialogResult(const Users::UserInfo_Ptr inUserInfo, qint32 inResult); // Слот, реагирует на результат диалога просмотра пользователя

    void slot_FindUsers(); // Слот реагирует на запрос поиска пользователей
    void slot_FindUsersRes(const QList<Users::TUserInfo> &inUsers); // Слот, получающий результат поиска пользователей

    void slot_ChatAddNew(); // Слот вызывает добавление ногвой беседы
    void slot_ChatDeleteCurrent(); // Слот вызывает удаление выбранной беседы
    void slot_ChatOpenCurrent(); // Слот вызывает открытие выбранной беседы

    void on_ContactsFindListView_doubleClicked(const QModelIndex &index);

    void on_ContactsListView_doubleClicked(const QModelIndex &index);

    void on_ContactsFilterLineEdit_textChanged(const QString &arg1);

    void on_ChatListView_doubleClicked(const QModelIndex &index);

    void on_ChatListView_customContextMenuRequested(const QPoint &pos);

private:
    Ui::TfmeMainFrame *ui;
};

#endif // MAINFRAME_H

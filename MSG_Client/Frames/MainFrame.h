#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <memory>

#include <QWidget>
#include <QList>

#include "Models/UsersModel/UsersModel.h"

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
    std::unique_ptr<TUsersModel> fFoundUsers = nullptr;

    void init(); // Метод инициализирует фрейм
    void Link(); // Метод слинкует сигналы со слотами

private slots:
    void slot_UserViewDialogResult(const TUserInfo &inUserInfo, qint32 inResult); // Слот, реагирует на результат диалога просмотра пользователя

    void slot_FindUsers(); // Слот реагирует на запрос поиска пользователей
    void slot_FindUsersRes(const QList<TUserInfo> &inUsers); // Слот, получающий результат поиска пользователей
    void slot_AddContactRes(qint32 inResult, TUserInfo &inContactInfo); // Слот, получающий результат добавления контакта
    void slot_DeleteContactRes(qint32 inResult, QUuid &inContactUuid); // Слот, получающий результат удаления контакта

    void on_ContactsFindListView_doubleClicked(const QModelIndex &index);

    void on_ContactsListView_doubleClicked(const QModelIndex &index);

private:
    Ui::TfmeMainFrame *ui;
};

#endif // MAINFRAME_H

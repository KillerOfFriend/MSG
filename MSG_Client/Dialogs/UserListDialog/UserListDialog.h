#ifndef USERLISTDIALOG_H
#define USERLISTDIALOG_H

#include <memory>

#include <QDialog>
#include <QUuid>
#include <QItemSelection>

#include "Delegates/UserItemDelegate/UserItemDelegate.h"

namespace Ui {
class TUserListDialog;
}

class TUserListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TUserListDialog(QWidget *inParent = nullptr);
    ~TUserListDialog();

    QList<QUuid> selectedUsersUuids(); // Метод вернёт Uuid'ы выбранных пользователей

private:
    Ui::TUserListDialog *ui;
    std::unique_ptr<TUserItemDelegate> fUserListDelegate = nullptr; // Делегат отображения списка пользователей

private slots:
    void slot_UserSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void on_btnConfirm_clicked();

    void on_btnCancel_clicked();



};

#endif // USERLISTDIALOG_H

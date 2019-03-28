#include "UserListDialog.h"
#include "ui_UserListDialog.h"

#include "Classes/DataModule/DataModule.h"
#include "Delegates/UserItemDelegate/UserItemDelegate.h"

//-----------------------------------------------------------------------------
TUserListDialog::TUserListDialog(QWidget *inParent) :
    QDialog(inParent),
    ui(new Ui::TUserListDialog)
{
    ui->setupUi(this);

    ui->UsersListView->setModel(TDM::Instance().UserAccount()->contacts().get());
    ui->UsersListView->setSelectionMode( QAbstractItemView::ExtendedSelection );

    fUserListDelegate.reset(new TUserItemDelegate());
    ui->UsersListView->setItemDelegate(fUserListDelegate.get());

    slot_UserSelectionChanged(QItemSelection(), QItemSelection());
    connect(ui->UsersListView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &TUserListDialog::slot_UserSelectionChanged);
}
//-----------------------------------------------------------------------------
TUserListDialog::~TUserListDialog()
{
    delete ui;
}
//-----------------------------------------------------------------------------
QList<Users::UserInfo_Ptr> TUserListDialog::selectedUsers() // Метод вернёт Uuid'ы выбранных пользователей
{
    QList<Users::UserInfo_Ptr> UsersResult;

    QModelIndexList Selected = ui->UsersListView->selectionModel()->selectedIndexes(); // Получаем список выделенных пользователей

    std::for_each(Selected.begin(), Selected.end(), [&UsersResult](const QModelIndex &Index) // Перебираем
    {
        QModelIndex UuidIndex = Index.sibling(Index.row(), TUsersModel::eColumns::cUserUuid); // Получаем индекс на Uuid
        TDM &DM = TDM::Instance();

        if (UuidIndex.isValid()) // Если cмешённый индекс валидный
        {
            auto FindRes = DM.UserAccount()->contacts()->find(UuidIndex.data().toUuid()); // Ищим по пользователя по uuid
            if (FindRes != DM.UserAccount()->contacts()->end()) // Если юзер найден
                UsersResult.push_back(FindRes->second); // Добавляем его в результат
        }
    });

    return UsersResult;
}
//-----------------------------------------------------------------------------
void TUserListDialog::on_btnConfirm_clicked()
{
    QDialog::accept();
    close();
}
//-----------------------------------------------------------------------------
void TUserListDialog::on_btnCancel_clicked()
{
    QDialog::reject();
    close();
}
//-----------------------------------------------------------------------------
void TUserListDialog::slot_UserSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    ui->btnConfirm->setEnabled( ui->UsersListView->selectionModel()->selectedIndexes().count() >= 2 );
}
//-----------------------------------------------------------------------------

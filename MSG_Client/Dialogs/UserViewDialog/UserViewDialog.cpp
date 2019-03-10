#include "UserViewDialog.h"
#include "ui_UserViewDialog.h"

//-----------------------------------------------------------------------------
TUserViewDialog::TUserViewDialog(TUserInfo &inUserInfo, QWidget *inParent) :
    QDialog(inParent), ui(new Ui::TUserViewDialog), fUserInfo(inUserInfo)
{
    ui->setupUi(this);

    setButtons(eResButtons::rbNoRes);
    setResult(eResButtons::rbNoRes);

    ui->UserLoginLabel->setText("<html><head/><body><p><span style=\" font-weight:600; color:#5500ff;\">" + fUserInfo.userLogin() + "</span></p></body></html>");
    ui->UserNameLabel->setText("<html><head/><body><p><span style=\" font-weight:600; color:#ffaa00;\">" + fUserInfo.userName() + "</span></p></body></html>");
}
//-----------------------------------------------------------------------------
TUserViewDialog::~TUserViewDialog()
{
    delete ui;
}
//-----------------------------------------------------------------------------
void TUserViewDialog::setButtons(eResButtons inButtons)
{
    ui->btnAddToContacts->setEnabled(inButtons & eResButtons::rbAdd);
    ui->btnRemoveFromContacts->setEnabled(inButtons & eResButtons::rbRemove);
    ui->btnSendMessage->setEnabled(inButtons & eResButtons::rbSendMsg);
}
//-----------------------------------------------------------------------------
void TUserViewDialog::on_btnAddToContacts_clicked()
{
    setResult(eResButtons::rbAdd);
    sig_Result(fUserInfo, eResButtons::rbAdd);
    accept();
}
//-----------------------------------------------------------------------------
void TUserViewDialog::on_btnRemoveFromContacts_clicked()
{
    setResult(eResButtons::rbRemove);
    sig_Result(fUserInfo, eResButtons::rbRemove);
    accept();
}
//-----------------------------------------------------------------------------
void TUserViewDialog::on_btnSendMessage_clicked()
{
    setResult(eResButtons::rbSendMsg);
    sig_Result(fUserInfo, eResButtons::rbSendMsg);
    accept();
}
//-----------------------------------------------------------------------------

#include "UserViewDialog.h"
#include "ui_UserViewDialog.h"

//-----------------------------------------------------------------------------
TUserViewDialog::TUserViewDialog(TUserInfo &inUserInfo, QWidget *inParent) :
    QDialog(inParent), ui(new Ui::TUserViewDialog), fUserInfo(inUserInfo)
{
    ui->setupUi(this);

    setButtons(eResButtons::rbNoRes);
    setResult(eResButtons::rbNoRes);

    QSize AvatarSize(ui->UserAvatarView->size().width() - 2, ui->UserAvatarView->size().height() - 2);
    scene.reset(new QGraphicsScene(ui->UserAvatarView));
    ui->UserAvatarView->setScene(scene.get());

    QImage AvatarImage = fUserInfo.userAvatar().scaled(AvatarSize);

    if (AvatarImage.isNull())
    {
        if (fUserInfo.userIsMale())
            AvatarImage = QImage(QImage(":/Resurse/Other/Images/Other/AvaMale.png").scaled(AvatarSize));
        else
            AvatarImage = QImage(":/Resurse/Other/Images/Other/AvaFemale.png").scaled(AvatarSize); // Загружаем дефолтный женский аватар
    }

    item.reset(new QGraphicsPixmapItem(QPixmap::fromImage(AvatarImage)));
    scene->addItem(item.get());

    ui->UserNameLabel->setText("<html><head/><body><p><span style=\" font-weight:600; color:#ffaa00;\">" + fUserInfo.userName() + "</span></p></body></html>");
    ui->UserLoginLabel->setText("<html><head/><body><p><span style=\" font-weight:600; color:#5500ff;\">" + fUserInfo.userLogin() + "</span></p></body></html>");

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

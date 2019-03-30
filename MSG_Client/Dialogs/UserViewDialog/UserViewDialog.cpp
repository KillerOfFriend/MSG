#include "UserViewDialog.h"
#include "ui_UserViewDialog.h"

#include "Classes/DataModule/DataModule.h"

//-----------------------------------------------------------------------------
TUserViewDialog::TUserViewDialog(Core::UserInfo_Ptr &inUserInfo, QWidget *inParent) :
    QDialog(inParent), ui(new Ui::TUserViewDialog)
{
    ui->setupUi(this);

    fUserInfo = inUserInfo;

    initFont();
    initText();

    setButtons(eResButtons::rbNoRes);
    setResult(eResButtons::rbNoRes);
}
//-----------------------------------------------------------------------------
TUserViewDialog::~TUserViewDialog()
{
    delete ui;
}
//-----------------------------------------------------------------------------
void TUserViewDialog::initFont() // Метод инициализирует стиль текста
{
    QString StyleSheet = "QLabel {color : #000000; }";
    QFont Font = QApplication::font();
    Font.setBold(true);
    Font.setPointSize(10);

    ui->UserSexLabel->setFont(Font);
    ui->UserSexLabel->setStyleSheet(StyleSheet);
    ui->UserTypeLabel->setFont(Font);
    ui->UserTypeLabel->setStyleSheet(StyleSheet);
    ui->UserBirthdayLabel->setFont(Font);
    ui->UserBirthdayLabel->setStyleSheet(StyleSheet);
    ui->UserRegistrationDateLabel->setFont(Font);
    ui->UserRegistrationDateLabel->setStyleSheet(StyleSheet);

    QString ValueStyleSheet = "QLabel {color : #808080; }";
    QFont ValueFont = QApplication::font();
    ValueFont.setBold(true);
    ValueFont.setFixedPitch(true);
    ValueFont.setPointSize(10);

    ui->UserSexValueLabel->setFont(ValueFont);
    ui->UserSexValueLabel->setStyleSheet(ValueStyleSheet);
    ui->UserTypeValueLabel->setFont(ValueFont);
    ui->UserTypeValueLabel->setStyleSheet(ValueStyleSheet);
    ui->UserBirthdayValueLabel->setFont(ValueFont);
    ui->UserBirthdayValueLabel->setStyleSheet(ValueStyleSheet);
    ui->UserRegistrationDateValueLabel->setFont(ValueFont);
    ui->UserRegistrationDateValueLabel->setStyleSheet(ValueStyleSheet);
}
//-----------------------------------------------------------------------------
void TUserViewDialog::initText() // Метод инициализирует надписи
{
    ui->UserHeaderWidget->slot_SetUserAvatar(fUserInfo->userAvatar(), fUserInfo->userIsMale());
    ui->UserHeaderWidget->slot_SetUserName(fUserInfo->userName());
    ui->UserHeaderWidget->slot_SetUserLogin(fUserInfo->userLogin());

    ui->UserSexLabel->setText(tr("Пол:"));
    ui->UserTypeLabel->setText(tr("Тип:"));
    ui->UserBirthdayLabel->setText(tr("Дата рождения:"));
    ui->UserRegistrationDateLabel->setText(tr("Дата регистрации:"));

    //--

    if (fUserInfo->userIsMale()) // Талерасты не пройдут
        ui->UserSexValueLabel->setText(tr("Мужской"));
    else ui->UserSexValueLabel->setText(tr("Женский"));

    TDM &DM = TDM::Instance();
    OtherTypes::TUserType ForFind(fUserInfo->userType()); // Инициализируем айтем для поиска
    auto FindRes = DM.Models()->UserTypeModel()->find(ForFind); // Ищим тип пользователя по его коду

    if (FindRes == DM.Models()->UserTypeModel()->end()) // Если тип не найден
        ui->UserTypeValueLabel->setText("UNKNOWN!"); // Сообщаем о косяке
    else ui->UserTypeValueLabel->setText(FindRes->TypeName); // Выводим тип пользователя

    if (fUserInfo->userBirthday().isNull())
        ui->UserBirthdayValueLabel->setText(tr("Не указана"));
    else ui->UserBirthdayValueLabel->setText(fUserInfo->userBirthday().toString("dd.MM.yyyy"));

    ui->UserRegistrationDateValueLabel->setText(fUserInfo->userRegistrationDate().toString("dd.MM.yyyy"));
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

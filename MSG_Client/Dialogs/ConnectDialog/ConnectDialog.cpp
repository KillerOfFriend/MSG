#include "ConnectDialog.h"
#include "ui_ConnectDialog.h"

#include <QtGlobal>
#include <QMessageBox>
#include <QRegExpValidator>

#include "Classes/Settings/Settings.h"
#include "Classes/DataModule/DataModule.h"
#include "resultcodes.h"

//-----------------------------------------------------------------------------
TConnectDialog::TConnectDialog(QWidget *inParent) :
    QDialog(inParent),
    ui(new Ui::TConnectDialog)
{
    ui->setupUi(this);

    Link();
    init();
}
//-----------------------------------------------------------------------------
TConnectDialog::~TConnectDialog()
{
    delete ui;
}
//-----------------------------------------------------------------------------
void TConnectDialog::init() /// Метод инициализирует диалог
{
    TSettings &Settings = TSettings::Instance();

    QRegularExpression EMailRegExp("^([a-z0-9_-]+\\.)*[a-z0-9_-]+@[a-z0-9_-]+(\\.[a-z0-9_-]+)*\\.[a-z]{2,6}$", QRegularExpression::CaseInsensitiveOption);
    QRegularExpression PasswordRegExp("^((?=.*\\d)(?=.*[a-zA-Z])[a-zA-Z0-9!@#$%&*]{6,})$", QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionValidator* EMailValidator = new QRegularExpressionValidator(EMailRegExp, this);
    QRegularExpressionValidator* PasswordValidator = new QRegularExpressionValidator(PasswordRegExp, this);

    ui->LoginLineEdit->setValidator(EMailValidator);
    ui->NewLoginLineEdit->setValidator(EMailValidator);

    ui->PasswordLineEdit->setValidator(PasswordValidator);
    ui->NewPassword1LineEdit->setValidator(PasswordValidator);
    ui->NewPassword2LineEdit->setValidator(PasswordValidator);

    ui->HostLineEdit->setText(Settings.ServerAddress().toString());
    ui->PortSpinBox->setValue(Settings.ServerPort());

    ui->SaveLoginCheckBox->setChecked(Settings.SaveLogin());

    if (Settings.SaveLogin())
        ui->LoginLineEdit->setText(Settings.Login());

    slot_connCheck();
    slot_regCheck();
    slot_confCheck();

    ui->ConnectTabWidget->setCurrentIndex(tbAutorization);
}
//-----------------------------------------------------------------------------
void TConnectDialog::Link() /// Метод соединит сигналы\слоты
{
    connect(ui->LoginLineEdit, &QLineEdit::textChanged, this, &TConnectDialog::slot_connCheck);
    connect(ui->PasswordLineEdit, &QLineEdit::textChanged, this, &TConnectDialog::slot_connCheck);

    connect(ui->NewLoginLineEdit, &QLineEdit::textChanged, this, &TConnectDialog::slot_regCheck);
    connect(ui->NewPassword1LineEdit, &QLineEdit::textChanged, this, &TConnectDialog::slot_regCheck);
    connect(ui->NewPassword2LineEdit, &QLineEdit::textChanged, this, &TConnectDialog::slot_regCheck);

    connect(ui->HostLineEdit, &QLineEdit::textChanged, this, &TConnectDialog::slot_confCheck);
    connect(ui->PortSpinBox, qOverload<const QString&>(&QSpinBox::valueChanged), this, &TConnectDialog::slot_confCheck);

    slot_LinkEnterPress(true);
}
//-----------------------------------------------------------------------------
void TConnectDialog::slot_SetUiEnabled(bool inEnabled)
{
    if (!inEnabled)
    {
        ui->btnConnect->setEnabled(false);
        ui->btnRegistrationUser->setEnabled(false);
        ui->btnSaveSettings->setEnabled(false);
    }
    else
    {
        slot_connCheck();
        slot_regCheck();
        slot_confCheck();
    }

    ui->LoginLineEdit->setEnabled(inEnabled);
    ui->PasswordLineEdit->setEnabled(inEnabled);
    ui->NewLoginLineEdit->setEnabled(inEnabled);
    ui->NewNameLineEdit->setEnabled(inEnabled);
    ui->NewPassword1LineEdit->setEnabled(inEnabled);
    ui->NewPassword2LineEdit->setEnabled(inEnabled);
    ui->HostLineEdit->setEnabled(inEnabled);
    ui->PortSpinBox->setEnabled(inEnabled);

    slot_LinkEnterPress(inEnabled);
}
//-----------------------------------------------------------------------------
void TConnectDialog::slot_LinkEnterPress(bool inEnabled)
{
    if (inEnabled)
    {
        connect(ui->LoginLineEdit, &QLineEdit::returnPressed, ui->btnConnect, &QPushButton::click);
        connect(ui->PasswordLineEdit, &QLineEdit::returnPressed, ui->btnConnect, &QPushButton::click);

        connect(ui->NewLoginLineEdit, &QLineEdit::returnPressed, ui->btnRegistrationUser, &QPushButton::click);
        connect(ui->NewPassword1LineEdit, &QLineEdit::returnPressed, ui->btnRegistrationUser, &QPushButton::click);
        connect(ui->NewPassword2LineEdit, &QLineEdit::returnPressed, ui->btnRegistrationUser, &QPushButton::click);
    }
    else
    {
        disconnect(ui->LoginLineEdit, &QLineEdit::returnPressed, ui->btnConnect, &QPushButton::click);
        disconnect(ui->PasswordLineEdit, &QLineEdit::returnPressed, ui->btnConnect, &QPushButton::click);

        disconnect(ui->NewLoginLineEdit, &QLineEdit::returnPressed, ui->btnRegistrationUser, &QPushButton::click);
        disconnect(ui->NewPassword1LineEdit, &QLineEdit::returnPressed, ui->btnRegistrationUser, &QPushButton::click);
        disconnect(ui->NewPassword2LineEdit, &QLineEdit::returnPressed, ui->btnRegistrationUser, &QPushButton::click);
    }
}
//-----------------------------------------------------------------------------
void TConnectDialog::slot_RegistrationResult(qint32 inRes)
{
    disconnect(TDM::Instance().Client().get(), &TMSGClient::sig_UserCreateResult, this, &TConnectDialog::slot_RegistrationResult); // Разрываем связь

    TDM::Instance().Client()->disconnect(); // Закрываем соединение

    switch (inRes)
    {
        case Res::rTimeOut: { QMessageBox::critical(this, tr("Ошибка"), tr("Время ожидания вышло!")); break; };
        case Res::CreateClient::ccAlredyExist:
        {
            QMessageBox::warning(this, tr("Предупреждение"), tr("Пользователь c таким логином уже существует!"));

            ui->NewPassword1LineEdit->clear();
            ui->NewNameLineEdit->clear();
            ui->NewPassword2LineEdit->clear();
            ui->NewLoginLineEdit->setFocus();

            break;
        };
        case Res::CreateClient::ccCreated:
        {
            QMessageBox::information(this, tr("Сообщение"), tr("Пользователь успешно создан."));

            ui->NewLoginLineEdit->clear();
            ui->NewPassword1LineEdit->clear();
            ui->NewPassword2LineEdit->clear();

            ui->LoginLineEdit->clear();
            ui->PasswordLineEdit->clear();
            ui->ConnectTabWidget->setCurrentIndex(tbAutorization);
            ui->LoginLineEdit->setFocus();

            break;
        };
        default: { QMessageBox::critical(this, tr("Ошибка"), tr("Неизвестная ошибка!")); break; };
    }

    slot_SetUiEnabled(true);
}
//-----------------------------------------------------------------------------
void TConnectDialog::slot_AuthorizationResult(qint32 Result)
{
    disconnect(TDM::Instance().Client().get(), &TMSGClient::sig_AuthorizationResult, this, &TConnectDialog::slot_AuthorizationResult);

    if (Result != Res::CanAut::caAuthorizationTrue) // Если не удалось авторизоваться
        TDM::Instance().Client()->disconnect(); // Закрываем соединение

    switch (Result)
    {
        case Res::rTimeOut: { QMessageBox::critical(this, tr("Ошибка"), tr("Время ожидания вышло!")); break; };
        case Res::CanAut::caAuthorizationFalse: { QMessageBox::warning(this, tr("Предупреждение"), tr("Пользователь не найден")); break; }
        case Res::CanAut::caUserInfoError: { QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось прочитать данные пользователя")); break; }
        case Res::CanAut::caIncorrectPass: { QMessageBox::warning(this, tr("Предупреждение"), tr("Не верный пароль")); break; }
        case Res::CanAut::caAuthorizationTrue:
        {
            qDebug() << "Авторизация прошла успешно";

            TSettings &Settings = TSettings::Instance();
            Settings.setSaveLogin(ui->SaveLoginCheckBox->isChecked());

            if (!ui->SaveLoginCheckBox->isChecked())
                Settings.setLogin("");
            else Settings.setLogin(ui->LoginLineEdit->text());

            close();
            break;
        }

        default: { QMessageBox::critical(this, tr("Ошибка"), tr("Неизвестная ошибка!")); break; };
    }
    slot_SetUiEnabled(true);
}
//-----------------------------------------------------------------------------
void TConnectDialog::slot_regCheck() /// Метод проверит возможность регистрации
{
    bool isFull = !ui->NewLoginLineEdit->text().isEmpty()
            && !ui->NewNameLineEdit->text().isEmpty()
            && !ui->NewPassword1LineEdit->text().isEmpty()
            && !ui->NewPassword2LineEdit->text().isEmpty();

    ui->btnRegistrationUser->setEnabled(isFull);
}
//-----------------------------------------------------------------------------
void TConnectDialog::slot_connCheck() /// Метод проверит возможность соединения
{
    bool isFull = !ui->LoginLineEdit->text().isEmpty()
            && !ui->PasswordLineEdit->text().isEmpty();

    ui->btnConnect->setEnabled(isFull);
}
//-----------------------------------------------------------------------------
void TConnectDialog::slot_confCheck() /// Метод проверит возможность сохранения настроек
{
    bool isFull = !ui->HostLineEdit->text().isEmpty()
        && ui->PortSpinBox->value();

    ui->btnSaveSettings->setEnabled(isFull);
}
//-----------------------------------------------------------------------------
void TConnectDialog::on_SaveLoginCheckBox_clicked(bool checked)
{
    TSettings::Instance().setSaveLogin(checked);
}
//-----------------------------------------------------------------------------
void TConnectDialog::on_btnConnect_clicked()
{
    TDM &DM = TDM::Instance();


    if(!DM.Client())
        QMessageBox::warning(this, tr("Предупреждение"), tr("Нет возможности соедениения!"));
    else
    if (!DM.Client()->connectToServer()) // Если нет соединения
        QMessageBox::warning(this, tr("Предупреждение"), tr("Не удалось соединиться с сервером! Проверьте настройки и подключение к сети."));
    else // Соединение установлено
    {
        connect(TDM::Instance().Client().get(), &TMSGClient::sig_AuthorizationResult, this, &TConnectDialog::slot_AuthorizationResult);
        connect(TDM::Instance().Client().get(), &TMSGClient::sig_TimeOut, this, &TConnectDialog::slot_AuthorizationResult);

        slot_SetUiEnabled(false);

        if (!DM.Client()->authorization(ui->LoginLineEdit->text(), ui->PasswordLineEdit->text()))
        {
            disconnect(TDM::Instance().Client().get(), &TMSGClient::sig_AuthorizationResult, this, &TConnectDialog::slot_AuthorizationResult);
            disconnect(TDM::Instance().Client().get(), &TMSGClient::sig_TimeOut, this, &TConnectDialog::slot_AuthorizationResult);

            QMessageBox::warning(this, tr("Предупреждение"), tr("Не удалось авторизироваться!"));
            slot_SetUiEnabled(true);
        }
    }
}
//-----------------------------------------------------------------------------
void TConnectDialog::on_btnClose_clicked()
{ close(); }
//-----------------------------------------------------------------------------
void TConnectDialog::on_btnRegistrationUser_clicked()
{
    if (ui->NewPassword1LineEdit->text() != ui->NewPassword2LineEdit->text())
    {
        QMessageBox::warning(this, tr("Предупреждение"), tr("Пароли не совпадают!"));
        return;
    }

    TDM &DM = TDM::Instance();

    if(!DM.Client())
        QMessageBox::warning(this, tr("Предупреждение"), tr("Нет возможности соедениения!"));
    else
    if (!DM.Client()->connectToServer()) // Если нет соединения
        QMessageBox::warning(this, tr("Предупреждение"), tr("Не удалось соединиться с сервером! Проверьте настройки и подключение к сети."));
    else // Соединение установлено
    {
        connect(TDM::Instance().Client().get(), &TMSGClient::sig_UserCreateResult, this, &TConnectDialog::slot_RegistrationResult);
        slot_SetUiEnabled(false);

        if (!DM.Client()->createUser(ui->NewLoginLineEdit->text(), ui->NewPassword1LineEdit->text(), ui->NewNameLineEdit->text(), ui->MaleRadioButton->isChecked()))
        {
            disconnect(TDM::Instance().Client().get(), &TMSGClient::sig_UserCreateResult, this, &TConnectDialog::slot_RegistrationResult);

            QMessageBox::warning(this, tr("Предупреждение"), tr("Не удалось создать пользователя!"));
            slot_SetUiEnabled(true);
        }
    }
}
//-----------------------------------------------------------------------------
void TConnectDialog::on_btnSaveSettings_clicked()
{
    TSettings &Settings = TSettings::Instance();

    Settings.setServerAddress(QHostAddress(ui->HostLineEdit->text()));
    Settings.setServerPort(ui->PortSpinBox->value());
}
//-----------------------------------------------------------------------------

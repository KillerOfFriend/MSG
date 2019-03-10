#ifndef CONNECTDIALOG_H
#define CONNECTDIALOG_H

#include <QDialog>

#include <Classes/UserInfo/UserInfo.h>

namespace Ui {
class TConnectDialog;
}

enum eTabs
{
    tbAutorization = 0,
    tbRegistration = 1,
    tbSettings = 2
};

class TConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TConnectDialog(QWidget *inParent = nullptr);
    ~TConnectDialog();

private:
    Ui::TConnectDialog *ui;

    void init(); // Метод инициализирует диалог
    void Link(); // Метод соединит сигналы\слоты

private slots:
    void slot_SetUiEnabled(bool inEnabled); // Слот установит активность интерфейса
    void slot_LinkEnterPress(bool inEnabled); // Слот соеденит сигналы реакции на нажатие Enter

    void slot_connCheck(); // Слот проверит возможность соединения
    void slot_regCheck(); // Слот проверит возможность регистрации
    void slot_confCheck(); // Слот проверит возможность сохранения настроек

    void slot_RegistrationResult(qint32 inRes); // Слот, реагирующий на возврат результата регистрации пользователя
    void slot_AuthorizationResult(qint32 Result); // Слот, реагирующий на возврат результата авторизации

    void on_SaveLoginCheckBox_clicked(bool checked);
    void on_btnRegistrationUser_clicked();
    void on_btnClose_clicked();
    void on_btnSaveSettings_clicked();
    void on_btnConnect_clicked();
};

#endif // CONNECTDIALOG_H

#ifndef USERVIEWDIALOG_H
#define USERVIEWDIALOG_H

#include <QDialog>
#include <QFont>

#include "Classes/UserInfo/UserInfo.h"

namespace Ui {
class TUserViewDialog;
}

class TUserViewDialog : public QDialog
{
    Q_OBJECT
public:
    enum eResButtons
    {
        rbNoRes =    0x0,
        rbAdd =      0x1,
        rbRemove =   0x2,
        rbSendMsg =  0x4
    };

    explicit TUserViewDialog(Users::UserInfo_Ptr &inUserInfo, QWidget *inParent = nullptr);
    ~TUserViewDialog();

    void setButtons(eResButtons inButtons);

private slots:
    void on_btnAddToContacts_clicked();
    void on_btnRemoveFromContacts_clicked();
    void on_btnSendMessage_clicked();

private:
    Ui::TUserViewDialog *ui; 

    Users::UserInfo_Ptr fUserInfo ; // Указател на объект информации о пользователе

    void initFont(); // Метод инициализирует стиль текста
    void initText(); // Метод инициализирует надписи

signals:
    void sig_Result(const Users::UserInfo_Ptr inUserInfo, qint32 inResult);

};

#endif // USERVIEWDIALOG_H

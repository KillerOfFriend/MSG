#ifndef USERVIEWDIALOG_H
#define USERVIEWDIALOG_H

#include <QDialog>

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

    explicit TUserViewDialog(TUserInfo &inUserInfo, QWidget *inParent = nullptr);
    ~TUserViewDialog();

    void setButtons(eResButtons inButtons);

private slots:
    void on_btnAddToContacts_clicked();
    void on_btnRemoveFromContacts_clicked();
    void on_btnSendMessage_clicked();

private:
    Ui::TUserViewDialog *ui; 
    TUserInfo &fUserInfo ; // Ссылка на объект информации о пользователе

signals:
    void sig_Result(const TUserInfo &inUserInfo, qint32 inResult);

};

#endif // USERVIEWDIALOG_H

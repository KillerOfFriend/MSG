#ifndef USERVIEWDIALOG_H
#define USERVIEWDIALOG_H

#include <memory>

#include <QDialog>
#include <QPixmap>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

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

    explicit TUserViewDialog(Users::TUserInfo &inUserInfo, QWidget *inParent = nullptr);
    ~TUserViewDialog();

    void setButtons(eResButtons inButtons);

private slots:
    void on_btnAddToContacts_clicked();
    void on_btnRemoveFromContacts_clicked();
    void on_btnSendMessage_clicked();

private:
    Ui::TUserViewDialog *ui; 
    Users::TUserInfo &fUserInfo ; // Ссылка на объект информации о пользователе

    std::unique_ptr<QGraphicsScene> scene = nullptr;
    std::unique_ptr<QGraphicsPixmapItem> item = nullptr;

signals:
    void sig_Result(const Users::TUserInfo &inUserInfo, qint32 inResult);

};

#endif // USERVIEWDIALOG_H

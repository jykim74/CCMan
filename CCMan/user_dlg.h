#ifndef REG_USER_DLG_H
#define REG_USER_DLG_H

#include <QDialog>
#include "ui_user_dlg.h"

namespace Ui {
class UserDlg;
}

class UserDlg : public QDialog, public Ui::UserDlg
{
    Q_OBJECT

public:
    explicit UserDlg(QWidget *parent = nullptr);
    ~UserDlg();

    void setEditMode( int nSeq );

private slots:
    void clickRegister();
    void clickModify();
    void clickDelete();

private:
    void initialize();
    int seq_;
};

#endif // REG_USER_DLG_H

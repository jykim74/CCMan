#ifndef REG_USER_DLG_H
#define REG_USER_DLG_H

#include <QDialog>
#include "ui_reg_user_dlg.h"

namespace Ui {
class RegUserDlg;
}

class RegUserDlg : public QDialog, public Ui::RegUserDlg
{
    Q_OBJECT

public:
    explicit RegUserDlg(QWidget *parent = nullptr);
    ~RegUserDlg();

    virtual void accept();

private:

};

#endif // REG_USER_DLG_H

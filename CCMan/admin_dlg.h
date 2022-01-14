#ifndef ADMIN_DLG_H
#define ADMIN_DLG_H

#include <QDialog>
#include "ui_admin_dlg.h"

namespace Ui {
class AdminDlg;
}

class AdminDlg : public QDialog, public Ui::AdminDlg
{
    Q_OBJECT

public:
    explicit AdminDlg(QWidget *parent = nullptr);
    ~AdminDlg();

    void setEditMode( int nSeq );
    void showEvent( QShowEvent *event );

private slots:
    void clickRegister();
    void clickModify();
    void clickDelete();

private:
    void initialize();
    int seq_;
};

#endif // ADMIN_DLG_H

#ifndef ISSUE_CRL_DLG_H
#define ISSUE_CRL_DLG_H

#include <QDialog>
#include "ui_issue_crl_dlg.h"

namespace Ui {
class IssueCRLDlg;
}

class IssueCRLDlg : public QDialog, public Ui::IssueCRLDlg
{
    Q_OBJECT

public:
    explicit IssueCRLDlg(QWidget *parent = nullptr);
    ~IssueCRLDlg();

private slots:
    virtual void accept();
    void clickDownload();

private:
    void initialize();
};

#endif // ISSUE_CRL_DLG_H

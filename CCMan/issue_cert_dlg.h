#ifndef ISSUE_CERT_DLG_H
#define ISSUE_CERT_DLG_H

#include <QDialog>
#include "ui_issue_cert_dlg.h"

namespace Ui {
class IssueCertDlg;
}

class IssueCertDlg : public QDialog, public Ui::IssueCertDlg
{
    Q_OBJECT

public:
    explicit IssueCertDlg(QWidget *parent = nullptr);
    ~IssueCertDlg();

private slots:
    virtual void accept();
    void algChanged(int index);
    void findCSR();
    void clickRegUser();
    void clickUseCSR();

private:
    void initialize();
    void setDefault();
};

#endif // ISSUE_CERT_DLG_H

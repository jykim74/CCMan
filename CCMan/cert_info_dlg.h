#ifndef CERT_INFO_DLG_H
#define CERT_INFO_DLG_H

#include <QDialog>
#include "js_db.h"
#include "js_cc.h"
#include "ui_cert_info_dlg.h"
#include "js_bin.h"

namespace Ui {
class CertInfoDlg;
}

class CertInfoDlg : public QDialog, public Ui::CertInfoDlg
{
    Q_OBJECT

public:
    explicit CertInfoDlg(QWidget *parent = nullptr);
    ~CertInfoDlg();

    int getCertNum() { return cert_num_; };
    void setCertNum( int cert_num );
    void setCertBin( const BIN *pCert );

private slots:
    void showEvent(QShowEvent *event);
    void clickField( QModelIndex index );
    void clickCheck();

private:
    int cert_num_;
    BIN cert_bin_;
    JCC_CertList* cert_list_;

    void initialize();
    void pathInit();
    void initUI();
    void clearTable();
};

#endif // CERT_INFO_DLG_H

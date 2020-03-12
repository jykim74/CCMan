#include "revoke_cert_dlg.h"
#include "mainwindow.h"
#include "man_applet.h"
#include "cc_client.h"
#include "js_cc.h"
#include "js_db.h"
#include "js_db_data.h"
#include "common.h"


RevokeCertDlg::RevokeCertDlg(QWidget *parent) :
    QDialog(parent)
{
    cert_num_ = -1;
    setupUi(this);

    initUI();
//    initialize();
}

RevokeCertDlg::~RevokeCertDlg()
{

}

void RevokeCertDlg::setCertNum(int cert_num)
{
    cert_num_ = cert_num;
    initialize();
}


void RevokeCertDlg::initialize()
{
    int ret = -1;
    if( cert_num_ < 0 ) return;

    JCC_Cert sCert;
    memset( &sCert, 0x00, sizeof(sCert));
    manApplet->ccClient()->getCert( cert_num_, &sCert );

    mSubjectDNText->setText( sCert.pSubjectDN );
    mSerialText->setText( QString("%1").arg( sCert.nNum ));

    QDateTime dateTime = QDateTime::currentDateTime();
    mRevokeDateTime->setDateTime( dateTime );
    JS_DB_resetCert( &sCert );
}

void RevokeCertDlg::accept()
{
    int ret = -1;

    JCC_Cert sCert;
    JCC_Revoked sRevoked;

    memset( &sCert, 0x00, sizeof(sCert));
    memset( &sRevoked, 0x00, sizeof(sRevoked));

    manApplet->ccClient()->getCert( cert_num_, &sCert );

    int nReason = mReasonCombo->currentIndex();

    JS_DB_setRevoked( &sRevoked, -1, cert_num_, sCert.nIssuerNum, sCert.pSerial, mRevokeDateTime->dateTime().toTime_t(), nReason );
    manApplet->ccClient()->addRevoked( &sRevoked );

    JS_DB_resetCert( &sCert );
    JS_DB_resetRevoked( &sRevoked );
    QDialog::accept();
}

void RevokeCertDlg::initUI()
{
    mReasonCombo->addItems( kRevokeReasonList );
}

#include <QFileDialog>

#include "issue_cert_dlg.h"
#include "common.h"
#include "mainwindow.h"
#include "man_applet.h"
#include "cc_client.h"
#include "js_cc.h"
#include "js_db.h"
#include "js_db_data.h"
#include "js_pki.h"
#include "js_pki_x509.h"
#include "js_pki_tools.h"

IssueCertDlg::IssueCertDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    initialize();
    setDefault();
}

IssueCertDlg::~IssueCertDlg()
{

}

void IssueCertDlg::accept()
{
    int ret = 0;
    JCC_IssueCertReq sIssueCertReq;
    JCC_IssueCertRsp sIssueCertRsp;

    int nUserNum = -1;
    int nCertPolicyNum = -1;
    QString strName;
    QString strSSN;
    QString strEmail;
    QString strCSR;

    memset( &sIssueCertReq, 0x00, sizeof(sIssueCertReq));
    memset( &sIssueCertRsp, 0x00, sizeof(sIssueCertRsp));

    if( mRegUserCheck->isChecked() )
    {
        strName = mNameText->text();
        strSSN = mSSNText->text();
        strEmail = mEmailText->text();
    }
    else
    {
        nUserNum = mUserNumText->text().toInt();
    }

    QVariant data = mCertPolicyCombo->currentData();
    nCertPolicyNum = data.toInt();

    BIN binCSR = {0,0};
    char *pHex = NULL;

    if( mUseCSRCheck->isChecked() )
    {
        QString strCSRPath = mCSRPathText->text();

        JS_BIN_fileRead( strCSRPath.toStdString().c_str(), &binCSR );
    }
    else
    {
        int nAlg = 0;
        BIN binPub = {0,0};
        BIN binPub2 = {0,0};
        BIN binPri = {0,0};

        QString strSubjectDn = mSubjectDNText->text();

        if( mAlgCombo->currentText() == "RSA" )
        {
            nAlg = JS_PKI_KEY_TYPE_RSA;
            int nKeySize = mOptionCombo->currentText().toInt();
            ret = JS_PKI_RSAGenKeyPair( nKeySize, 65537, &binPub, &binPub2, &binPri );
        }
        else if( mAlgCombo->currentText() == "EC" )
        {
            nAlg = JS_PKI_KEY_TYPE_ECC;
            int nGroupID = JS_PKI_getNidFromSN( mOptionCombo->currentText().toStdString().c_str() );
            ret = JS_PKI_ECCGenKeyPair( nGroupID, &binPub, &binPub2, &binPri );
        }

        ret = JS_PKI_makeCSR( nAlg, "SHA256", strSubjectDn.toStdString().c_str(), &binPri, NULL, &binCSR );

        JS_BIN_reset( &binPub );
        JS_BIN_reset( &binPub2 );
        JS_BIN_reset( &binPri );
    }

    JS_BIN_encodeHex( &binCSR, &pHex );
    strCSR = pHex;

    JS_CC_setIssueCertReq( &sIssueCertReq,
                           nUserNum,
                           nCertPolicyNum,
                           strName.toStdString().c_str(),
                           strSSN.toStdString().c_str(),
                           strEmail.toStdString().c_str(),
                           strCSR.toStdString().c_str() );

    ret = manApplet->ccClient()->issueCert( &sIssueCertReq, &sIssueCertRsp );

    JS_BIN_reset( &binCSR );
    if( pHex ) JS_free( pHex );
    JS_CC_resetIssueCertReq( &sIssueCertReq );
    JS_CC_resetIssueCertRsp( &sIssueCertRsp );

    if( ret == 0 )
        QDialog::accept();
}

void IssueCertDlg::algChanged( int index )
{
    mOptionCombo->clear();

    if( index == 0 )
    {
        mOptionCombo->addItems(kRSAOptionList);
    }
    else if( index == 1 )
    {
        mOptionCombo->addItems(kECCOptionList);
    }
}

void IssueCertDlg::initialize()
{
    mAlgCombo->addItems( kMechList );
    mOptionCombo->addItems( kRSAOptionList );

    connect( mAlgCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(algChanged(int)));
    connect( mCSRFindBtn, SIGNAL(clicked()), this, SLOT(findCSR()));
    connect( mRegUserCheck, SIGNAL(clicked()), this, SLOT(clickRegUser()));
    connect( mUseCSRCheck, SIGNAL(clicked()), this, SLOT(clickUseCSR()));

    JCC_CertPolicyList *pCertPolicyList = NULL;
    JCC_CertPolicyList *pCurList = NULL;

    manApplet->ccClient()->getCertPolicyList( &pCertPolicyList );

    pCurList = pCertPolicyList;

    while( pCurList )
    {
        int nPolicyNum = pCurList->sCertPolicy.nNum;
        QVariant data = nPolicyNum;
        mCertPolicyCombo->addItem( pCurList->sCertPolicy.pName, data );

        pCurList = pCurList->pNext;
    }

    if( pCertPolicyList ) JS_DB_resetCertPolicyList( &pCertPolicyList );
}

void IssueCertDlg::setDefault()
{
    mRegUserCheck->setChecked(false);
    mUseCSRCheck->setChecked( false );

    clickRegUser();
    clickUseCSR();
}

void IssueCertDlg::findCSR()
{
    QFileDialog::Options options;
    options |= QFileDialog::DontUseNativeDialog;

    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName( this,
                                                     tr("CSR files"),
                                                     QDir::currentPath(),
                                                     tr("CSR Files (*.csr);;DER Files (*.der);;All Files (*.*)"),
                                                     &selectedFilter,
                                                     options );

    mCSRPathText->setText( fileName );
}

void IssueCertDlg::clickRegUser()
{
    bool bStatus = mRegUserCheck->isChecked();

    mUserNumText->setEnabled(!bStatus);

    mNameText->setEnabled(bStatus);
    mSSNText->setEnabled(bStatus);
    mEmailText->setEnabled(bStatus);
}

void IssueCertDlg::clickUseCSR()
{
    bool bStatus = mUseCSRCheck->isChecked();

    mCSRFindBtn->setEnabled( bStatus );
    mCSRPathText->setEnabled( bStatus );

    mSubjectDNText->setEnabled( !bStatus );
    mAlgCombo->setEnabled( !bStatus );
    mOptionCombo->setEnabled( !bStatus );
}
#include "mainwindow.h"
#include "man_applet.h"
#include "cert_info_dlg.h"
#include "js_pki.h"
#include "js_pki_x509.h"
#include "js_pki_ext.h"
#include "js_util.h"
#include "cc_client.h"


CertInfoDlg::CertInfoDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    initUI();
    cert_num_ = -1;
}

CertInfoDlg::~CertInfoDlg()
{

}

void CertInfoDlg::setCertNum(int cert_num)
{
    cert_num_ = cert_num;
}

void CertInfoDlg::showEvent(QShowEvent *event)
{
    initialize();
}

void CertInfoDlg::initialize()
{
    int ret = 0;
    int i = 0;

    BIN binCert = {0,0};
    JCertInfo  sCertInfo;
    JDB_Cert    sCert;
    JExtensionInfoList *pExtInfoList = NULL;
    char    sNotBefore[64];
    char    sNotAfter[64];

    memset( &sCert, 0x00, sizeof(sCert));

    if( cert_num_ < 0 )
    {
        manApplet->warningBox( tr( "Select certificate"), this );
        this->hide();
        return;
    }

    clearTable();

    manApplet->ccClient()->getCert( cert_num_, &sCert );

    memset( &sCertInfo, 0x00, sizeof(sCertInfo));
    JS_BIN_decodeHex( sCert.pCert, &binCert );

    ret = JS_PKI_getCertInfo( &binCert, &sCertInfo, &pExtInfoList );
    if( ret != 0 )
    {
        manApplet->warningBox( tr("fail to get certificate information"), this );
        JS_BIN_reset( &binCert );
        this->hide();
        return;
    }

    mFieldTable->insertRow(i);
    mFieldTable->setItem( i, 0, new QTableWidgetItem( QString("Version")));
    mFieldTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(sCertInfo.nVersion)));
    i++;

    if( sCertInfo.pSerial )
    {
        mFieldTable->insertRow(i);
        mFieldTable->setItem(i, 0, new QTableWidgetItem(QString("Serial")));
        mFieldTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(sCertInfo.pSerial)));
        i++;
    }

    JS_UTIL_getDateTime( sCertInfo.uNotBefore, sNotBefore );
    mFieldTable->insertRow(i);
    mFieldTable->setItem( i, 0, new QTableWidgetItem( QString("NotBefore")));
    mFieldTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(sNotBefore)));
    i++;

    JS_UTIL_getDateTime( sCertInfo.uNotAfter, sNotAfter );
    mFieldTable->insertRow(i);
    mFieldTable->setItem( i, 0, new QTableWidgetItem( QString("NotAfter")));
    mFieldTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(sNotAfter)));
    i++;

    if( sCertInfo.pSubjectName )
    {
        QString name = QString::fromUtf8( sCertInfo.pSubjectName );

        mFieldTable->insertRow(i);
        mFieldTable->setItem(i, 0, new QTableWidgetItem(QString("SubjectName")));
        mFieldTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg( name )));
        i++;
    }

    if( sCertInfo.pPublicKey )
    {
        mFieldTable->insertRow(i);
        mFieldTable->setItem(i, 0, new QTableWidgetItem(QString("PublicKey")));
        mFieldTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(sCertInfo.pPublicKey)));
        i++;
    }

    if( sCertInfo.pIssuerName )
    {
        mFieldTable->insertRow(i);
        mFieldTable->setItem(i, 0, new QTableWidgetItem(QString("IssuerName")));
        mFieldTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(sCertInfo.pIssuerName)));
        i++;
    }

    if( sCertInfo.pSignAlgorithm )
    {
        mFieldTable->insertRow(i);
        mFieldTable->setItem(i, 0, new QTableWidgetItem(QString("SigAlgorithm")));
        mFieldTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(sCertInfo.pSignAlgorithm)));
        i++;
    }

    if( sCertInfo.pSignature )
    {
        mFieldTable->insertRow(i);
        mFieldTable->setItem(i, 0, new QTableWidgetItem(QString("Signature")));
        mFieldTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(sCertInfo.pSignature)));
        i++;
    }

    if( pExtInfoList )
    {
        JExtensionInfoList *pCurList = pExtInfoList;

        while( pCurList )
        {
            JDB_PolicyExt sPolicyRec;
            memset( &sPolicyRec, 0x00, sizeof(sPolicyRec));
            JS_PKI_transExtensionToDBRec( &pCurList->sExtensionInfo, &sPolicyRec );

            mFieldTable->insertRow(i);
            mFieldTable->setItem(i,0, new QTableWidgetItem(QString("%1").arg( sPolicyRec.pSN )));
            mFieldTable->setItem(i,1, new QTableWidgetItem(QString("[%1]%2")
                                                               .arg( sPolicyRec.bCritical )
                                                               .arg( sPolicyRec.pValue )));


            pCurList = pCurList->pNext;

            JS_DB_resetPolicyExt( &sPolicyRec );
            i++;
        }
    }

    JS_BIN_reset( &binCert );
    JS_PKI_resetCertInfo( &sCertInfo );
    if( pExtInfoList ) JS_PKI_resetExtensionInfoList( &pExtInfoList );
    JS_DB_resetCert( &sCert );
}

void CertInfoDlg::initUI()
{
    QStringList sBaseLabels = { tr("Field"), tr("Value") };

    mFieldTable->clear();
    mFieldTable->horizontalHeader()->setStretchLastSection(true);
    mFieldTable->setColumnCount(2);
    mFieldTable->setHorizontalHeaderLabels( sBaseLabels );
    mFieldTable->verticalHeader()->setVisible(false);

    connect( mCloseBtn, SIGNAL(clicked()), this, SLOT(clickClose()));
    connect( mFieldTable, SIGNAL(clicked(QModelIndex)), this, SLOT(clickField(QModelIndex)));
}

void CertInfoDlg::clickClose()
{
    this->hide();
}

void CertInfoDlg::clickField(QModelIndex index)
{
    int row = index.row();
    int col = index.column();

    QTableWidgetItem* item = mFieldTable->item( row, 1 );
    if( item == NULL ) return;

    mDetailText->setText( item->text() );
}

void CertInfoDlg::clearTable()
{
    int rowCnt = mFieldTable->rowCount();

    for( int i=0; i < rowCnt; i++ )
        mFieldTable->removeRow(0);
}
#include "mainwindow.h"
#include "man_applet.h"

#include "crl_info_dlg.h"
#include "js_pki.h"
#include "js_pki_x509.h"
#include "js_pki_ext.h"
#include "js_util.h"
#include "cc_client.h"


CRLInfoDlg::CRLInfoDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    initUI();
    crl_num_ = -1;
    ext_info_list_ = NULL;
    revoke_info_list_ = NULL;

    memset( &crl_info_, 0x00, sizeof(crl_info_));
}

CRLInfoDlg::~CRLInfoDlg()
{

}

void CRLInfoDlg::showEvent(QShowEvent *event)
{
    initialize();
}

void CRLInfoDlg::setCRLNum(int crl_num)
{
    crl_num_ = crl_num;
}

void CRLInfoDlg::clickClose()
{
    this->hide();
}

void CRLInfoDlg::initialize()
{
    int ret = 0;
    int i = 0;

    BIN binCRL = {0,0};
    char    sLastUpdate[64];
    char    sNextUpdate[64];
    JDB_CRL sCRL;

    memset( &sCRL, 0x00, sizeof(sCRL));

    JS_PKI_resetCRLInfo( &crl_info_ );

    if( crl_num_ < 0 )
    {
        manApplet->warningBox( tr("Select CRL"), this );
        return;
    }

    clearTable();
    if( ext_info_list_ ) JS_PKI_resetExtensionInfoList( &ext_info_list_ );
    if( revoke_info_list_ ) JS_PKI_resetRevokeInfoList( &revoke_info_list_ );

    manApplet->ccClient()->getCRL( crl_num_, &sCRL );

    JS_BIN_decodeHex( sCRL.pCRL, &binCRL );

    ret = JS_PKI_getCRLInfo( &binCRL, &crl_info_, &ext_info_list_, &revoke_info_list_ );
    if( ret != 0 )
    {
        manApplet->warningBox( tr("fail to get CRL information"), this );
        JS_BIN_reset( &binCRL );
        this->hide();
        return;
    }

    mCRLListTable->insertRow(i);
    mCRLListTable->setItem( i, 0, new QTableWidgetItem( QString("Version")));
    mCRLListTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(crl_info_.nVersion)));
    i++;

    if( crl_info_.pIssuerName )
    {
        mCRLListTable->insertRow(i);
        mCRLListTable->setItem( i, 0, new QTableWidgetItem( QString("IssuerName")));
        mCRLListTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(crl_info_.pIssuerName)));
        i++;
    }


    JS_UTIL_getDateTime( crl_info_.uLastUpdate, sLastUpdate );
    mCRLListTable->insertRow(i);
    mCRLListTable->setItem( i, 0, new QTableWidgetItem( QString("LastUpdate")));
    mCRLListTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(sLastUpdate)));
    i++;

    JS_UTIL_getDateTime( crl_info_.uNextUpdate, sNextUpdate );
    mCRLListTable->insertRow(i);
    mCRLListTable->setItem( i, 0, new QTableWidgetItem( QString("NextUpdate")));
    mCRLListTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(sNextUpdate)));
    i++;

    if( crl_info_.pSignAlgorithm )
    {
        mCRLListTable->insertRow(i);
        mCRLListTable->setItem( i, 0, new QTableWidgetItem( QString("SignAlgorithm")));
        mCRLListTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(crl_info_.pSignAlgorithm)));
        i++;
    }

    if( crl_info_.pSignature )
    {
        mCRLListTable->insertRow(i);
        mCRLListTable->setItem( i, 0, new QTableWidgetItem( QString("Signature")));
        mCRLListTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(crl_info_.pSignature)));
        i++;
    }

    if( ext_info_list_ )
    {
        JExtensionInfoList *pCurList = ext_info_list_;

        while( pCurList )
        {
            JDB_PolicyExt   sPolicyExt;
            memset( &sPolicyExt, 0x00, sizeof(sPolicyExt));
            JS_PKI_transExtensionToDBRec( &pCurList->sExtensionInfo, &sPolicyExt );

            mCRLListTable->insertRow(i);
            mCRLListTable->setItem(i,0, new QTableWidgetItem(QString("%1").arg( sPolicyExt.pSN )));
            mCRLListTable->setItem(i,1, new QTableWidgetItem(QString("[%1]%2")
                                                               .arg( sPolicyExt.bCritical )
                                                               .arg( sPolicyExt.pValue )));


            pCurList = pCurList->pNext;
            JS_DB_resetPolicyExt( &sPolicyExt );
            i++;
        }
    }

    if( revoke_info_list_ )
    {
        int k = 0;
        JRevokeInfoList *pCurRevList = revoke_info_list_;

        while( pCurRevList )
        {
            mRevokeListTable->insertRow(k);
            mRevokeListTable->setItem( k, 0, new QTableWidgetItem(QString("%1").arg( pCurRevList->sRevokeInfo.pSerial)));
            mRevokeListTable->setItem( k, 1, new QTableWidgetItem(QString("%1").arg( pCurRevList->sRevokeInfo.uRevokeDate)));

            pCurRevList = pCurRevList->pNext;
            k++;
        }
    }

    JS_BIN_reset( &binCRL );
    JS_DB_resetCRL( &sCRL );
}

void CRLInfoDlg::initUI()
{
    QStringList sCRLLabels = { tr("Field"), tr("Value") };

    mCRLListTable->clear();
    mCRLListTable->horizontalHeader()->setStretchLastSection(true);
    mCRLListTable->setColumnCount(2);
    mCRLListTable->setHorizontalHeaderLabels( sCRLLabels );
    mCRLListTable->verticalHeader()->setVisible(false);

    QStringList sRevokeLabels = { tr("Serial"), tr("RevokedDate") };
    mRevokeListTable->clear();
    mRevokeListTable->horizontalHeader()->setStretchLastSection(true);
    mRevokeListTable->setColumnCount(2);
    mRevokeListTable->setHorizontalHeaderLabels( sRevokeLabels );
    mRevokeListTable->verticalHeader()->setVisible(false);

    mRevokeDetailTable->clear();
    mRevokeDetailTable->horizontalHeader()->setStretchLastSection(true);
    mRevokeDetailTable->setColumnCount(2);
    mRevokeDetailTable->setHorizontalHeaderLabels(sCRLLabels);
    mRevokeDetailTable->verticalHeader()->setVisible(false);

    connect( mCloseBtn, SIGNAL(clicked()), this, SLOT(clickClose()));
    connect( mCRLListTable, SIGNAL(clicked(QModelIndex)), this, SLOT(clickCRLField(QModelIndex)));
    connect( mRevokeListTable, SIGNAL(clicked(QModelIndex)), this, SLOT(clickRevokeField(QModelIndex)));
}

void CRLInfoDlg::clearTable()
{
    int rowCnt = mCRLListTable->rowCount();

    for( int i=0; i < rowCnt; i++ )
        mCRLListTable->removeRow(0);

    rowCnt = mRevokeListTable->rowCount();

    for( int i=0; i < rowCnt; i++ )
        mRevokeListTable->removeRow(0);

    rowCnt = mRevokeDetailTable->rowCount();

    for( int i=0; i < rowCnt; i++ )
        mRevokeDetailTable->removeRow(0);
}

void CRLInfoDlg::clickCRLField(QModelIndex index)
{
    int row = index.row();
    int col = index.column();

    QTableWidgetItem* item = mCRLListTable->item( row, 1 );
    if( item == NULL ) return;

    mCRLDetailText->setPlainText( item->text() );
}

void CRLInfoDlg::clickRevokeField(QModelIndex index)
{
    int row = index.row();
    int col = index.column();

    int rowCnt = mRevokeDetailTable->rowCount();

    for( int i=0; i < rowCnt; i++ )
        mRevokeDetailTable->removeRow(0);

    JRevokeInfoList *pRevInfoList = revoke_info_list_;

    for( int i = 0; i < row; i++ )
    {
        pRevInfoList = pRevInfoList->pNext;
    }


    JDB_PolicyExt   sPolicyExt;
    memset( &sPolicyExt, 0x00, sizeof(sPolicyExt));
    JS_PKI_transExtensionToDBRec( &pRevInfoList->sRevokeInfo.sExtReason, &sPolicyExt );


    mRevokeDetailTable->insertRow(0);
    mRevokeDetailTable->setItem(0,0, new QTableWidgetItem(QString("%1")
                                                                .arg( sPolicyExt.pSN )));
    mRevokeDetailTable->setItem(0,1, new QTableWidgetItem(QString("[%1]%2")
                                                           .arg( sPolicyExt.bCritical )
                                                           .arg( sPolicyExt.pValue )));

    JS_DB_resetPolicyExt( &sPolicyExt );
}
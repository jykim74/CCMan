#include "mainwindow.h"
#include "man_applet.h"

#include "crl_info_dlg.h"
#include "js_pki.h"
#include "js_pki_x509.h"
#include "js_pki_ext.h"
#include "js_util.h"
#include "cc_client.h"
#include "common.h"


CRLInfoDlg::CRLInfoDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    initUI();
    crl_num_ = -1;
    ext_info_list_ = NULL;
    revoke_info_list_ = NULL;

    memset( &crl_info_, 0x00, sizeof(crl_info_));
    tabWidget->setCurrentIndex(0);

#if defined(Q_OS_MAC)
    layout()->setSpacing(5);

    mCRLTab->layout()->setSpacing(5);
    mCRLTab->layout()->setMargin(5);
    mRevokeTab->layout()->setSpacing(5);
    mRevokeTab->layout()->setMargin(5);
#endif
    resize(minimumSizeHint().width(), minimumSizeHint().height());
}

CRLInfoDlg::~CRLInfoDlg()
{
    if( ext_info_list_ ) JS_PKI_resetExtensionInfoList( &ext_info_list_ );
    if( revoke_info_list_ ) JS_PKI_resetRevokeInfoList( &revoke_info_list_ );
}

void CRLInfoDlg::showEvent(QShowEvent *event)
{
    initialize();
}

void CRLInfoDlg::setCRLNum(int crl_num)
{
    crl_num_ = crl_num;
}

void CRLInfoDlg::initialize()
{
    int ret = 0;
    int i = 0;

    BIN binCRL = {0,0};
    BIN binFinger = {0,0};

    char    sThisUpdate[64];
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
        close();
        return;
    }

    JS_PKI_genHash( "SHA1", &binCRL, &binFinger );

    mCRLListTable->insertRow(i);
    mCRLListTable->setRowHeight(i,10);
    mCRLListTable->setItem( i, 0, new QTableWidgetItem( QString("Version")));
    mCRLListTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(crl_info_.nVersion + 1)));
    i++;

    if( crl_info_.pIssuerName )
    {
        mCRLListTable->insertRow(i);
        mCRLListTable->setRowHeight(i,10);
        mCRLListTable->setItem( i, 0, new QTableWidgetItem( QString("IssuerName")));
        mCRLListTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(crl_info_.pIssuerName)));
        i++;
    }


    JS_UTIL_getDateTime( crl_info_.tThisUpdate, sThisUpdate );
    mCRLListTable->insertRow(i);
    mCRLListTable->setRowHeight(i,10);
    mCRLListTable->setItem( i, 0, new QTableWidgetItem( QString("ThisUpdate")));
    mCRLListTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(sThisUpdate)));
    i++;

    JS_UTIL_getDateTime( crl_info_.tNextUpdate, sNextUpdate );
    mCRLListTable->insertRow(i);
    mCRLListTable->setRowHeight(i,10);
    mCRLListTable->setItem( i, 0, new QTableWidgetItem( QString("NextUpdate")));
    mCRLListTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(sNextUpdate)));
    i++;

    if( crl_info_.pSignAlgorithm )
    {
        mCRLListTable->insertRow(i);
        mCRLListTable->setRowHeight(i,10);
        mCRLListTable->setItem( i, 0, new QTableWidgetItem( QString("SignAlgorithm")));
        mCRLListTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(crl_info_.pSignAlgorithm)));
        i++;
    }

    if( crl_info_.pSignature )
    {
        mCRLListTable->insertRow(i);
        mCRLListTable->setRowHeight(i,10);
        mCRLListTable->setItem( i, 0, new QTableWidgetItem( QString("Signature")));
        mCRLListTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(crl_info_.pSignature)));
        i++;
    }

    if( ext_info_list_ )
    {
        JExtensionInfoList *pCurList = ext_info_list_;

        while( pCurList )
        {
            QString strValue;
            QString strSN = pCurList->sExtensionInfo.pOID;
            bool bCrit = pCurList->sExtensionInfo.bCritical;
            getInfoValue( &pCurList->sExtensionInfo, strValue );

            QTableWidgetItem *item = new QTableWidgetItem( strValue );
            if( bCrit )
                item->setIcon(QIcon(":/images/critical.png"));
            else
                item->setIcon(QIcon(":/images/normal.png"));

            mCRLListTable->insertRow(i);
            mCRLListTable->setRowHeight(i,10);
            mCRLListTable->setItem(i,0, new QTableWidgetItem(QString("%1").arg(strSN)));
            mCRLListTable->setItem(i,1, item );

            pCurList = pCurList->pNext;
            i++;
        }
    }

    mCRLListTable->insertRow(i);
    mCRLListTable->setRowHeight(i,10);
    mCRLListTable->setItem(i, 0, new QTableWidgetItem(tr("FingerPrint")));
    mCRLListTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(getHexString(binFinger.pVal, binFinger.nLen))));
    i++;

    if( revoke_info_list_ )
    {
        int k = 0;
        JRevokeInfoList *pCurRevList = revoke_info_list_;

        while( pCurRevList )
        {
            mRevokeListTable->insertRow(k);
            mRevokeListTable->setRowHeight(i,10);
            mRevokeListTable->setItem( k, 0, new QTableWidgetItem(QString("%1").arg( pCurRevList->sRevokeInfo.pSerial)));
            mRevokeListTable->setItem( k, 1, new QTableWidgetItem(QString("%1").arg( pCurRevList->sRevokeInfo.tRevokeDate)));

            pCurRevList = pCurRevList->pNext;
            k++;
        }
    }

    JS_BIN_reset( &binCRL );
    JS_BIN_reset( &binFinger );

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
    mCRLListTable->horizontalHeader()->setStyleSheet( kTableStyle );
    mCRLListTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mCRLListTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList sRevokeLabels = { tr("Serial"), tr("RevokedDate") };
    mRevokeListTable->clear();
    mRevokeListTable->horizontalHeader()->setStretchLastSection(true);
    mRevokeListTable->setColumnCount(2);
    mRevokeListTable->setHorizontalHeaderLabels( sRevokeLabels );
    mRevokeListTable->verticalHeader()->setVisible(false);
    mRevokeListTable->horizontalHeader()->setStyleSheet( kTableStyle );
    mRevokeListTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mRevokeListTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    mRevokeDetailTable->clear();
    mRevokeDetailTable->horizontalHeader()->setStretchLastSection(true);
    mRevokeDetailTable->setColumnCount(2);
    mRevokeDetailTable->setHorizontalHeaderLabels(sCRLLabels);
    mRevokeDetailTable->verticalHeader()->setVisible(false);
    mRevokeDetailTable->horizontalHeader()->setStyleSheet( kTableStyle );
    mRevokeDetailTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mRevokeDetailTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect( mCloseBtn, SIGNAL(clicked()), this, SLOT(close()));
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


    JDB_ProfileExt   sProfileExt;
    memset( &sProfileExt, 0x00, sizeof(sProfileExt));
    JS_PKI_transExtensionToDBRec( &pRevInfoList->sRevokeInfo.sExtReason, &sProfileExt );


    mRevokeDetailTable->insertRow(0);
    mRevokeDetailTable->setRowHeight(0,10);
    mRevokeDetailTable->setItem(0,0, new QTableWidgetItem(QString("%1")
                                                                .arg( sProfileExt.pSN )));
    mRevokeDetailTable->setItem(0,1, new QTableWidgetItem(QString("[%1]%2")
                                                           .arg( sProfileExt.bCritical )
                                                           .arg( sProfileExt.pValue )));

    JS_DB_resetProfileExt( &sProfileExt );
}

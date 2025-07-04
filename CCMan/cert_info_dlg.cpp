#include "mainwindow.h"
#include "man_applet.h"
#include "cert_info_dlg.h"
#include "js_pki.h"
#include "js_pki_x509.h"
#include "js_pki_ext.h"
#include "js_util.h"
#include "cc_client.h"
#include "common.h"

enum {
    FIELD_ALL = 0,
    FIELD_VERSION1_ONLY,
    FIELD_EXTENSION_ONLY,
    FIELD_CRITICAL_ONLY,
    FIELD_ATTRIBUTE_ONLY
};


CertInfoDlg::CertInfoDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    initUI();
    cert_num_ = -1;
    cert_list_ = NULL;
    memset( &cert_bin_, 0x00, sizeof(BIN));
    tabWidget->setCurrentIndex(0);

#if defined(Q_OS_MAC)
    layout()->setSpacing(5);

    mInfoTab->layout()->setSpacing(5);
    mInfoTab->layout()->setMargin(5);
    mPathTab->layout()->setSpacing(5);
    mPathTab->layout()->setMargin(5);
#endif
    resize(minimumSizeHint().width(), minimumSizeHint().height());
}

CertInfoDlg::~CertInfoDlg()
{
    if( cert_list_ ) JS_DB_resetCertList( &cert_list_ );
    JS_BIN_reset( &cert_bin_ );
}

void CertInfoDlg::setCertNum(int cert_num)
{
    cert_num_ = cert_num;
}

void CertInfoDlg::setCertBin( const BIN *pCert )
{
    cert_num_ = -1;
    JS_BIN_reset( &cert_bin_ );
    JS_BIN_copy( &cert_bin_, pCert );
}

void CertInfoDlg::showEvent(QShowEvent *event)
{
    initialize();
    if( cert_num_ >= 0 ) pathInit();
}

void CertInfoDlg::changeFieldType( int index )
{

}

void CertInfoDlg::initialize()
{
    int ret = 0;
    int i = 0;

    BIN binFinger = {0,0};

    JCertInfo  sCertInfo;
    JDB_Cert    sCert;
    JExtensionInfoList *pExtInfoList = NULL;
    char    sNotBefore[64];
    char    sNotAfter[64];

    int nType = mFieldTypeCombo->currentIndex();

    memset( &sCert, 0x00, sizeof(sCert));

    if( cert_num_ < 0 && cert_bin_.nLen <= 0)
    {
        manApplet->warningBox( tr( "Select certificate"), this );
        close();
        return;
    }

    clearTable();

    if( cert_num_ >= 0 )
    {
        manApplet->ccClient()->getCert( cert_num_, &sCert );
        memset( &sCertInfo, 0x00, sizeof(sCertInfo));
        JS_BIN_reset( &cert_bin_ );
        JS_BIN_decodeHex( sCert.pCert, &cert_bin_ );
    }

    ret = JS_PKI_getCertInfo( &cert_bin_, &sCertInfo, &pExtInfoList );
    if( ret != 0 )
    {
        manApplet->warningBox( tr("fail to get certificate information"), this );
        JS_BIN_reset( &cert_bin_ );
        this->hide();
        return;
    }

    JS_PKI_genHash( "SHA1", &cert_bin_, &binFinger );

    if( nType == FIELD_ALL || nType == FIELD_VERSION1_ONLY )
    {

        mFieldTable->insertRow(i);
        mFieldTable->setRowHeight(i,10);
        mFieldTable->setItem( i, 0, new QTableWidgetItem( QString("Version")));
        mFieldTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(sCertInfo.nVersion + 1)));
        i++;

        if( sCertInfo.pSerial )
        {
            mFieldTable->insertRow(i);
            mFieldTable->setRowHeight(i,10);
            mFieldTable->setItem(i, 0, new QTableWidgetItem(QString("Serial")));
            mFieldTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(sCertInfo.pSerial)));
            i++;
        }

        JS_UTIL_getDateTime( sCertInfo.tNotBefore, sNotBefore );
        mFieldTable->insertRow(i);
        mFieldTable->setRowHeight(i,10);
        mFieldTable->setItem( i, 0, new QTableWidgetItem( QString("NotBefore")));
        mFieldTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(sNotBefore)));
        i++;

        JS_UTIL_getDateTime( sCertInfo.tNotAfter, sNotAfter );
        mFieldTable->insertRow(i);
        mFieldTable->setRowHeight(i,10);
        mFieldTable->setItem( i, 0, new QTableWidgetItem( QString("NotAfter")));
        mFieldTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(sNotAfter)));
        i++;

        if( sCertInfo.pSubjectName )
        {
            QString name = QString::fromUtf8( sCertInfo.pSubjectName );

            mFieldTable->insertRow(i);
            mFieldTable->setRowHeight(i,10);
            mFieldTable->setItem(i, 0, new QTableWidgetItem(QString("SubjectName")));
            mFieldTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg( name )));
            i++;
        }

        if( sCertInfo.pPublicKey )
        {
            mFieldTable->insertRow(i);
            mFieldTable->setRowHeight(i,10);
            mFieldTable->setItem(i, 0, new QTableWidgetItem(QString("PublicKey")));
            mFieldTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(sCertInfo.pPublicKey)));
            i++;
        }

        if( sCertInfo.pIssuerName )
        {
            mFieldTable->insertRow(i);
            mFieldTable->setRowHeight(i,10);
            mFieldTable->setItem(i, 0, new QTableWidgetItem(QString("IssuerName")));
            mFieldTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(sCertInfo.pIssuerName)));
            i++;
        }

        if( sCertInfo.pSignAlgorithm )
        {
            mFieldTable->insertRow(i);
            mFieldTable->setRowHeight(i,10);
            mFieldTable->setItem(i, 0, new QTableWidgetItem(QString("SigAlgorithm")));
            mFieldTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(sCertInfo.pSignAlgorithm)));
            i++;
        }

        if( sCertInfo.pSignature )
        {
            mFieldTable->insertRow(i);
            mFieldTable->setRowHeight(i,10);
            mFieldTable->setItem(i, 0, new QTableWidgetItem(QString("Signature")));
            mFieldTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(sCertInfo.pSignature)));
            i++;
        }
    }

    if( nType == FIELD_ALL || nType == FIELD_EXTENSION_ONLY || nType == FIELD_CRITICAL_ONLY )
    {

        if( pExtInfoList )
        {
            JExtensionInfoList *pCurList = pExtInfoList;

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

                mFieldTable->insertRow(i);
                mFieldTable->setRowHeight(i,10);
                mFieldTable->setItem(i,0, new QTableWidgetItem( QString("%1").arg(strSN)));
                mFieldTable->setItem(i, 1, item );


                pCurList = pCurList->pNext;
                i++;
            }
        }
    }


    if( nType == FIELD_ALL || nType == FIELD_ATTRIBUTE_ONLY )
    {
        mFieldTable->insertRow(i);
        mFieldTable->setRowHeight(i,10);
        mFieldTable->setItem(i, 0, new QTableWidgetItem(tr("FingerPrint")));
        mFieldTable->setItem(i, 1, new QTableWidgetItem(QString("%1").arg(getHexString(binFinger.pVal, binFinger.nLen))));
        i++;
    }

    JS_BIN_reset( &binFinger );

    JS_PKI_resetCertInfo( &sCertInfo );
    if( pExtInfoList ) JS_PKI_resetExtensionInfoList( &pExtInfoList );
    JS_DB_resetCert( &sCert );
}

void CertInfoDlg::pathInit()
{
    int i = 0;
    int count = 0;
    JCC_Cert sCert;
    const JCC_CertList* pCurList = NULL;
    CCClient* ccClient = manApplet->ccClient();

    memset( &sCert, 0x00, sizeof(sCert));

    ccClient->getCert( cert_num_, &sCert );
    JS_DB_createCertList( &sCert, &cert_list_ );

    int nIssuerNum = sCert.nIssuerNum;
    JS_DB_resetCert( &sCert );

    while( nIssuerNum > 0 )
    {
        ccClient->getCert( nIssuerNum, &sCert );
        JS_DB_appendCertList( cert_list_, &sCert );

        nIssuerNum = sCert.nIssuerNum;
        JS_DB_resetCert( &sCert );
    }

    mCertPathTree->clear();
    mCertPathTree->header()->setVisible(false);
    mCertPathTree->setColumnCount(1);

    QList<QTreeWidgetItem *> items;
    QTreeWidgetItem* pPrevItem = NULL;

    count = JS_DB_countCertList( cert_list_ );

    for( int i = 0; i < count; i++ )
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(0);

        pCurList = JS_DB_getCertListAt( count - i - 1, cert_list_ );
        if( pCurList == NULL ) continue;

        item->setText( 0, pCurList->sCert.pSubjectDN );

        if( i == 0 )
        {
            mCertPathTree->insertTopLevelItem(0, item );
        }
        else
        {
            pPrevItem->addChild( item );
        }

        pPrevItem = item;
    }

    mCertPathTree->expandAll();
}

void CertInfoDlg::initUI()
{
    QStringList sBaseLabels = { tr("Field"), tr("Value") };
    QStringList sFieldTypes = { tr("All"), tr("Version1 Only"), tr("Extension Only"), tr("Critical Extension Only"), tr("Attribute Only") };

    mFieldTypeCombo->addItems( sFieldTypes );

    mFieldTable->clear();
    mFieldTable->horizontalHeader()->setStretchLastSection(true);
    mFieldTable->setColumnCount(2);
    mFieldTable->setHorizontalHeaderLabels( sBaseLabels );
    mFieldTable->verticalHeader()->setVisible(false);
    mFieldTable->horizontalHeader()->setStyleSheet( kTableStyle );
    mFieldTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mFieldTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect( mCloseBtn, SIGNAL(clicked()), this, SLOT(close()));
    connect( mFieldTable, SIGNAL(clicked(QModelIndex)), this, SLOT(clickField(QModelIndex)));
    connect( mCheckBtn, SIGNAL(clicked()), this, SLOT(clickCheck()));
    connect( mFieldTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(changeFieldType(int)));
}


void CertInfoDlg::clickField(QModelIndex index)
{
    int row = index.row();
    int col = index.column();

    QTableWidgetItem* item = mFieldTable->item( row, 1 );
    if( item == NULL ) return;

    mDetailText->setPlainText( item->text() );
}

void CertInfoDlg::clickCheck()
{
    int ret = 0;
    int i = 0;
    tabWidget->setCurrentIndex(1);

    BINList *pChainList = NULL;
    BIN     binCert = {0,0};
    JCC_CertList *pCurList = NULL;

    int nCount = JS_DB_countCertList( cert_list_ );
    pCurList = cert_list_;

    while( pCurList )
    {
        if( i == nCount - 1 )
        {
            JS_BIN_decodeHex( pCurList->sCert.pCert, &binCert );
        }
        else
        {
            BIN bin = {0,0};
            JS_BIN_decodeHex( pCurList->sCert.pCert, &bin );

            if( pChainList == NULL )
                JS_BIN_createList( &bin, &pChainList );
            else
                JS_BIN_appendList( pChainList, &bin );

            JS_BIN_reset( &bin );
        }

        i++;
        pCurList = pCurList->pNext;
    }

    ret = JS_PKI_checkValidPath( pChainList, NULL, &binCert );

    QString strRes = QString( "Ret: %1").arg( ret );
    mCertStatusText->setPlainText( strRes );

    if( pChainList ) JS_BIN_resetList( &pChainList );
    JS_BIN_reset( &binCert );
}

void CertInfoDlg::clearTable()
{
    int rowCnt = mFieldTable->rowCount();

    for( int i=0; i < rowCnt; i++ )
        mFieldTable->removeRow(0);
}

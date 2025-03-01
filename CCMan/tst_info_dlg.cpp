#include "tst_info_dlg.h"
#include "man_applet.h"
#include "js_pki.h"
#include "js_tsp.h"
#include "js_pkcs7.h"
#include "cc_client.h"

TSTInfoDlg::TSTInfoDlg(QWidget *parent) :
    QDialog(parent)
{
    seq_ = -1;
    setupUi(this);
    initUI();

    connect( mCloseBtn, SIGNAL(clicked()), this, SLOT(close()));

#if defined(Q_OS_MAC)
    layout()->setSpacing(5);
#endif
    resize(minimumSizeHint().width(), minimumSizeHint().height());
}

TSTInfoDlg::~TSTInfoDlg()
{

}

void TSTInfoDlg::setSeq( int nSeq )
{
    seq_ = nSeq;
}

void TSTInfoDlg::showEvent(QShowEvent *event)
{
    initialize();
}

void TSTInfoDlg::initialize()
{
    int i = 0;
    int ret = 0;
    BIN binTST = {0,0};
    JTSTInfo    sTSTInfo;
    JCC_TSP     sTSP;
    QString strAccuracy;
    QString strMsgImprint;

    memset( &sTSTInfo, 0x00, sizeof(sTSTInfo));
    memset( &sTSP, 0x00, sizeof(sTSP));

    CCClient* ccClient = manApplet->ccClient();

    if( ccClient == NULL ) return;

    if( seq_ < 0 )
    {
        manApplet->warningBox( tr( "You have to set TST sequece"), this );
        this->hide();
        return;
    }

    clearTable();

    ccClient->getTSP( seq_, &sTSP );

    JS_BIN_decodeHex( sTSP.pTSTInfo, &binTST );

    ret = JS_TSP_decodeTSTInfo( &binTST, &sTSTInfo );
    if( ret != 0 )
    {
        manApplet->warningBox( tr( "Fail to decode TST message"), this );
        this->hide();
        goto end;
    }


    mInfoTable->insertRow(i);
    mInfoTable->setRowHeight( i, 10 );
    mInfoTable->setItem( i, 0, new QTableWidgetItem(QString("Version")));
    mInfoTable->setItem( i, 1, new QTableWidgetItem(QString("%1").arg(sTSTInfo.nVersion)));
    i++;

    mInfoTable->insertRow(i);
    mInfoTable->setRowHeight( i, 10 );
    mInfoTable->setItem( i, 0, new QTableWidgetItem(QString("Order")));
    mInfoTable->setItem( i, 1, new QTableWidgetItem(QString("%1").arg(sTSTInfo.nOrder)));
    i++;

    mInfoTable->insertRow(i);
    mInfoTable->setRowHeight( i, 10 );
    mInfoTable->setItem( i, 0, new QTableWidgetItem(QString("Serial")));
    mInfoTable->setItem( i, 1, new QTableWidgetItem(QString("%1").arg(sTSTInfo.nSerial)));
    i++;

    if( sTSTInfo.pPolicy )
    {
        mInfoTable->insertRow(i);
        mInfoTable->setRowHeight( i, 10 );
        mInfoTable->setItem( i, 0, new QTableWidgetItem(QString("Policy")));
        mInfoTable->setItem( i, 1, new QTableWidgetItem(QString("%1").arg(sTSTInfo.pPolicy)));
        i++;
    }

    if( sTSTInfo.pGenName )
    {
        mInfoTable->insertRow(i);
        mInfoTable->setRowHeight( i, 10 );
        mInfoTable->setItem( i, 0, new QTableWidgetItem(QString("GenName")));
        mInfoTable->setItem( i, 1, new QTableWidgetItem(QString("%1").arg(sTSTInfo.pGenName)));
        i++;
    }

    if( sTSTInfo.pGenTime )
    {
        mInfoTable->insertRow(i);
        mInfoTable->setRowHeight( i, 10 );
        mInfoTable->setItem( i, 0, new QTableWidgetItem(QString("GenTime")));
        mInfoTable->setItem( i, 1, new QTableWidgetItem(QString("%1").arg(sTSTInfo.pGenTime)));
        i++;
    }

    if( sTSTInfo.pNonce )
    {
        mInfoTable->insertRow(i);
        mInfoTable->setRowHeight( i, 10 );
        mInfoTable->setItem( i, 0, new QTableWidgetItem(QString("Nonce")));
        mInfoTable->setItem( i, 1, new QTableWidgetItem(QString("%1").arg(sTSTInfo.pNonce)));
        i++;
    }


    strAccuracy = QString( "Sec:%1 millis:%2 micro:%3")
            .arg( sTSTInfo.sAccuracy.nSec )
            .arg( sTSTInfo.sAccuracy.nMiliSec )
            .arg( sTSTInfo.sAccuracy.nMicroSec );

    mInfoTable->insertRow(i);
    mInfoTable->setRowHeight( i, 10 );
    mInfoTable->setItem( i, 0, new QTableWidgetItem(QString("Accuracy")));
    mInfoTable->setItem( i, 1, new QTableWidgetItem(QString("%1").arg(strAccuracy)));
    i++;


    strMsgImprint = QString( "%1|%2")
            .arg( sTSTInfo.sMsgImprint.pAlg )
            .arg( sTSTInfo.sMsgImprint.pImprint );

    mInfoTable->insertRow(i);
    mInfoTable->setRowHeight( i, 10 );
    mInfoTable->setItem( i, 0, new QTableWidgetItem(QString("MsgImprint")));
    mInfoTable->setItem( i, 1, new QTableWidgetItem(QString("%1").arg(strMsgImprint)));
    i++;

end :
    JS_TSP_resetTSTInfo( &sTSTInfo );
    JS_DB_resetTSP( &sTSP );
    JS_BIN_reset( &binTST );
}

void TSTInfoDlg::initUI()
{
    QStringList sBaseLabels = { tr("Field"), tr("Value") };
    QString style = "QHeaderView::section {background-color:#404040;color:#FFFFFF;}";

    mInfoTable->clear();
    mInfoTable->horizontalHeader()->setStretchLastSection(true);
    mInfoTable->setColumnCount(2);
    mInfoTable->setHorizontalHeaderLabels( sBaseLabels );
    mInfoTable->verticalHeader()->setVisible(false);
    mInfoTable->horizontalHeader()->setStyleSheet( style );
    mInfoTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mInfoTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void TSTInfoDlg::clearTable()
{
    int rowCnt = mInfoTable->rowCount();

    for( int i=0; i < rowCnt; i++ )
        mInfoTable->removeRow(0);
}

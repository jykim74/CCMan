#include "make_crl_policy_dlg.h"
#include "mainwindow.h"
#include "man_applet.h"
#include "common.h"
#include "cc_client.h"
#include "js_db.h"
#include "man_tree_item.h"

static QStringList sHashList = { "SHA1", "SHA224", "SHA256", "SHA384", "SHA512" };
static QStringList sTypeList = { "URI", "email", "DNS" };
static QStringList sVersionList = { "V1", "V2" };


MakeCRLPolicyDlg::MakeCRLPolicyDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    initUI();
    connectExtends();
    setExtends();
    setTableMenus();

    is_edit_ = false;
    policy_num_ = -1;
}

MakeCRLPolicyDlg::~MakeCRLPolicyDlg()
{

}

void MakeCRLPolicyDlg::setEdit(bool is_edit)
{
    is_edit_ = is_edit;
}

void MakeCRLPolicyDlg::setPolicyNum(int policy_num)
{
    policy_num_ = policy_num;
}


void MakeCRLPolicyDlg::initialize()
{
    mCRLTab->setCurrentIndex(0);

    if( is_edit_ )
        loadPolicy();
    else
        defaultPolicy();
}

void MakeCRLPolicyDlg::showEvent(QShowEvent *event)
{
    initialize();
}

void MakeCRLPolicyDlg::loadPolicy()
{    
    JCC_CRLPolicy sCRLPolicy;
    memset( &sCRLPolicy, 0x00, sizeof(sCRLPolicy));
    manApplet->ccClient()->getCRLPolicy( policy_num_, &sCRLPolicy );

    mNameText->setText( sCRLPolicy.pName );
    mVersionCombo->setCurrentIndex( sCRLPolicy.nVersion );
    mHashCombo->setCurrentText( sCRLPolicy.pHash );

    if( sCRLPolicy.nLastUpdate == 0 )
    {
        mUseFromNowCheck->setChecked(true);
        mValidDaysText->setText( QString("%1").arg(sCRLPolicy.nNextUpdate));
    }
    else
    {
        QDateTime lastUpdate;
        QDateTime nextUpdate;

        lastUpdate.setTime_t( sCRLPolicy.nLastUpdate );
        nextUpdate.setTime_t( sCRLPolicy.nNextUpdate );

        mLastUpdateDateTime->setDateTime(lastUpdate);
        mNextUpdateDateTime->setDateTime(nextUpdate );
    }

    clickUseFromNow();

    JCC_PolicyExtList   *pPolicyExtList = NULL;
    JCC_PolicyExtList   *pCurList = NULL;

    manApplet->ccClient()->getCRLPolicyExtList( policy_num_, &pPolicyExtList );
    pCurList = pPolicyExtList;

    while( pCurList )
    {
        if( strcasecmp( pCurList->sPolicyExt.pSN, kExtNameCRLNum.toStdString().c_str() ) == 0 )
            setCRLNumUse( &pCurList->sPolicyExt );
        else if( strcasecmp( pCurList->sPolicyExt.pSN, kExtNameAKI.toStdString().c_str() ) == 0 )
            setAKIUse( &pCurList->sPolicyExt );
        else if( strcasecmp( pCurList->sPolicyExt.pSN, kExtNameIDP.toStdString().c_str() ) == 0 )
            setIDPUse( &pCurList->sPolicyExt );
        else if( strcasecmp( pCurList->sPolicyExt.pSN, kExtNameIAN.toStdString().c_str() ) == 0 )
            setIANUse( &pCurList->sPolicyExt );

        pCurList = pCurList->pNext;
    }

    JS_DB_resetCRLPolicy( &sCRLPolicy );
    if( pPolicyExtList ) JS_DB_resetPolicyExtList( &pPolicyExtList );
}

void MakeCRLPolicyDlg::defaultPolicy()
{
    int rowCnt = 0;
    mNameText->setText("");

    mCRLNumText->setText("");
    mCRLNumUseCheck->setChecked(false);
    mCRLNumAutoCheck->setChecked(false);
    mCRLNumCriticalCheck->setChecked(false);

    mAKIUseCheck->setChecked(false);
    mAKICriticalCheck->setChecked(false);
    mAKICertIssuerCheck->setChecked(false);
    mAKICertSerialCheck->setChecked(false);

    rowCnt = mIDPTable->rowCount();
    for( int i=0; i < rowCnt; i++ )
        mIDPTable->removeRow(0);
    mIDPUseCheck->setChecked(false);
    mIDPCriticalCheck->setChecked(false);

    rowCnt = mIANTable->rowCount();
    for( int i=0; i < rowCnt; i++ )
        mIANTable->removeRow(0);
    mIANUseCheck->setChecked(false);
    mIANCriticalCheck->setChecked(false);
    mIANText->setText("");

    mVersionCombo->setCurrentIndex(1);
    mHashCombo->setCurrentIndex(2);

    mUseFromNowCheck->setChecked(true);
    clickUseFromNow();
    mValidDaysText->setText("10");
}

void MakeCRLPolicyDlg::accept()
{
    QString strName = mNameText->text();

    if( strName.isEmpty() )
    {
        manApplet->warningBox( tr( "You have to insert name"), this );
        mNameText->setFocus();
        return;
    }

    int nPolicyNum = manApplet->ccClient()->getNum( ITEM_TYPE_CRL_POLICY );
    JCC_CRLPolicy sCRLPolicy;

    memset( &sCRLPolicy, 0x00, sizeof(sCRLPolicy) );
    int nLastUpdate = 0;
    int nNextUpdate = 0;


    if( mUseFromNowCheck->isChecked() )
    {       
        nLastUpdate = 0;
        nNextUpdate = mValidDaysText->text().toLong();
    }
    else {
        QDateTime lastTime;
        QDateTime nextTime;

        lastTime.setDate( mLastUpdateDateTime->date() );
        nextTime.setDate( mNextUpdateDateTime->date() );

        nLastUpdate = lastTime.toTime_t();
        nNextUpdate = nextTime.toTime_t();
    }

    JS_DB_setCRLPolicy( &sCRLPolicy,
                        nPolicyNum,
                        strName.toStdString().c_str(),
                        mVersionCombo->currentIndex(),
                        nLastUpdate,
                        nNextUpdate,
                        mHashCombo->currentText().toStdString().c_str() );


    if( is_edit_ )
    {       
        manApplet->ccClient()->modCRLPolicy( policy_num_, &sCRLPolicy );
        manApplet->ccClient()->delCRLPolicyExts( policy_num_ );
        nPolicyNum = policy_num_;
    }
    else
    {
        manApplet->ccClient()->addCRLPolicy( &sCRLPolicy );
    }


    /* need to set extend fields here */

    if( mCRLNumUseCheck->isChecked() ) saveCRLNumUse( nPolicyNum );
    if( mIDPUseCheck->isChecked() ) saveIDPUse( nPolicyNum );
    if( mAKIUseCheck->isChecked() ) saveAKIUse( nPolicyNum );
    if( mIANUseCheck->isChecked() ) saveIANUse( nPolicyNum );

    /* ....... */

    JS_DB_resetCRLPolicy( &sCRLPolicy );
    manApplet->mainWindow()->createRightCRLPolicyList();
    QDialog::accept();
}

void MakeCRLPolicyDlg::initUI()
{
    mHashCombo->addItems(sHashList);
    mIDPCombo->addItems(sTypeList);
    mIANCombo->addItems(sTypeList);
    mVersionCombo->addItems(sVersionList);

    QDateTime   now;
    now.setTime_t(time(NULL));

    mLastUpdateDateTime->setDateTime(now);
    mNextUpdateDateTime->setDateTime(now);
}

void MakeCRLPolicyDlg::connectExtends()
{
    connect( mCRLNumUseCheck, SIGNAL(clicked()), this, SLOT(clickCRLNum()));
    connect( mAKIUseCheck, SIGNAL(clicked()), this, SLOT(clickAKI()));
    connect( mIDPUseCheck, SIGNAL(clicked()), this, SLOT(clickIDP()));
    connect( mIANUseCheck, SIGNAL(clicked()), this, SLOT(clickIAN()));
    connect( mUseFromNowCheck, SIGNAL(clicked()), this, SLOT(clickUseFromNow()));

    connect( mIDPAddBtn, SIGNAL(clicked()), this, SLOT(addIDP()));
    connect( mIANAddBtn, SIGNAL(clicked()), this, SLOT(addIAN()));

    connect( mIDPClearBtn, SIGNAL(clicked()), this, SLOT(clearIDP()));
    connect( mIANClearBtn, SIGNAL(clicked()), this, SLOT(clearIAN()));
}

void MakeCRLPolicyDlg::setExtends()
{
    clickCRLNum();
    clickAKI();
    clickIDP();
    clickIAN();
}

void MakeCRLPolicyDlg::setTableMenus()
{
    QStringList sDPNLabels = { "Type", "Value" };
    mIDPTable->setColumnCount(2);
    mIDPTable->horizontalHeader()->setStretchLastSection(true);
    mIDPTable->setHorizontalHeaderLabels(sDPNLabels);
    mIDPTable->verticalHeader()->setVisible(false);

    QStringList sIANLabels = { "Type", "Value" };
    mIANTable->setColumnCount(2);
    mIANTable->horizontalHeader()->setStretchLastSection(true);
    mIANTable->setHorizontalHeaderLabels(sIANLabels);
    mIANTable->verticalHeader()->setVisible(false);
}

void MakeCRLPolicyDlg::clickCRLNum()
{
    bool bStatus = mCRLNumUseCheck->isChecked();

    mCRLNumCriticalCheck->setEnabled(bStatus);
    mCRLNumText->setEnabled(bStatus);
    mCRLNumAutoCheck->setEnabled(bStatus);
}

void MakeCRLPolicyDlg::clickAKI()
{
    bool bStatus = mAKIUseCheck->isChecked();

    mAKICriticalCheck->setEnabled(bStatus);
    mAKICertIssuerCheck->setEnabled(bStatus);
    mAKICertSerialCheck->setEnabled(bStatus);
}

void MakeCRLPolicyDlg::clickIDP()
{
    bool bStatus = mIDPUseCheck->isChecked();

    mIDPCriticalCheck->setEnabled(bStatus);
    mIDPAddBtn->setEnabled(bStatus);
    mIDPText->setEnabled(bStatus);
    mIDPTable->setEnabled(bStatus);
    mIDPCombo->setEnabled(bStatus);
}

void MakeCRLPolicyDlg::clickIAN()
{
    bool bStatus = mIANUseCheck->isChecked();

    mIANCriticalCheck->setEnabled(bStatus);
    mIANText->setEnabled(bStatus);
    mIANCombo->setEnabled(bStatus);
    mIANTable->setEnabled(bStatus);
    mIANAddBtn->setEnabled(bStatus);
}

void MakeCRLPolicyDlg::clickUseFromNow()
{
    bool bStatus = mUseFromNowCheck->isChecked();

    mValidDaysText->setEnabled( bStatus );
    mLastUpdateDateTime->setEnabled( !bStatus );
    mNextUpdateDateTime->setEnabled( !bStatus );
}

void MakeCRLPolicyDlg::addIDP()
{
    QString strType = mIDPCombo->currentText();
    QString strVal = mIDPText->text();

    int row = mIDPTable->rowCount();
    mIDPTable->setRowCount( row + 1 );

    mIDPTable->setItem( row, 0, new QTableWidgetItem( strType ));
    mIDPTable->setItem( row, 1, new QTableWidgetItem( strVal ));
}

void MakeCRLPolicyDlg::addIAN()
{
    QString strType = mIANCombo->currentText();
    QString strVal = mIANText->text();

    int row = mIANTable->rowCount();
    mIANTable->setRowCount( row + 1 );

    mIANTable->setItem( row, 0, new QTableWidgetItem( strType ));
    mIANTable->setItem( row, 1, new QTableWidgetItem( strVal ));
}

void MakeCRLPolicyDlg::clearIDP()
{
    int nCnt = mIDPTable->rowCount();

    for( int i=0; i < nCnt; i++)
        mIDPTable->removeRow(0);
}

void MakeCRLPolicyDlg::clearIAN()
{
    int nCnt = mIANTable->rowCount();

    for( int i=0; i < nCnt; i++)
        mIANTable->removeRow(0);
}

void MakeCRLPolicyDlg::saveCRLNumUse( int nPolicyNum )
{
    JCC_PolicyExt   sPolicyExt;
    memset( &sPolicyExt, 0x00, sizeof(sPolicyExt));

    QString strVal;

    if( mCRLNumAutoCheck->isChecked() )
        strVal = "auto";
    else {
        strVal = mCRLNumText->text();
    }

    JS_DB_setPolicyExt( &sPolicyExt, -1, nPolicyNum, mCRLNumCriticalCheck->isChecked(), "crlNumber", strVal.toStdString().c_str() );

    manApplet->ccClient()->addCRLPolicyExt( nPolicyNum, &sPolicyExt );
    JS_DB_resetPolicyExt( &sPolicyExt );
}

void MakeCRLPolicyDlg::saveAKIUse( int nPolicyNum )
{
    JCC_PolicyExt   sPolicyExt;
    memset( &sPolicyExt, 0x00, sizeof(sPolicyExt));

    QString strVal;

    if( mAKICertIssuerCheck->isChecked() )
        strVal += "ISSUER#";

    if( mAKICertSerialCheck->isChecked() )
        strVal += "SERIAL#";

    JS_DB_setPolicyExt( &sPolicyExt, -1, nPolicyNum, mAKICriticalCheck->isChecked(), "authorityKeyIdentifier", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCRLPolicyExt( nPolicyNum, &sPolicyExt );
    JS_DB_resetPolicyExt( &sPolicyExt );
}

void MakeCRLPolicyDlg::saveIDPUse( int nPolicyNum )
{
    JCC_PolicyExt sPolicyExt;
    memset( &sPolicyExt, 0x00, sizeof(sPolicyExt));

    QString strVal;

    for( int i = 0; i < mIDPTable->rowCount(); i++ )
    {
        QString strType;
        QString strData;

        strType = mIDPTable->takeItem(i,0)->text();
        strData = mIDPTable->takeItem(i,1)->text();

        if( i != 0 ) strVal += "#";
        strVal += strType;
        strVal += "$";
        strVal += strData;
    }

    JS_DB_setPolicyExt( &sPolicyExt, -1, nPolicyNum, mIDPCriticalCheck->isChecked(), "issuingDistributionPoint", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCRLPolicyExt( nPolicyNum, &sPolicyExt );
    JS_DB_resetPolicyExt( &sPolicyExt );
}

void MakeCRLPolicyDlg::saveIANUse( int nPolicyNum )
{
    JCC_PolicyExt   sPolicyExt;
    memset( &sPolicyExt, 0x00, sizeof(sPolicyExt));

    QString strVal = "";

    for( int i=0; i < mIANTable->rowCount(); i++ )
    {
        QString strType;
        QString strData;

        strType = mIANTable->takeItem(i,0)->text();
        strData = mIANTable->takeItem(i,1)->text();

        if( i != 0 ) strVal += "#";
        strVal += strType;
        strVal += "$";
        strVal += strData;
    }

    JS_DB_setPolicyExt( &sPolicyExt, -1, nPolicyNum, mIANCriticalCheck->isChecked(), "issuerAltName", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCRLPolicyExt( nPolicyNum, &sPolicyExt );
    JS_DB_resetPolicyExt( &sPolicyExt );
}

void MakeCRLPolicyDlg::setCRLNumUse( JCC_PolicyExt *pPolicyExt )
{
    mCRLNumUseCheck->setChecked(true);
    mCRLNumCriticalCheck->setChecked( pPolicyExt->bCritical );
    clickCRLNum();

    QString strVal = pPolicyExt->pValue;

    if( strVal == "auto" )
        mCRLNumAutoCheck->setChecked(true);
    else
        mCRLNumText->setText( strVal );
}

void MakeCRLPolicyDlg::setAKIUse( JCC_PolicyExt *pPolicyExt )
{
    mAKIUseCheck->setChecked(true);
    mAKICriticalCheck->setChecked( pPolicyExt->bCritical );
    clickAKI();

    QString strVal = pPolicyExt->pValue;

    mAKICertIssuerCheck->setChecked( strVal.contains("ISSUER") );
    mAKICertSerialCheck->setChecked( strVal.contains("SERIAL") );
}

void MakeCRLPolicyDlg::setIDPUse( JCC_PolicyExt *pPolicyExt )
{
    mIDPUseCheck->setChecked(true);
    mIDPCriticalCheck->setChecked( pPolicyExt->bCritical );
    clickIDP();

    QString strVal = pPolicyExt->pValue;

    QStringList valList = strVal.split("#");
    for( int i=0; i < valList.size(); i++ )
    {
        QString info = valList.at(i);

        QStringList infoList = info.split("$");
        QString strType = infoList.at(0);
        QString strData = infoList.at(1);

        mIDPTable->insertRow(i);
        mIDPTable->setItem(i, 0, new QTableWidgetItem(strType));
        mIDPTable->setItem(i, 1, new QTableWidgetItem(strData));
    }
}

void MakeCRLPolicyDlg::setIANUse( JCC_PolicyExt *pPolicyExt )
{
    mIANUseCheck->setChecked(true);
    mIANCriticalCheck->setChecked( pPolicyExt->bCritical );
    clickIAN();

    QString strVal = pPolicyExt->pValue;

    QStringList valList = strVal.split("#");

    for( int i=0; i < valList.size(); i++ )
    {
        QString info = valList.at(i);

        QStringList infoList = info.split("$");
        QString strType = infoList.at(0);
        QString strData = infoList.at(1);

        mIANTable->insertRow(i);
        mIANTable->setItem( i, 0, new QTableWidgetItem(strType));
        mIANTable->setItem( i, 1, new QTableWidgetItem(strData));
    }
}

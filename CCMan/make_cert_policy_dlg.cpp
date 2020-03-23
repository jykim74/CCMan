#include "make_cert_policy_dlg.h"
#include "mainwindow.h"
#include "man_applet.h"
#include "common.h"
#include "cc_client.h"
#include "js_db.h"
#include "js_db_data.h"
#include "man_tree_item.h"


MakeCertPolicyDlg::MakeCertPolicyDlg(QWidget *parent) :
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

MakeCertPolicyDlg::~MakeCertPolicyDlg()
{

}

void MakeCertPolicyDlg::setEdit(bool is_edit)
{
    is_edit_ = is_edit;
}

void MakeCertPolicyDlg::setPolicyNum(int policy_num)
{
    policy_num_ = policy_num;
}

void MakeCertPolicyDlg::showEvent(QShowEvent *event)
{
    initialize();


}

void MakeCertPolicyDlg::initialize()
{
    mCertTab->setCurrentIndex(0);

    if( is_edit_ )
        loadPolicy();
    else
        defaultPolicy();
}

void MakeCertPolicyDlg::loadPolicy()
{
    JCC_CertPolicy      sCertPolicy;
    memset( &sCertPolicy, 0x00, sizeof(sCertPolicy));

    QDateTime notBefore;
    QDateTime notAfter;

    manApplet->ccClient()->getCertPolicy( policy_num_, &sCertPolicy );

    mNameText->setText( sCertPolicy.pName );
    mVersionCombo->setCurrentIndex( sCertPolicy.nVersion );
    mHashCombo->setCurrentText( sCertPolicy.pHash );
    mSubjectDNText->setText( sCertPolicy.pDNTemplate );
    mDivideNumText->setText( QString("%1").arg(sCertPolicy.nDivideNum));

    if( sCertPolicy.nNotBefore == 0 )
    {
        mUseDaysCheck->setChecked(true);
        mDaysText->setText( QString("%1").arg(sCertPolicy.nNotAfter));
    }
    else {
        notBefore.setTime_t( sCertPolicy.nNotBefore );
        notAfter.setTime_t( sCertPolicy.nNotAfter );

        mNotBeforeDateTime->setDateTime( notBefore );
        mNotAfterDateTime->setDateTime( notAfter );
    }

    JCC_PolicyExtList   *pPolicyExtList = NULL;
    JCC_PolicyExtList   *pCurList = NULL;

    manApplet->ccClient()->getCertPolicyExtList( policy_num_, &pPolicyExtList );
    pCurList = pPolicyExtList;

    while( pCurList )
    {
        if( strcasecmp( pCurList->sPolicyExt.pSN, kExtNameAIA.toStdString().c_str() ) == 0 )
            setAIAUse( &pCurList->sPolicyExt );
        else if( strcasecmp( pCurList->sPolicyExt.pSN, kExtNameAKI.toStdString().c_str() ) == 0 )
            setAKIUse( &pCurList->sPolicyExt );
        else if( strcasecmp( pCurList->sPolicyExt.pSN, kExtNameBC.toStdString().c_str() ) == 0 )
            setBCUse( &pCurList->sPolicyExt );
        else if( strcasecmp( pCurList->sPolicyExt.pSN, kExtNameCRLDP.toStdString().c_str() ) == 0 )
            setCRLDPUse( &pCurList->sPolicyExt );
        else if( strcasecmp( pCurList->sPolicyExt.pSN, kExtNameEKU.toStdString().c_str() ) == 0 )
            setEKUUse( &pCurList->sPolicyExt );
        else if( strcasecmp( pCurList->sPolicyExt.pSN, kExtNameIAN.toStdString().c_str() ) == 0 )
            setIANUse( &pCurList->sPolicyExt );
        else if( strcasecmp( pCurList->sPolicyExt.pSN, kExtNameKeyUsage.toStdString().c_str() ) == 0 )
            setKeyUsageUse( &pCurList->sPolicyExt );
        else if( strcasecmp( pCurList->sPolicyExt.pSN, kExtNameNC.toStdString().c_str() ) == 0 )
            setNCUse( &pCurList->sPolicyExt );
        else if( strcasecmp( pCurList->sPolicyExt.pSN, kExtNamePolicy.toStdString().c_str() ) == 0 )
            setPolicyUse( &pCurList->sPolicyExt );
        else if( strcasecmp( pCurList->sPolicyExt.pSN, kExtNamePC.toStdString().c_str() ) == 0 )
            setPCUse( &pCurList->sPolicyExt );
        else if( strcasecmp( pCurList->sPolicyExt.pSN, kExtNamePM.toStdString().c_str() ) == 0 )
            setPMUse( &pCurList->sPolicyExt );
        else if( strcasecmp( pCurList->sPolicyExt.pSN, kExtNameSKI.toStdString().c_str() ) == 0 )
            setSKIUse( &pCurList->sPolicyExt );
        else if( strcasecmp( pCurList->sPolicyExt.pSN, kExtNameSAN.toStdString().c_str() ) == 0 )
            setSANUse( &pCurList->sPolicyExt );

        pCurList = pCurList->pNext;
    }

    JS_DB_resetCertPolicy( &sCertPolicy );
    if( pPolicyExtList ) JS_DB_resetPolicyExtList( &pPolicyExtList );
}

void MakeCertPolicyDlg::defaultPolicy()
{
    int rowCnt = 0;
    mNameText->setText("");


    mAIAText->setText("");

    rowCnt = mAIATable->rowCount();
    for( int i=0; i < rowCnt; i++ )
        mAIATable->removeRow(0);

    mAIAUseCheck->setChecked(false);
    mAIACriticalCheck->setChecked(false);

    mAKIUseCheck->setChecked(false);
    mAKICriticalCheck->setChecked(false);
    mAKICertIssuerCheck->setChecked(false);
    mAKICertSerialCheck->setChecked(false);

    mBCUseCheck->setChecked(false);
    mBCCriticalCheck->setChecked(false);
    mBCPathLenText->setText("");

    rowCnt = mCRLDPTable->rowCount();
    for( int i=0; i < rowCnt; i++ )
        mCRLDPTable->removeRow(0);

    mCRLDPUseCheck->setChecked(false);
    mCRLDPCriticalCheck->setChecked(false);

    mEKUList->clear();
    mEKUUseCheck->setChecked(false);
    mEKUCriticalCheck->setChecked(false);

    mIANUseCheck->setChecked(false);
    mIANCriticalCheck->setChecked(false);
    rowCnt = mIANTable->rowCount();
    for( int i = 0; i < rowCnt; i++ )
        mIANTable->removeRow(0);

    mKeyUsageList->clear();
    mKeyUsageUseCheck->setChecked(false);
    mKeyUsageCriticalCheck->setChecked(false);

    rowCnt = mNCTable->rowCount();
    for( int i=0; i < rowCnt; i++ )
        mNCTable->removeRow(0);
    mNCUseCheck->setChecked(false);
    mNCCriticalCheck->setChecked(false);

    rowCnt = mPolicyTable->rowCount();
    for( int i=0; i < rowCnt; i++ )
        mPolicyTable->removeRow(0);
    mPolicyUseCheck->setChecked(false);
    mPolicyCriticalCheck->setChecked(false);

    mPCUseCheck->setChecked(false);
    mPCCriticalCheck->setChecked(false);
    mPCInhibitText->setText("");
    mPCExplicitText->setText("");

    rowCnt = mPMTable->rowCount();
    for( int i=0; i < rowCnt; i++ )
        mPMTable->removeRow(0);

    mPMUseCheck->setChecked(false);
    mPMCriticalCheck->setChecked(false);

    mSKIUseCheck->setChecked(false);
    mSKICriticalCheck->setChecked(false);

    rowCnt = mSANTable->rowCount();
    for( int i = 0; i < rowCnt; i++ )
        mSANTable->removeRow(0);
    mSANUseCheck->setChecked(false);
    mSANCriticalCheck->setChecked(false);
}

void MakeCertPolicyDlg::accept()
{
    QString strName = mNameText->text();

    if( strName.isEmpty() )
    {
        manApplet->warningBox( tr( "You have to insert name"), this );
        mNameText->setFocus();
        return;
    }

    QString strSubjectDN = mSubjectDNText->text();
    if( strSubjectDN.isEmpty() )
    {
        manApplet->warningBox(tr( "You have to set subjec dn"), this );
        return;
    }

    int nPolicyNum = manApplet->ccClient()->getNum( ITEM_TYPE_CERT_POLICY );

    JCC_CertPolicy sCertPolicy;
    memset( &sCertPolicy, 0x00, sizeof(sCertPolicy));
    int nNotBefore = 0;
    int nNotAfter = 0;

    if( mUseDaysCheck->isChecked() )
    {
        nNotBefore = 0;
        nNotAfter = mDaysText->text().toLong();
    }
    else {
        QDateTime beforeTime;
        QDateTime afterTime;

        beforeTime.setDate( mNotBeforeDateTime->date() );
        afterTime.setDate( mNotAfterDateTime->date() );

        nNotBefore = beforeTime.toTime_t();
        nNotAfter = afterTime.toTime_t();
    }

    JS_DB_setCertPolicy( &sCertPolicy,
                         nPolicyNum,
                         strName.toStdString().c_str(),
                         mVersionCombo->currentIndex(),
                         nNotBefore,
                         nNotAfter,
                         mHashCombo->currentText().toStdString().c_str(),
                         strSubjectDN.toStdString().c_str(),
                         mDivideNumText->text().toInt() );

    if( is_edit_ )
    {
        manApplet->ccClient()->modCertPolicy( policy_num_, &sCertPolicy );
        manApplet->ccClient()->delCertPolicyExts( policy_num_ );
        nPolicyNum = policy_num_;
    }
    else
    {
        manApplet->ccClient()->addCertPolicy( &sCertPolicy );
    }

    /* need to set extend fields here */
    if( mAIAUseCheck->isChecked() ) saveAIAUse( nPolicyNum );
    if( mAKIUseCheck->isChecked() ) saveAKIUse( nPolicyNum );
    if( mBCUseCheck->isChecked() ) saveBCUse( nPolicyNum );
    if( mCRLDPUseCheck->isChecked() ) saveCRLDPUse( nPolicyNum );
    if( mEKUUseCheck->isChecked() ) saveEKUUse( nPolicyNum );
    if( mIANUseCheck->isChecked() ) saveIANUse( nPolicyNum );
    if( mKeyUsageUseCheck->isChecked() ) saveKeyUsageUse( nPolicyNum );
    if( mNCUseCheck->isChecked() ) saveNCUse( nPolicyNum );
    if( mPolicyUseCheck->isChecked() ) savePolicyUse( nPolicyNum );
    if( mPCUseCheck->isChecked() ) savePCUse( nPolicyNum );
    if( mPMUseCheck->isChecked() ) savePMUse( nPolicyNum );
    if( mSKIUseCheck->isChecked() ) saveSKIUse( nPolicyNum );
    if( mSANUseCheck->isChecked() ) saveSANUse( nPolicyNum );
    /* ....... */

    manApplet->mainWindow()->createRightCertPolicyList();
    JS_DB_resetCertPolicy( &sCertPolicy );
    QDialog::accept();
}

void MakeCertPolicyDlg::initUI()
{
    mKeyUsageCombo->addItems(kKeyUsageList);
    mEKUCombo->addItems(kExtKeyUsageList);
    mVersionCombo->addItems(kCertVersionList);
//    mCRLDPCombo->addItems(kTypeList);
    mCRLDPCombo->addItem( "URI" );
    mAIATargetCombo->addItems( kAIATargetList );
    mAIATypeCombo->addItems(kTypeList);
    mSANCombo->addItems(kTypeList);
    mIANCombo->addItems(kTypeList);
    mNCTypeCombo->addItems(kTypeList);
    mNCSubCombo->addItems(kNCSubList);
    mBCCombo->addItems(kBCTypeList);
    mHashCombo->addItems(kHashList);
}

void MakeCertPolicyDlg::setTableMenus()
{
    QStringList sPolicyLabels = { "OID", "CPS", "UserNotice" };
    mPolicyTable->setColumnCount(3);
    mPolicyTable->horizontalHeader()->setStretchLastSection(true);
    mPolicyTable->setHorizontalHeaderLabels( sPolicyLabels );
    mPolicyTable->verticalHeader()->setVisible(false);

    QStringList sCRLDPLabels = { "Type", "Value" };
    mCRLDPTable->setColumnCount(2);
    mCRLDPTable->horizontalHeader()->setStretchLastSection(true);
    mCRLDPTable->setHorizontalHeaderLabels(sCRLDPLabels);
    mCRLDPTable->verticalHeader()->setVisible(false);

    QStringList sAIALabels = { "Target", "Type", "Value" };
    mAIATable->setColumnCount(3);
    mAIATable->horizontalHeader()->setStretchLastSection(true);
    mAIATable->setHorizontalHeaderLabels(sAIALabels);
    mAIATable->verticalHeader()->setVisible(false);

    QStringList sSANLabels = { "Type", "Value" };
    mSANTable->setColumnCount(2);
    mSANTable->horizontalHeader()->setStretchLastSection(true);
    mSANTable->setHorizontalHeaderLabels(sSANLabels);
    mSANTable->verticalHeader()->setVisible(false);

    QStringList sIANLabels = { "Type", "Value" };
    mIANTable->setColumnCount(2);
    mIANTable->horizontalHeader()->setStretchLastSection(true);
    mIANTable->setHorizontalHeaderLabels(sIANLabels);
    mIANTable->verticalHeader()->setVisible(false);

    QStringList sPMLabels = { "Tareg", "Value", "Target", "Value" };
    mPMTable->setColumnCount(4);
    mPMTable->horizontalHeader()->setStretchLastSection(true);
    mPMTable->setHorizontalHeaderLabels(sPMLabels);
    mPMTable->verticalHeader()->setVisible(false);

    QStringList sNCLabels = { "Type", "Target", "Value", "Min", "Max" };
    mNCTable->setColumnCount(5);
    mNCTable->horizontalHeader()->setStretchLastSection(true);
    mNCTable->setHorizontalHeaderLabels(sNCLabels);
    mNCTable->verticalHeader()->setVisible(false);
}

void MakeCertPolicyDlg::connectExtends()
{
    connect( mUseCSRCheck, SIGNAL(clicked()), this, SLOT(clickUseCSR()));
    connect( mUseDaysCheck, SIGNAL(clicked()), this, SLOT(clickUseDays()));

    connect( mAIAUseCheck, SIGNAL(clicked()), this, SLOT(clickAIAUse()));
    connect( mAKIUseCheck, SIGNAL(clicked()), this, SLOT(clickAKIUse()));
    connect( mBCUseCheck, SIGNAL(clicked()), this, SLOT(clickBCUse()));
    connect( mCRLDPUseCheck, SIGNAL(clicked()), this, SLOT(clickCRLDPUse()));
    connect( mEKUUseCheck, SIGNAL(clicked()), this, SLOT(clickEKUUse()));
    connect( mIANUseCheck, SIGNAL(clicked()), this, SLOT(clickIANUse()));
    connect( mKeyUsageUseCheck, SIGNAL(clicked()), this, SLOT(clickKeyUsageUse()));
    connect( mNCUseCheck, SIGNAL(clicked()), this, SLOT( clickNCUse()));
    connect( mPolicyUseCheck, SIGNAL(clicked()), this, SLOT(clickPolicyUse()));
    connect( mPCUseCheck, SIGNAL(clicked()), this, SLOT(clickPCUse()));
    connect( mPMUseCheck, SIGNAL(clicked()), this, SLOT(clickPMUse()));
    connect( mSKIUseCheck, SIGNAL(clicked()), this, SLOT(clickSKIUse()));
    connect( mSANUseCheck, SIGNAL(clicked()), this, SLOT(clickSANUse()));

    connect( mKeyUsageAddBtn, SIGNAL(clicked()), this, SLOT(addKeyUsage()));
    connect( mPolicyAddBtn, SIGNAL(clicked()), this, SLOT(addPolicy()));
    connect( mEKUAddBtn, SIGNAL(clicked()), this, SLOT(addEKU()));
    connect( mCRLDPAddBtn, SIGNAL(clicked()), this, SLOT(addCRLDP()));
    connect( mAIAAddBtn, SIGNAL(clicked()), this, SLOT(addAIA()));
    connect( mSANAddBtn, SIGNAL(clicked()), this, SLOT(addSAN()));
    connect( mIANAddBtn, SIGNAL(clicked()), this, SLOT(addIAN()));
    connect( mPMAddBtn, SIGNAL(clicked()), this, SLOT(addPM()));
    connect( mNCAddBtn, SIGNAL(clicked()), this, SLOT(addNC()));

    connect( mKeyUsageClearBtn, SIGNAL(clicked()), this, SLOT(clearKeyUsage()));
    connect( mPolicyClearBtn, SIGNAL(clicked()), this, SLOT(clearPolicy()));
    connect( mEKUClearBtn, SIGNAL(clicked()), this, SLOT(clearEKU()));
    connect( mCRLDPClearBtn, SIGNAL(clicked()), this, SLOT(clearCRLDP()));
    connect( mAIAClearBtn, SIGNAL(clicked()), this, SLOT(clearAIA()));
    connect( mSANClearBtn, SIGNAL(clicked()), this, SLOT(clearSAN()));
    connect( mIANClearBtn, SIGNAL(clicked()), this, SLOT(clearIAN()));
    connect( mPMClearBtn, SIGNAL(clicked()), this, SLOT(clearPM()));
    connect( mNCClearBtn, SIGNAL(clicked()), this, SLOT(clearNC()));
}

void MakeCertPolicyDlg::clickUseCSR()
{
    bool bStatus = mUseCSRCheck->isChecked();

    mSubjectDNText->setEnabled( !bStatus );

    if( bStatus )
        mSubjectDNText->setText( "#CSR" );
    else {
        mSubjectDNText->setText( "" );
    }
}

void MakeCertPolicyDlg::clickUseDays()
{
    bool bStatus = mUseDaysCheck->isChecked();

    mDaysText->setEnabled(bStatus);
    mNotAfterDateTime->setEnabled(!bStatus);
    mNotBeforeDateTime->setEnabled(!bStatus);
}

void MakeCertPolicyDlg::setExtends()
{
    clickUseCSR();
    clickUseDays();

    clickAIAUse();
    clickAKIUse();
    clickBCUse();
    clickCRLDPUse();
    clickEKUUse();
    clickIANUse();
    clickKeyUsageUse();
    clickNCUse();
    clickPolicyUse();
    clickPCUse();
    clickPMUse();
    clickSKIUse();
    clickSANUse();
}


void MakeCertPolicyDlg::clickAIAUse()
{
    bool bStatus = mAIAUseCheck->isChecked();

    mAIACriticalCheck->setEnabled(bStatus);
    mAIAAddBtn->setEnabled(bStatus);
    mAIATypeCombo->setEnabled(bStatus);
    mAIATargetCombo->setEnabled(bStatus);
    mAIAText->setEnabled(bStatus);
    mAIATable->setEnabled(bStatus);
}

void MakeCertPolicyDlg::clickAKIUse()
{
    bool bStatus = mAKIUseCheck->isChecked();

    mAKICriticalCheck->setEnabled(bStatus);
    mAKICertIssuerCheck->setEnabled(bStatus);
    mAKICertSerialCheck->setEnabled(bStatus);
}

void MakeCertPolicyDlg::clickBCUse()
{
    bool bStatus = mBCUseCheck->isChecked();

    mBCCriticalCheck->setEnabled(bStatus);
    mBCCombo->setEnabled(bStatus);
    mBCPathLenText->setEnabled(bStatus);
}

void MakeCertPolicyDlg::clickCRLDPUse()
{
    bool bStatus = mCRLDPUseCheck->isChecked();

    mCRLDPCriticalCheck->setEnabled( bStatus );
    mCRLDPCombo->setEnabled(bStatus);
    mCRLDPAddBtn->setEnabled(bStatus);
    mCRLDPText->setEnabled(bStatus);
    mCRLDPTable->setEnabled(bStatus);
}

void MakeCertPolicyDlg::clickEKUUse()
{
    bool bStatus = mEKUUseCheck->isChecked();

    mEKUCriticalCheck->setEnabled( bStatus );
    mEKUCombo->setEnabled(bStatus);
    mEKUAddBtn->setEnabled(bStatus);
    mEKUList->setEnabled(bStatus);
}

void MakeCertPolicyDlg::clickIANUse()
{
    bool bStatus = mIANUseCheck->isChecked();

    mIANCriticalCheck->setEnabled(bStatus);
    mIANCombo->setEnabled(bStatus);
    mIANText->setEnabled(bStatus);
    mIANTable->setEnabled(bStatus);
    mIANAddBtn->setEnabled(bStatus);
}

void MakeCertPolicyDlg::clickKeyUsageUse()
{
    bool bStatus = mKeyUsageUseCheck->isChecked();

    mKeyUsageCriticalCheck->setEnabled( bStatus );
    mKeyUsageCombo->setEnabled( bStatus );
    mKeyUsageAddBtn->setEnabled( bStatus );
    mKeyUsageList->setEnabled( bStatus );
}

void MakeCertPolicyDlg::clickNCUse()
{
    bool bStatus = mNCUseCheck->isChecked();

    mNCCriticalCheck->setEnabled(bStatus);
    mNCSubCombo->setEnabled(bStatus);
    mNCAddBtn->setEnabled(bStatus);
    mNCTypeCombo->setEnabled(bStatus);
    mNCSubText->setEnabled(bStatus);
    mNCMaxText->setEnabled(bStatus);
    mNCMinText->setEnabled(bStatus);
    mNCTable->setEnabled(bStatus);
}

void MakeCertPolicyDlg::clickPolicyUse()
{
    bool bStatus = mPolicyUseCheck->isChecked();

    mPolicyCriticalCheck->setEnabled( bStatus );
    mPolicyAddBtn->setEnabled( bStatus );
    mPolicyOIDText->setEnabled( bStatus );
    mPolicyCPSText->setEnabled( bStatus );
    mPolicyUserNoticeText->setEnabled( bStatus );
    mPolicyTable->setEnabled( bStatus );
}

void MakeCertPolicyDlg::clickPCUse()
{
    bool bStatus = mPCUseCheck->isChecked();

    mPCCriticalCheck->setEnabled(bStatus);
    mPCInhibitText->setEnabled(bStatus);
    mPCExplicitText->setEnabled(bStatus);
}

void MakeCertPolicyDlg::clickPMUse()
{
    bool bStatus = mPMUseCheck->isChecked();

    mPMCriticalCheck->setEnabled(bStatus);
    mPMAddBtn->setEnabled(bStatus);
    mPMIssuerDomainPolicyText->setEnabled(bStatus);
    mPMSubjectDomainPolicyText->setEnabled(bStatus);
    mPMTable->setEnabled(bStatus);
}

void MakeCertPolicyDlg::clickSKIUse()
{
    bool bStatus = mSKIUseCheck->isChecked();

    mSKICriticalCheck->setEnabled( bStatus );
}

void MakeCertPolicyDlg::clickSANUse()
{
    bool bStatus = mSANUseCheck->isChecked();

    mSANCriticalCheck->setEnabled(bStatus);
    mSANCombo->setEnabled(bStatus);
    mSANAddBtn->setEnabled(bStatus);
    mSANText->setEnabled(bStatus);
    mSANTable->setEnabled(bStatus);
}


void MakeCertPolicyDlg::addKeyUsage()
{
    QString strVal = mKeyUsageCombo->currentText();

    mKeyUsageList->addItem( strVal );
}

void MakeCertPolicyDlg::addPolicy()
{
    QString strOID = mPolicyOIDText->text();
    QString strCPS = mPolicyCPSText->text();
    QString strUserNotice = mPolicyUserNoticeText->text();

    int row = mPolicyTable->rowCount();

    mPolicyTable->setRowCount( row + 1 );

    mPolicyTable->setItem( row, 0, new QTableWidgetItem(strOID));
    mPolicyTable->setItem( row, 1, new QTableWidgetItem(strCPS));
    mPolicyTable->setItem( row, 2, new QTableWidgetItem(strUserNotice));
}

void MakeCertPolicyDlg::addEKU()
{
    QString strVal = mEKUCombo->currentText();

    mEKUList->addItem( strVal );
}

void MakeCertPolicyDlg::addCRLDP()
{
    QString strType = mCRLDPCombo->currentText();
    QString strVal = mCRLDPText->text();

    int row = mCRLDPTable->rowCount();
    mCRLDPTable->setRowCount( row + 1 );

    mCRLDPTable->setItem( row, 0, new QTableWidgetItem( strType ));
    mCRLDPTable->setItem( row, 1, new QTableWidgetItem( strVal ));
}

void MakeCertPolicyDlg::addAIA()
{
    QString strTarget = mAIATargetCombo->currentText();
    QString strType = mAIATypeCombo->currentText();
    QString strVal = mAIAText->text();

    int row = mAIATable->rowCount();

    mAIATable->setRowCount( row + 1 );

    mAIATable->setItem( row, 0, new QTableWidgetItem( strTarget ));
    mAIATable->setItem( row, 1, new QTableWidgetItem( strType) );
    mAIATable->setItem( row, 2, new QTableWidgetItem( strVal ));
}

void MakeCertPolicyDlg::addSAN()
{
    QString strType = mSANCombo->currentText();
    QString strVal = mSANText->text();

    int row = mSANTable->rowCount();
    mSANTable->setRowCount( row + 1 );

    mSANTable->setItem( row, 0, new QTableWidgetItem(strType));
    mSANTable->setItem( row, 1, new QTableWidgetItem(strVal));
}

void MakeCertPolicyDlg::addIAN()
{
    QString strType = mIANCombo->currentText();
    QString strVal = mIANText->text();

    int row = mIANTable->rowCount();
    mIANTable->setRowCount( row + 1 );

    mIANTable->setItem( row, 0, new QTableWidgetItem(strType));
    mIANTable->setItem( row, 1, new QTableWidgetItem(strVal));
}

void MakeCertPolicyDlg::addPM()
{
    QString strIDP = mPMIssuerDomainPolicyText->text();
    QString strSDP = mPMSubjectDomainPolicyText->text();

    int row = mPMTable->rowCount();
    mPMTable->setRowCount( row + 1 );

    mPMTable->setItem( row, 0, new QTableWidgetItem( "IssuerDomainPolicy"));
    mPMTable->setItem( row, 1, new QTableWidgetItem( strIDP));
    mPMTable->setItem( row, 2, new QTableWidgetItem( "SubjectDomainPolicy"));
    mPMTable->setItem( row, 3, new QTableWidgetItem( strSDP));
}

void MakeCertPolicyDlg::addNC()
{
    QString strType = mNCTypeCombo->currentText();
    QString strSubType = mNCSubCombo->currentText();
    QString strVal = mNCSubText->text();
    QString strMax = mNCMaxText->text();
    QString strMin = mNCMinText->text();

    int row = mNCTable->rowCount();
    mNCTable->setRowCount( row + 1 );

    mNCTable->setItem( row, 0, new QTableWidgetItem(strType));
    mNCTable->setItem( row, 1, new QTableWidgetItem(strSubType));
    mNCTable->setItem( row, 2, new QTableWidgetItem(strVal));
    mNCTable->setItem( row, 3, new QTableWidgetItem(strMax));
    mNCTable->setItem( row, 4, new QTableWidgetItem(strMin));
}

void MakeCertPolicyDlg::clearKeyUsage()
{
    mKeyUsageList->clear();
}

void MakeCertPolicyDlg::clearPolicy()
{
    int nCnt = mPolicyTable->rowCount();

    for( int i=0; i < nCnt; i++ )
        mPolicyTable->removeRow(0);
}

void MakeCertPolicyDlg::clearEKU()
{
    mEKUList->clear();
}

void MakeCertPolicyDlg::clearCRLDP()
{
    int nCnt = mCRLDPTable->rowCount();

    for( int i=0; i < nCnt; i++)
        mCRLDPTable->removeRow(0);
}

void MakeCertPolicyDlg::clearAIA()
{
    int nCnt = mAIATable->rowCount();

    for( int i=0; i < nCnt; i++)
        mAIATable->removeRow(0);
}

void MakeCertPolicyDlg::clearSAN()
{
    int nCnt = mSANTable->rowCount();

    for( int i=0; i < nCnt; i++)
        mSANTable->removeRow(0);
}

void MakeCertPolicyDlg::clearIAN()
{
    int nCnt = mIANTable->rowCount();

    for( int i=0; i < nCnt; i++)
        mIANTable->removeRow(0);
}

void MakeCertPolicyDlg::clearPM()
{
    int nCnt = mPMTable->rowCount();

    for( int i=0; i < nCnt; i++)
        mPMTable->removeRow(0);
}

void MakeCertPolicyDlg::clearNC()
{
    int nCnt = mNCTable->rowCount();

    for( int i=0; i < nCnt; i++)
        mNCTable->removeRow(0);
}

void MakeCertPolicyDlg::saveAIAUse(int nPolicyNum )
{
    JCC_PolicyExt   sPolicyExt;
    memset( &sPolicyExt, 0x00, sizeof(sPolicyExt));

    QString strVal = "";

    for( int i=0; i < mAIATable->rowCount(); i++ )
    {
        QString strMethod;
        QString strType;
        QString strData;

        strMethod = mAIATable->takeItem( i, 0 )->text();
        strType = mAIATable->takeItem( i, 1)->text();
        strData = mAIATable->takeItem( i, 2 )->text();

        if( i != 0 ) strVal += "#";
        strVal += strMethod;
        strVal += "$";
        strVal += strType;
        strVal += "$";
        strVal += strData;
    }

    JS_DB_setPolicyExt( &sPolicyExt, -1, nPolicyNum, mAIACriticalCheck->isChecked(), "authorityInfoAccess", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCertPolicyExt( nPolicyNum, &sPolicyExt );
    JS_DB_resetPolicyExt( &sPolicyExt );
}

void MakeCertPolicyDlg::saveAKIUse(int nPolicyNum )
{
    JCC_PolicyExt   sPolicyExt;
    memset( &sPolicyExt, 0x00, sizeof(sPolicyExt));

    QString strVal;

    if( mAKICertIssuerCheck->isChecked() ) strVal += "ISSUER#";
    if( mAKICertSerialCheck->isChecked() ) strVal += "SERIAL#";

    JS_DB_setPolicyExt( &sPolicyExt, -1, nPolicyNum, mAKICriticalCheck->isChecked(), "authorityKeyIdentifier", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCertPolicyExt( nPolicyNum, &sPolicyExt );
    JS_DB_resetPolicyExt( &sPolicyExt );
}

void MakeCertPolicyDlg::saveBCUse(int nPolicyNum )
{
    JCC_PolicyExt   sPolicyExt;
    memset( &sPolicyExt, 0x00, sizeof(sPolicyExt));

    QString strVal;

    if( mBCCombo->currentIndex() == 0 )
        strVal = "CA";
    else {
        strVal = "EE";
    }

    if( mBCPathLenText->text().length() > 0 )
    {
        strVal += "#";
        strVal += mBCPathLenText->text();
    }

    JS_DB_setPolicyExt( &sPolicyExt, -1, nPolicyNum, mBCCriticalCheck->isChecked(), "basicConstraints", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCertPolicyExt( nPolicyNum, &sPolicyExt );
    JS_DB_resetPolicyExt( &sPolicyExt );
}

void MakeCertPolicyDlg::saveCRLDPUse(int nPolicyNum )
{
    JCC_PolicyExt   sPolicyExt;
    memset( &sPolicyExt, 0x00, sizeof(sPolicyExt));

    QString strVal = "";

    for( int i=0; i < mCRLDPTable->rowCount(); i++ )
    {
        QString strType = "";
        QString strData = "";

        strType = mCRLDPTable->takeItem( i, 0 )->text();
        strData = mCRLDPTable->takeItem( i, 1 )->text();

        if( i != 0 ) strVal += "#";
        strVal += strType;
        strVal += "$";
        strVal += strData;
    }

    JS_DB_setPolicyExt( &sPolicyExt, -1, nPolicyNum, mCRLDPCriticalCheck->isChecked(), "crlDistributionPoints", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCertPolicyExt( nPolicyNum, &sPolicyExt );
    JS_DB_resetPolicyExt( &sPolicyExt );
}

void MakeCertPolicyDlg::saveEKUUse(int nPolicyNum )
{
    JCC_PolicyExt   sPolicyExt;
    memset( &sPolicyExt, 0x00, sizeof(sPolicyExt));

    QString strVal = "";

    for( int i=0; i < mEKUList->count(); i++ )
    {
        if( i != 0 ) strVal += "#";
        strVal += mEKUList->item(i)->text();
    }

    JS_DB_setPolicyExt( &sPolicyExt, -1, nPolicyNum, mEKUCriticalCheck->isChecked(), "extendedKeyUsage", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCertPolicyExt( nPolicyNum, &sPolicyExt );
    JS_DB_resetPolicyExt( &sPolicyExt );
}

void MakeCertPolicyDlg::saveIANUse(int nPolicyNum )
{
    JCC_PolicyExt   sPolicyExt;
    memset( &sPolicyExt, 0x00, sizeof(sPolicyExt));

    QString strVal;

    for( int i=0; i < mIANTable->rowCount(); i++ )
    {
        QString strType;
        QString strData;

        strType = mIANTable->takeItem( i, 0)->text();
        strData = mIANTable->takeItem( i, 1)->text();

        if( i != 0 ) strVal += "#";
        strVal += strType;
        strVal += "$";
        strVal += strData;
    }

    JS_DB_setPolicyExt( &sPolicyExt, -1, nPolicyNum, mIANCriticalCheck->isChecked(), "issuerAltName", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCertPolicyExt( nPolicyNum, &sPolicyExt );
    JS_DB_resetPolicyExt( &sPolicyExt );
}

void MakeCertPolicyDlg::saveKeyUsageUse(int nPolicyNum )
{
    JCC_PolicyExt   sPolicyExt;
    memset( &sPolicyExt, 0x00, sizeof(sPolicyExt));

    QString strValue;

    for( int i =0; i < mKeyUsageList->count(); i++ )
    {
        if( i != 0 ) strValue += "#";
        strValue += mKeyUsageList->item(i)->text();
    }

    JS_DB_setPolicyExt( &sPolicyExt, -1, nPolicyNum, mKeyUsageCriticalCheck->isChecked(), "keyUsage", strValue.toStdString().c_str() );
    manApplet->ccClient()->addCertPolicyExt( nPolicyNum, &sPolicyExt );
    JS_DB_resetPolicyExt( &sPolicyExt );
}

void MakeCertPolicyDlg::saveNCUse(int nPolicyNum )
{
    JCC_PolicyExt   sPolicyExt;
    memset( &sPolicyExt, 0x00, sizeof(sPolicyExt));

    QString strVal = "";

    for( int i=0; i < mNCTable->rowCount(); i++ )
    {
        QString strType;
        QString strKind;
        QString strData;
        QString strMin;
        QString strMax;

        strType = mNCTable->takeItem( i, 0 )->text();
        strKind = mNCTable->takeItem(i, 1)->text();
        strData = mNCTable->takeItem(i, 2)->text();
        strMin = mNCTable->takeItem(i,3)->text();
        strMax = mNCTable->takeItem(i,4)->text();

        if( i != 0 ) strVal += "#";
        strVal += strType;
        strVal += "$";
        strVal += strKind;
        strVal += "$";
        strVal += strData;
        strVal += "$";
        strVal += strMin;
        strVal += "$";
        strVal += strMax;
    }

    JS_DB_setPolicyExt( &sPolicyExt, -1, nPolicyNum, mNCCriticalCheck->isChecked(), "nameConstraints", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCertPolicyExt( nPolicyNum, &sPolicyExt );
    JS_DB_resetPolicyExt( &sPolicyExt );
}

void MakeCertPolicyDlg::savePolicyUse(int nPolicyNum )
{
    JCC_PolicyExt   sPolicyExt;
    memset( &sPolicyExt, 0x00, sizeof(sPolicyExt));

    QString strVal;

    for( int i=0; i < mPolicyTable->rowCount(); i++ )
    {
        if( i != 0 ) strVal += "%%";

        strVal += "OID$";
        strVal += mPolicyTable->takeItem(i,0)->text();
        strVal += "#CPS$";
        strVal += mPolicyTable->takeItem(i,1)->text();
        strVal += "#UserNotice$";
        strVal += mPolicyTable->takeItem(i,2)->text();
        strVal += "#";
    }

    JS_DB_setPolicyExt( &sPolicyExt, -1, nPolicyNum, mPolicyCriticalCheck->isChecked(), "certificatePolicies", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCertPolicyExt( nPolicyNum, &sPolicyExt );
    JS_DB_resetPolicyExt( &sPolicyExt );
}

void MakeCertPolicyDlg::savePCUse(int nPolicyNum )
{
    JCC_PolicyExt   sPolicyExt;
    memset( &sPolicyExt, 0x00, sizeof(sPolicyExt));

    QString strVal;
    strVal = "REP";

    if( mPCExplicitText->text().length() > 0 )
    {
        strVal += "$";
        strVal += mPCExplicitText->text();
    }

    strVal += "#IPM";
    if( mPCInhibitText->text().length() > 0 )
    {
        strVal += "$";
        strVal += mPCInhibitText->text();
    }

    JS_DB_setPolicyExt( &sPolicyExt, -1, nPolicyNum, mPCCriticalCheck->isChecked(), "policyConstraints", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCertPolicyExt( nPolicyNum, &sPolicyExt );
    JS_DB_resetPolicyExt( &sPolicyExt );
}

void MakeCertPolicyDlg::savePMUse(int nPolicyNum )
{
    JCC_PolicyExt   sPolicyExt;
    memset( &sPolicyExt, 0x00, sizeof(sPolicyExt));

    QString strVal;

    for( int i=0; i < mPMTable->rowCount(); i++ )
    {
        QString strIDP;
        QString strSDP;

        strIDP = mPMTable->takeItem(i, 1)->text();
        strSDP = mPMTable->takeItem(i, 3)->text();

        if( i != 0 ) strVal += "#";
        strVal += strIDP;
        strVal += "$";
        strVal += strSDP;
    }

    JS_DB_setPolicyExt( &sPolicyExt, -1, nPolicyNum, mPMCriticalCheck->isChecked(), "policyMappings", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCertPolicyExt( nPolicyNum, &sPolicyExt );
    JS_DB_resetPolicyExt( &sPolicyExt );
}

void MakeCertPolicyDlg::saveSKIUse(int nPolicyNum )
{
    JCC_PolicyExt   sPolicyExt;
    memset( &sPolicyExt, 0x00, sizeof(sPolicyExt));

    JS_DB_setPolicyExt( &sPolicyExt, -1, nPolicyNum, mSKICriticalCheck->isChecked(), "subjectKeyIdentifier", NULL );
    manApplet->ccClient()->addCertPolicyExt( nPolicyNum, &sPolicyExt );
    JS_DB_resetPolicyExt( &sPolicyExt );
}

void MakeCertPolicyDlg::saveSANUse(int nPolicyNum)
{
    JCC_PolicyExt   sPolicyExt;
    memset( &sPolicyExt, 0x00, sizeof(sPolicyExt));

    QString strVal = "";
    for( int i=0; i < mSANTable->rowCount(); i++ )
    {
        QString strType;
        QString strData;

        strType = mSANTable->takeItem( i, 0 )->text();
        strData = mSANTable->takeItem( i, 1 )->text();

        if( i != 0 ) strVal += "#";
        strVal += strType;
        strVal += "$";
        strVal += strData;
    }

    JS_DB_setPolicyExt( &sPolicyExt, -1, nPolicyNum, mSANCriticalCheck->isChecked(), "subjectAltName", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCertPolicyExt( nPolicyNum, &sPolicyExt );
    JS_DB_resetPolicyExt( &sPolicyExt );
}

void MakeCertPolicyDlg::setAIAUse( JCC_PolicyExt *pPolicyExt )
{
    mAIAUseCheck->setChecked(true);
    mAIACriticalCheck->setChecked( pPolicyExt->bCritical );
    clickAIAUse();

    QString strVal = pPolicyExt->pValue;

    QStringList valList = strVal.split("#");

    for( int i=0; i < valList.size(); i++ )
    {
        QString info = valList.at(i);
        QString strMethod = "";
        QString strType = "";
        QString strData = "";

        QStringList infoList = info.split("$");
        strMethod = infoList.at(0);
        strType = infoList.at(1);
        strData = infoList.at(2);

        mAIATable->insertRow(i);
        mAIATable->setItem( i, 0, new QTableWidgetItem(strMethod));
        mAIATable->setItem( i, 1, new QTableWidgetItem(strType));
        mAIATable->setItem( i, 2, new QTableWidgetItem(strData));
    }
}

void MakeCertPolicyDlg::setAKIUse( JCC_PolicyExt *pPolicyExt )
{
    mAKIUseCheck->setChecked(true);
    mAKICriticalCheck->setChecked( pPolicyExt->bCritical );
    clickAKIUse();

    QString strVal = pPolicyExt->pValue;

    bool bStatus = strVal.contains("ISSUER");
    mAKICertIssuerCheck->setChecked(bStatus);

    bStatus = strVal.contains("SERIAL");
    mAKICertSerialCheck->setChecked(bStatus);
}

void MakeCertPolicyDlg::setBCUse( JCC_PolicyExt *pPolicyExt )
{
    mBCUseCheck->setChecked(true);
    mBCCriticalCheck->setChecked( pPolicyExt->bCritical );
    clickBCUse();

    QString strVal = pPolicyExt->pValue;

    QStringList valList = strVal.split("#");
    QString strType= valList.at(0);
    QString strLen = valList.at(1);

    mBCCombo->setCurrentText( strType );
    mBCPathLenText->setText( strLen );
}

void MakeCertPolicyDlg::setCRLDPUse( JCC_PolicyExt *pPolicyExt )
{
    mCRLDPUseCheck->setChecked(true);
    mCRLDPCriticalCheck->setChecked( pPolicyExt->bCritical );
    clickCRLDPUse();

    QString strVal = pPolicyExt->pValue;

    QStringList valList = strVal.split("#");

    for( int i=0; i < valList.size(); i++ )
    {
        QString info = valList.at(i);
        QStringList typeData = info.split("$");

        QString strType = typeData.at(0);
        QString strData = typeData.at(1);

        mCRLDPTable->insertRow(i);
        mCRLDPTable->setItem( i, 0, new QTableWidgetItem(strType));
        mCRLDPTable->setItem( i, 1, new QTableWidgetItem(strData));
    }
}

void MakeCertPolicyDlg::setEKUUse( JCC_PolicyExt *pPolicyExt )
{
    QString strVal = "";

    mEKUUseCheck->setChecked(true);
    mEKUCriticalCheck->setChecked( pPolicyExt->bCritical );
    clickEKUUse();

    strVal = pPolicyExt->pValue;
    QStringList valList = strVal.split("#");

    if( valList.size() > 0 ) mEKUList->insertItems( 0, valList );
}

void MakeCertPolicyDlg::setIANUse( JCC_PolicyExt *pPolicyExt )
{
    mIANUseCheck->setChecked(true);
    mIANCriticalCheck->setChecked( pPolicyExt->bCritical );
    clickIANUse();

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
        mIANTable->setItem(i, 1, new QTableWidgetItem(strData));
    }
}

void MakeCertPolicyDlg::setKeyUsageUse( JCC_PolicyExt *pPolicyExt )
{
    mKeyUsageUseCheck->setChecked(true);
    mKeyUsageCriticalCheck->setChecked( pPolicyExt->bCritical );
    clickKeyUsageUse();

    QString strVal = pPolicyExt->pValue;

    mKeyUsageList->clear();

    QStringList valList = strVal.split("#");
    if( valList.size() > 0 ) mKeyUsageList->insertItems(0, valList );
}

void MakeCertPolicyDlg::setNCUse( JCC_PolicyExt *pPolicyExt )
{
    mNCUseCheck->setChecked(true);
    mNCCriticalCheck->setChecked( pPolicyExt->bCritical );
    clickNCUse();

    QString strVal = pPolicyExt->pValue;

    QStringList valList = strVal.split("#");

    for( int i=0; i < valList.size(); i++ )
    {
        QString info = valList.at(i);
        QStringList infoList = info.split("$");

        QString strType = infoList.at(0);
        QString strKind = infoList.at(1);
        QString strData = infoList.at(2);
        QString strMin;
        QString strMax;

        if( infoList.size() > 3 ) strMin = infoList.at(3);
        if( infoList.size() > 4 ) strMax = infoList.at(4);

        mNCTable->insertRow(i);
        mNCTable->setItem(i, 0, new QTableWidgetItem(strType));
        mNCTable->setItem(i, 1, new QTableWidgetItem(strKind));
        mNCTable->setItem(i, 2, new QTableWidgetItem(strData));
        mNCTable->setItem(i, 3, new QTableWidgetItem(strMin));
        mNCTable->setItem(i, 4, new QTableWidgetItem(strMax));
    }
}

void MakeCertPolicyDlg::setPolicyUse( JCC_PolicyExt *pPolicyExt )
{
    mPolicyUseCheck->setChecked(true);
    mPolicyCriticalCheck->setChecked( pPolicyExt->bCritical );
    clickPolicyUse();

    QString strVal = pPolicyExt->pValue;

    QStringList valList = strVal.split("%%");

    for( int i=0; i < valList.size(); i++ )
    {
        QString strInfo = valList.at(i);
        QStringList infoList = strInfo.split("#");
        QString strOID = "";
        QString strCPS = "";
        QString strUserNotice = "";

        for( int k = 0; k < infoList.size(); k++ )
        {
            QString info = infoList.at(k);
            QStringList typeData = info.split("$");

            if( typeData.size() < 2 ) continue;

            QString strType = typeData.at(0);
            QString strData = typeData.at(1);

            if( strType == "OID" )
                strOID = strData;
            else if( strType == "CPS" )
                strCPS = strData;
            else if( strType == "UserNotice" )
                strUserNotice = strData;
        }

        int row = mPolicyTable->rowCount();

        mPolicyTable->setRowCount( row + 1 );

        mPolicyTable->setItem( row, 0, new QTableWidgetItem(strOID));
        mPolicyTable->setItem( row, 1, new QTableWidgetItem(strCPS));
        mPolicyTable->setItem( row, 2, new QTableWidgetItem(strUserNotice));
    }
}

void MakeCertPolicyDlg::setPCUse( JCC_PolicyExt *pPolicyExt )
{
    mPCUseCheck->setChecked(true);
    mPCCriticalCheck->setChecked( pPolicyExt->bCritical );
    clickPCUse();

    QString strVal = pPolicyExt->pValue;
    QStringList valList = strVal.split("#");

    for( int i=0; i < valList.size(); i++ )
    {
        QString info = valList.at(i);
        QStringList infoList = info.split("$");

        QString strType = infoList.at(0);
        QString strData = infoList.at(1);

        if( strType == "REP" )
            mPCExplicitText->setText( strData );
        else if( strType == "IPM" )
            mPCInhibitText->setText( strData );
    }
}

void MakeCertPolicyDlg::setPMUse( JCC_PolicyExt *pPolicyExt )
{
    mPMUseCheck->setChecked(true);
    mPMCriticalCheck->setChecked( pPolicyExt->bCritical );
    clickPMUse();

    QString strVal = pPolicyExt->pValue;

    QStringList valList = strVal.split("#");

    for( int i=0; i < valList.size(); i++ )
    {
        QString info = valList.at(i);
        QStringList infoList = info.split("$");

        QString strIDP = infoList.at(0);
        QString strSDP = infoList.at(1);

        mPMTable->insertRow(i);
        mPMTable->setItem(i,0,new QTableWidgetItem("issuerDomainPolicy"));
        mPMTable->setItem(i,1,new QTableWidgetItem(strIDP));
        mPMTable->setItem(i,2,new QTableWidgetItem("subjectDomainPolicy"));
        mPMTable->setItem(i,3,new QTableWidgetItem(strSDP));
    }
}

void MakeCertPolicyDlg::setSKIUse( JCC_PolicyExt *pPolicyExt )
{
    mSKIUseCheck->setChecked(true);
    mSKICriticalCheck->setChecked( pPolicyExt->bCritical );
    clickSKIUse();
}

void MakeCertPolicyDlg::setSANUse( JCC_PolicyExt *pPolicyExt )
{
    mSANUseCheck->setChecked(true);
    mSANCriticalCheck->setChecked( pPolicyExt->bCritical );
    clickSANUse();

    QString strVal = pPolicyExt->pValue;

    QStringList valList = strVal.split("#");

    for( int i=0; i < valList.size(); i++ )
    {
        QString info = valList.at(i);
        QStringList infoList = info.split("$");

        QString strType = infoList.at(0);
        QString strData = infoList.at(1);

        mSANTable->insertRow(i);
        mSANTable->setItem( i, 0, new QTableWidgetItem(strType));
        mSANTable->setItem(i, 1, new QTableWidgetItem(strData));
    }
}

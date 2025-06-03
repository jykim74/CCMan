#include <QMenu>

#include "make_cert_profile_dlg.h"
#include "mainwindow.h"
#include "man_applet.h"
#include "common.h"
#include "cc_client.h"
#include "js_db.h"
#include "js_db_data.h"
#include "man_tree_item.h"

static QStringList kPeriodTypes = { "Day", "Month", "Year" };


MakeCertProfileDlg::MakeCertProfileDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    initUI();
    connectExtends();
    setExtends();
    setTableMenus();

    is_edit_ = false;
    profile_num_ = -1;

    initialize();

#if defined(Q_OS_MAC)
    layout()->setSpacing(5);

    mBaseTab->layout()->setSpacing(5);
    mBaseTab->layout()->setMargin(5);

    mExtend1Tab->layout()->setSpacing(5);
    mExtend1Tab->layout()->setMargin(5);

    mExtend2Tab->layout()->setSpacing(5);
    mExtend2Tab->layout()->setMargin(5);

    mExtend3Tab->layout()->setSpacing(5);
    mExtend3Tab->layout()->setMargin(5);

    mExtend4Tab->layout()->setSpacing(5);
    mExtend4Tab->layout()->setMargin(5);

    mExtensionsGroup->layout()->setSpacing(5);
    mExtensionsGroup->layout()->setSpacing(5);

    mKeyUsageGroup->layout()->setSpacing(5);
    mKeyUsageGroup->layout()->setMargin(5);

    mPolicyGroup->layout()->setSpacing(5);
    mPolicyGroup->layout()->setMargin(5);

    mSKIGroup->layout()->setSpacing(5);
    mSKIGroup->layout()->setMargin(5);

    mAKIGroup->layout()->setSpacing(5);
    mAKIGroup->layout()->setMargin(5);

    mEKUGroup->layout()->setSpacing(5);
    mEKUGroup->layout()->setMargin(5);

    mCRLDPGroup->layout()->setSpacing(5);
    mCRLDPGroup->layout()->setMargin(5);

    mAIAGroup->layout()->setSpacing(5);
    mAIAGroup->layout()->setMargin(5);

    mBCGroup->layout()->setSpacing(5);
    mBCGroup->layout()->setMargin(5);

    mSANGroup->layout()->setSpacing(5);
    mSANGroup->layout()->setMargin(5);

    mIANGroup->layout()->setSpacing(5);
    mIANGroup->layout()->setMargin(5);

    mPCGroup->layout()->setSpacing(5);
    mPCGroup->layout()->setMargin(5);

    mPMGroup->layout()->setSpacing(5);
    mPMGroup->layout()->setMargin(5);

    mNCGroup->layout()->setSpacing(5);
    mNCGroup->layout()->setMargin(5);
#endif
    resize(minimumSizeHint().width(), minimumSizeHint().height());
}

MakeCertProfileDlg::~MakeCertProfileDlg()
{

}

void MakeCertProfileDlg::setEdit( int nProfileNum )
{
    is_edit_ = true;
    profile_num_ = nProfileNum;
    loadProfile( profile_num_ );
}

void MakeCertProfileDlg::initialize()
{
    mCertTab->setCurrentIndex(0);

    defaultProfile();
}

void MakeCertProfileDlg::loadProfile( int nProfileNum, bool bCopy )
{
    JCC_CertProfile      sCertProfile;
    memset( &sCertProfile, 0x00, sizeof(sCertProfile));

    QDateTime notBefore;
    QDateTime notAfter;

    manApplet->ccClient()->getCertProfile( nProfileNum, &sCertProfile );
    QString strName = sCertProfile.pName;

    if( bCopy == true )
        mNameText->setText( strName + "_Copy" );
    else
        mNameText->setText( strName );

    mVersionCombo->setCurrentIndex( sCertProfile.nVersion );
    mHashCombo->setCurrentText( sCertProfile.pHash );
    mSubjectDNText->setText( sCertProfile.pDNTemplate );
    mExtUsageCombo->setCurrentIndex( sCertProfile.nExtUsage );

    if( sCertProfile.nNotBefore >= 0 && sCertProfile.nNotBefore <= 2 )
    {
        mUseDaysCheck->setChecked(true);
        mDaysTypeCombo->setCurrentIndex( sCertProfile.nNotBefore );
        mDaysText->setText( QString("%1").arg(sCertProfile.nNotAfter));
    }
    else {
        mUseDaysCheck->setChecked(false);

        notBefore.setTime_t( sCertProfile.nNotBefore );
        notAfter.setTime_t( sCertProfile.nNotAfter );

        mNotBeforeDateTime->setDateTime( notBefore );
        mNotAfterDateTime->setDateTime( notAfter );
    }

    clickUseDays();

    JCC_ProfileExtList   *pProfileExtList = NULL;
    JCC_ProfileExtList   *pCurList = NULL;

    manApplet->ccClient()->getCertProfileExtList( nProfileNum, &pProfileExtList );
    pCurList = pProfileExtList;

    while( pCurList )
    {
        if( strcasecmp( pCurList->sProfileExt.pSN, kExtNameAIA.toStdString().c_str() ) == 0 )
            setAIAUse( &pCurList->sProfileExt );
        else if( strcasecmp( pCurList->sProfileExt.pSN, kExtNameAKI.toStdString().c_str() ) == 0 )
            setAKIUse( &pCurList->sProfileExt );
        else if( strcasecmp( pCurList->sProfileExt.pSN, kExtNameBC.toStdString().c_str() ) == 0 )
            setBCUse( &pCurList->sProfileExt );
        else if( strcasecmp( pCurList->sProfileExt.pSN, kExtNameCRLDP.toStdString().c_str() ) == 0 )
            setCRLDPUse( &pCurList->sProfileExt );
        else if( strcasecmp( pCurList->sProfileExt.pSN, kExtNameEKU.toStdString().c_str() ) == 0 )
            setEKUUse( &pCurList->sProfileExt );
        else if( strcasecmp( pCurList->sProfileExt.pSN, kExtNameIAN.toStdString().c_str() ) == 0 )
            setIANUse( &pCurList->sProfileExt );
        else if( strcasecmp( pCurList->sProfileExt.pSN, kExtNameKeyUsage.toStdString().c_str() ) == 0 )
            setKeyUsageUse( &pCurList->sProfileExt );
        else if( strcasecmp( pCurList->sProfileExt.pSN, kExtNameNC.toStdString().c_str() ) == 0 )
            setNCUse( &pCurList->sProfileExt );
        else if( strcasecmp( pCurList->sProfileExt.pSN, kExtNamePolicy.toStdString().c_str() ) == 0 )
            setPolicyUse( &pCurList->sProfileExt );
        else if( strcasecmp( pCurList->sProfileExt.pSN, kExtNamePC.toStdString().c_str() ) == 0 )
            setPCUse( &pCurList->sProfileExt );
        else if( strcasecmp( pCurList->sProfileExt.pSN, kExtNamePM.toStdString().c_str() ) == 0 )
            setPMUse( &pCurList->sProfileExt );
        else if( strcasecmp( pCurList->sProfileExt.pSN, kExtNameSKI.toStdString().c_str() ) == 0 )
            setSKIUse( &pCurList->sProfileExt );
        else if( strcasecmp( pCurList->sProfileExt.pSN, kExtNameSAN.toStdString().c_str() ) == 0 )
            setSANUse( &pCurList->sProfileExt );
        else
            setExtensionsUse( &pCurList->sProfileExt );

        pCurList = pCurList->pNext;
    }

    JS_DB_resetCertProfile( &sCertProfile );
    if( pProfileExtList ) JS_DB_resetProfileExtList( &pProfileExtList );
}

void MakeCertProfileDlg::defaultProfile()
{
    int rowCnt = 0;
    mNameText->setText("");

    mVersionCombo->setCurrentIndex(2);
    mHashCombo->setCurrentIndex(2);

    mAIAText->setText("");

    rowCnt = mAIATable->rowCount();
    for( int i=0; i < rowCnt; i++ )
        mAIATable->removeRow(0);

    mAIAGroup->setChecked(false);
    mAIACriticalCheck->setChecked(false);

    mAKIGroup->setChecked(false);
    mAKICriticalCheck->setChecked(false);
    mAKICertIssuerCheck->setChecked(false);
    mAKICertSerialCheck->setChecked(false);

    mBCGroup->setChecked(false);
    mBCCriticalCheck->setChecked(false);
    mBCPathLenText->setText("");

    rowCnt = mCRLDPTable->rowCount();
    for( int i=0; i < rowCnt; i++ )
        mCRLDPTable->removeRow(0);

    mCRLDPGroup->setChecked(false);
    mCRLDPCriticalCheck->setChecked(false);

    mEKUList->clear();
    mEKUGroup->setChecked(false);
    mEKUCriticalCheck->setChecked(false);

    mIANGroup->setChecked(false);
    mIANCriticalCheck->setChecked(false);
    rowCnt = mIANTable->rowCount();
    for( int i = 0; i < rowCnt; i++ )
        mIANTable->removeRow(0);

    mKeyUsageList->clear();
    mKeyUsageGroup->setChecked(false);
    mKeyUsageCriticalCheck->setChecked(false);

    rowCnt = mNCTable->rowCount();
    for( int i=0; i < rowCnt; i++ )
        mNCTable->removeRow(0);
    mNCGroup->setChecked(false);
    mNCCriticalCheck->setChecked(false);

    rowCnt = mPolicyTable->rowCount();
    for( int i=0; i < rowCnt; i++ )
        mPolicyTable->removeRow(0);
    mPolicyGroup->setChecked(false);
    mPolicyCriticalCheck->setChecked(false);

    mPCGroup->setChecked(false);
    mPCCriticalCheck->setChecked(false);
    mPCInhibitText->setText("");
    mPCExplicitText->setText("");

    rowCnt = mPMTable->rowCount();
    for( int i=0; i < rowCnt; i++ )
        mPMTable->removeRow(0);

    mPMGroup->setChecked(false);
    mPMCriticalCheck->setChecked(false);

    mSKIGroup->setChecked(false);
    mSKICriticalCheck->setChecked(false);

    rowCnt = mSANTable->rowCount();
    for( int i = 0; i < rowCnt; i++ )
        mSANTable->removeRow(0);
    mSANGroup->setChecked(false);
    mSANCriticalCheck->setChecked(false);

    mUseCSRCheck->setChecked(true);
    clickUseCSR();

    mUseDaysCheck->setChecked(true);
    clickUseDays();

    mDaysText->setText("365");
}

void MakeCertProfileDlg::accept()
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

    int nProfileNum = manApplet->ccClient()->getNum( ITEM_TYPE_CERT_PROFILE );

    JCC_CertProfile sCertProfile;
    memset( &sCertProfile, 0x00, sizeof(sCertProfile));
    int nNotBefore = 0;
    int nNotAfter = 0;

    if( mUseDaysCheck->isChecked() )
    {
        nNotBefore = mDaysTypeCombo->currentIndex();
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

    JS_DB_setCertProfile( &sCertProfile,
                         nProfileNum,
                         strName.toStdString().c_str(),
                          0,
                         mVersionCombo->currentIndex(),
                         nNotBefore,
                         nNotAfter,
                         mExtUsageCombo->currentIndex(),
                         mHashCombo->currentText().toStdString().c_str(),
                         strSubjectDN.toStdString().c_str() );

    if( is_edit_ )
    {
        manApplet->ccClient()->modCertProfile( profile_num_, &sCertProfile );
        manApplet->ccClient()->delCertProfileExts( profile_num_ );
        nProfileNum = profile_num_;
    }
    else
    {
        manApplet->ccClient()->addCertProfile( &sCertProfile );
    }

    /* need to set extend fields here */
    if( mAIAGroup->isChecked() ) saveAIAUse( nProfileNum );
    if( mAKIGroup->isChecked() ) saveAKIUse( nProfileNum );
    if( mBCGroup->isChecked() ) saveBCUse( nProfileNum );
    if( mCRLDPGroup->isChecked() ) saveCRLDPUse( nProfileNum );
    if( mEKUGroup->isChecked() ) saveEKUUse( nProfileNum );
    if( mIANGroup->isChecked() ) saveIANUse( nProfileNum );
    if( mKeyUsageGroup->isChecked() ) saveKeyUsageUse( nProfileNum );
    if( mNCGroup->isChecked() ) saveNCUse( nProfileNum );
    if( mPolicyGroup->isChecked() ) savePolicyUse( nProfileNum );
    if( mPCGroup->isChecked() ) savePCUse( nProfileNum );
    if( mPMGroup->isChecked() ) savePMUse( nProfileNum );
    if( mSKIGroup->isChecked() ) saveSKIUse( nProfileNum );
    if( mSANGroup->isChecked() ) saveSANUse( nProfileNum );
    if( mExtensionsGroup->isChecked() ) saveExtensionsUse( nProfileNum );
    /* ....... */

    manApplet->mainWindow()->createRightCertProfileList();
    JS_DB_resetCertProfile( &sCertProfile );
    QDialog::accept();
}

void MakeCertProfileDlg::initUI()
{
    static QStringList kExtUsageList = {
        tr("The Certificate Extension Only"),
        tr("The CSR Extension Only"),
        tr("Both Certificate and CSR and the The certificate first"),
        tr("Both Certificate and CSR and the CSR first")
    };

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

    mDaysTypeCombo->addItems( kPeriodTypes );

    QDateTime   now;
    now.setTime_t(time(NULL));

    mNotBeforeDateTime->setDateTime(now);
    mNotAfterDateTime->setDateTime(now);

    mExtUsageCombo->addItems( kExtUsageList );
}

void MakeCertProfileDlg::setTableMenus()
{
    QStringList sPolicyLabels = { tr("OID"), tr("CPS"), tr("UserNotice") };
    mPolicyTable->setColumnCount(3);
    mPolicyTable->horizontalHeader()->setStretchLastSection(true);
    mPolicyTable->setHorizontalHeaderLabels( sPolicyLabels );
    mPolicyTable->verticalHeader()->setVisible(false);
    mPolicyTable->horizontalHeader()->setStyleSheet( kTableStyle );
    mPolicyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mPolicyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mPolicyTable->setColumnWidth(0, 100);
    mPolicyTable->setColumnWidth(1, 100);

    QStringList sCRLDPLabels = { tr("Type"), tr("Value") };
    mCRLDPTable->setColumnCount(2);
    mCRLDPTable->horizontalHeader()->setStretchLastSection(true);
    mCRLDPTable->setHorizontalHeaderLabels(sCRLDPLabels);
    mCRLDPTable->verticalHeader()->setVisible(false);
    mCRLDPTable->horizontalHeader()->setStyleSheet( kTableStyle );
    mCRLDPTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mCRLDPTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mCRLDPTable->setColumnWidth(0,60);

    QStringList sAIALabels = { tr("Target"), tr("Type"), tr("Value") };
    mAIATable->setColumnCount(3);
    mAIATable->horizontalHeader()->setStretchLastSection(true);
    mAIATable->setHorizontalHeaderLabels(sAIALabels);
    mAIATable->verticalHeader()->setVisible(false);
    mAIATable->horizontalHeader()->setStyleSheet( kTableStyle );
    mAIATable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mAIATable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mAIATable->setColumnWidth(0,60);
    mAIATable->setColumnWidth(1,60);

    QStringList sSANLabels = { tr("Type"), tr("Value") };
    mSANTable->setColumnCount(2);
    mSANTable->horizontalHeader()->setStretchLastSection(true);
    mSANTable->setHorizontalHeaderLabels(sSANLabels);
    mSANTable->verticalHeader()->setVisible(false);
    mSANTable->horizontalHeader()->setStyleSheet( kTableStyle );
    mSANTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mSANTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mSANTable->setColumnWidth(0,60);

    QStringList sIANLabels = { tr("Type"), tr("Value") };
    mIANTable->setColumnCount(2);
    mIANTable->horizontalHeader()->setStretchLastSection(true);
    mIANTable->setHorizontalHeaderLabels(sIANLabels);
    mIANTable->verticalHeader()->setVisible(false);
    mIANTable->horizontalHeader()->setStyleSheet( kTableStyle );
    mIANTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mIANTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mIANTable->setColumnWidth(0,60);

    QStringList sPMLabels = { tr("Target"), tr("Value"), tr("Target"), tr("Value") };
    mPMTable->setColumnCount(4);
    mPMTable->horizontalHeader()->setStretchLastSection(true);
    mPMTable->setHorizontalHeaderLabels(sPMLabels);
    mPMTable->verticalHeader()->setVisible(false);
    mPMTable->horizontalHeader()->setStyleSheet( kTableStyle );
    mPMTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mPMTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mPMTable->setColumnWidth(0,100);
    mPMTable->setColumnWidth(1,100);
    mPMTable->setColumnWidth(2,100);

    QStringList sNCLabels = { tr("Type"), tr("Target"), tr("Value"), tr("Min"), tr("Max") };
    mNCTable->setColumnCount(5);
    mNCTable->horizontalHeader()->setStretchLastSection(true);
    mNCTable->setHorizontalHeaderLabels(sNCLabels);
    mNCTable->verticalHeader()->setVisible(false);
    mNCTable->horizontalHeader()->setStyleSheet( kTableStyle );
    mNCTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mNCTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mNCTable->setColumnWidth(0,60);
    mNCTable->setColumnWidth(1,120);
    mNCTable->setColumnWidth(2,200);
    mNCTable->setColumnWidth(3,60);
    mNCTable->setColumnWidth(4,60);

    QStringList sExtensionsLabels = { tr("OID"), tr("Critical"), tr("Value") };
    mExtensionsTable->setColumnCount(sExtensionsLabels.size());
    mExtensionsTable->horizontalHeader()->setStretchLastSection(true);
    mExtensionsTable->setHorizontalHeaderLabels(sExtensionsLabels);
    mExtensionsTable->verticalHeader()->setVisible(false);
    mExtensionsTable->horizontalHeader()->setStyleSheet( kTableStyle );
    mExtensionsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mExtensionsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mExtensionsTable->setColumnWidth(0,180);
    mExtensionsTable->setColumnWidth(1,60);
}

void MakeCertProfileDlg::connectExtends()
{
    connect( mUseCSRCheck, SIGNAL(clicked()), this, SLOT(clickUseCSR()));
    connect( mUseDaysCheck, SIGNAL(clicked()), this, SLOT(clickUseDays()));

    connect( mAIAGroup, SIGNAL(clicked()), this, SLOT(clickAIAUse()));
    connect( mAKIGroup, SIGNAL(clicked()), this, SLOT(clickAKIUse()));
    connect( mBCGroup, SIGNAL(clicked()), this, SLOT(clickBCUse()));
    connect( mCRLDPGroup, SIGNAL(clicked()), this, SLOT(clickCRLDPUse()));
    connect( mEKUGroup, SIGNAL(clicked()), this, SLOT(clickEKUUse()));
    connect( mIANGroup, SIGNAL(clicked()), this, SLOT(clickIANUse()));
    connect( mKeyUsageGroup, SIGNAL(clicked()), this, SLOT(clickKeyUsageUse()));
    connect( mNCGroup, SIGNAL(clicked()), this, SLOT( clickNCUse()));
    connect( mPolicyGroup, SIGNAL(clicked()), this, SLOT(clickPolicyUse()));
    connect( mPCGroup, SIGNAL(clicked()), this, SLOT(clickPCUse()));
    connect( mPMGroup, SIGNAL(clicked()), this, SLOT(clickPMUse()));
    connect( mSKIGroup, SIGNAL(clicked()), this, SLOT(clickSKIUse()));
    connect( mSANGroup, SIGNAL(clicked()), this, SLOT(clickSANUse()));
    connect( mExtensionsGroup, SIGNAL(clicked()), this, SLOT(clickExtensionsUse()));

    connect( mKeyUsageAddBtn, SIGNAL(clicked()), this, SLOT(addKeyUsage()));
    connect( mPolicyAddBtn, SIGNAL(clicked()), this, SLOT(addPolicy()));
    connect( mEKUAddBtn, SIGNAL(clicked()), this, SLOT(addEKU()));
    connect( mCRLDPAddBtn, SIGNAL(clicked()), this, SLOT(addCRLDP()));
    connect( mAIAAddBtn, SIGNAL(clicked()), this, SLOT(addAIA()));
    connect( mSANAddBtn, SIGNAL(clicked()), this, SLOT(addSAN()));
    connect( mIANAddBtn, SIGNAL(clicked()), this, SLOT(addIAN()));
    connect( mPMAddBtn, SIGNAL(clicked()), this, SLOT(addPM()));
    connect( mNCAddBtn, SIGNAL(clicked()), this, SLOT(addNC()));
    connect( mExtensionsAddBtn, SIGNAL(clicked()), this, SLOT(addExtensions()));

    connect( mKeyUsageClearBtn, SIGNAL(clicked()), this, SLOT(clearKeyUsage()));
    connect( mPolicyClearBtn, SIGNAL(clicked()), this, SLOT(clearPolicy()));
    connect( mEKUClearBtn, SIGNAL(clicked()), this, SLOT(clearEKU()));
    connect( mCRLDPClearBtn, SIGNAL(clicked()), this, SLOT(clearCRLDP()));
    connect( mAIAClearBtn, SIGNAL(clicked()), this, SLOT(clearAIA()));
    connect( mSANClearBtn, SIGNAL(clicked()), this, SLOT(clearSAN()));
    connect( mIANClearBtn, SIGNAL(clicked()), this, SLOT(clearIAN()));
    connect( mPMClearBtn, SIGNAL(clicked()), this, SLOT(clearPM()));
    connect( mNCClearBtn, SIGNAL(clicked()), this, SLOT(clearNC()));
    connect( mExtensionsClearBtn, SIGNAL(clicked()), this, SLOT(clearExtensions()));

    connect( mKeyUsageList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotKeyUsageMenuRequested(QPoint)));
    connect( mEKUList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotEKUMenuRequested(QPoint)));
    connect( mPolicyTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotPolicyMenuRequested(QPoint)));
    connect( mCRLDPTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCRLDPMenuRequested(QPoint)));
    connect( mAIATable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotAIAMenuRequested(QPoint)));
    connect( mSANTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotSANMenuRequested(QPoint)));
    connect( mIANTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotIANMenuRequested(QPoint)));
    connect( mPMTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotPMMenuRequested(QPoint)));
    connect( mNCTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotNCMenuRequested(QPoint)));
    connect( mExtensionsTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotExtensionsMenuRequested(QPoint)));
}

void MakeCertProfileDlg::slotKeyUsageMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    QAction *delAct = new QAction( tr("Delete"), this );
    connect( delAct, SIGNAL(triggered()), this, SLOT(deleteKeyUsageMenu()));

    menu->addAction( delAct );
    menu->popup( mKeyUsageList->viewport()->mapToGlobal(pos));
}

void MakeCertProfileDlg::deleteKeyUsageMenu()
{
    QModelIndex idx = mKeyUsageList->currentIndex();
    QListWidgetItem *item = mKeyUsageList->takeItem(idx.row());
    if( item ) delete item;
}

void MakeCertProfileDlg::slotEKUMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    QAction *delAct = new QAction( tr("Delete"), this );
    connect( delAct, SIGNAL(triggered()), this, SLOT(deleteEKUMenu()));

    menu->addAction( delAct );
    menu->popup( mEKUList->viewport()->mapToGlobal(pos));
}

void MakeCertProfileDlg::deleteEKUMenu()
{
    QModelIndex idx = mEKUList->currentIndex();
    QListWidgetItem *item = mEKUList->takeItem(idx.row());
    if( item ) delete item;
}

void MakeCertProfileDlg::slotPolicyMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    QAction *delAct = new QAction( tr("Delete"), this );
    connect( delAct, SIGNAL(triggered()), this, SLOT(deletePolicyMenu()));

    menu->addAction( delAct );
    menu->popup( mPolicyTable->viewport()->mapToGlobal(pos));
}

void MakeCertProfileDlg::deletePolicyMenu()
{
    QModelIndex idx = mPolicyTable->currentIndex();
    mPolicyTable->removeRow( idx.row() );
}

void MakeCertProfileDlg::slotCRLDPMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    QAction *delAct = new QAction( tr("Delete"), this );
    connect( delAct, SIGNAL(triggered()), this, SLOT(deleteCRLDPMenu()));

    menu->addAction( delAct );
    menu->popup( mCRLDPTable->viewport()->mapToGlobal(pos));
}

void MakeCertProfileDlg::deleteCRLDPMenu()
{
    QModelIndex idx = mCRLDPTable->currentIndex();
    mCRLDPTable->removeRow( idx.row() );
}

void MakeCertProfileDlg::slotAIAMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    QAction *delAct = new QAction( tr("Delete"), this );
    connect( delAct, SIGNAL(triggered()), this, SLOT(deleteAIAMenu()));

    menu->addAction( delAct );
    menu->popup( mAIATable->viewport()->mapToGlobal(pos));
}

void MakeCertProfileDlg::deleteAIAMenu()
{
    QModelIndex idx = mAIATable->currentIndex();
    mAIATable->removeRow( idx.row() );
}

void MakeCertProfileDlg::slotSANMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    QAction *delAct = new QAction( tr("Delete"), this );
    connect( delAct, SIGNAL(triggered()), this, SLOT(deleteSANMenu()));

    menu->addAction( delAct );
    menu->popup( mSANTable->viewport()->mapToGlobal(pos));
}

void MakeCertProfileDlg::deleteSANMenu()
{
    QModelIndex idx = mSANTable->currentIndex();
    mSANTable->removeRow( idx.row() );
}

void MakeCertProfileDlg::slotIANMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    QAction *delAct = new QAction( tr("Delete"), this );
    connect( delAct, SIGNAL(triggered()), this, SLOT(deleteIANMenu()));

    menu->addAction( delAct );
    menu->popup( mIANTable->viewport()->mapToGlobal(pos));
}

void MakeCertProfileDlg::deleteIANMenu()
{
    QModelIndex idx = mIANTable->currentIndex();
    mIANTable->removeRow( idx.row() );
}

void MakeCertProfileDlg::slotPMMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    QAction *delAct = new QAction( tr("Delete"), this );
    connect( delAct, SIGNAL(triggered()), this, SLOT(deletePMMenu()));

    menu->addAction( delAct );
    menu->popup( mPMTable->viewport()->mapToGlobal(pos));
}

void MakeCertProfileDlg::deletePMMenu()
{
    QModelIndex idx = mPMTable->currentIndex();
    mPMTable->removeRow( idx.row() );
}

void MakeCertProfileDlg::slotNCMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    QAction *delAct = new QAction( tr("Delete"), this );
    connect( delAct, SIGNAL(triggered()), this, SLOT(deleteNCMenu()));

    menu->addAction( delAct );
    menu->popup( mNCTable->viewport()->mapToGlobal(pos));
}

void MakeCertProfileDlg::slotExtensionsMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    QAction *delAct = new QAction( tr("Delete"), this );
    connect( delAct, SIGNAL(triggered()), this, SLOT(deleteExtensionsMenu()));

    menu->addAction( delAct );
    menu->popup( mExtensionsTable->viewport()->mapToGlobal(pos));
}

void MakeCertProfileDlg::deleteExtensionsMenu()
{
    QModelIndex idx = mExtensionsTable->currentIndex();
    mExtensionsTable->removeRow( idx.row() );
}

void MakeCertProfileDlg::deleteNCMenu()
{
    QModelIndex idx = mNCTable->currentIndex();
    mNCTable->removeRow( idx.row() );
}

void MakeCertProfileDlg::clickUseCSR()
{
    bool bStatus = mUseCSRCheck->isChecked();

    mSubjectDNText->setEnabled( !bStatus );

    if( bStatus )
        mSubjectDNText->setText( "#CSR_DN" );
    else {
        mSubjectDNText->setText( "" );
    }
}

void MakeCertProfileDlg::clickUseDays()
{
    bool bStatus = mUseDaysCheck->isChecked();

    mDaysTypeCombo->setEnabled( bStatus );
    mDaysText->setEnabled(bStatus);
    mNotAfterDateTime->setEnabled(!bStatus);
    mNotBeforeDateTime->setEnabled(!bStatus);
}

void MakeCertProfileDlg::setExtends()
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
    clickExtensionsUse();
}


void MakeCertProfileDlg::clickAIAUse()
{
    bool bStatus = mAIAGroup->isChecked();

    mAIACriticalCheck->setEnabled(bStatus);
    mAIAClearBtn->setEnabled(bStatus);
    mAIAAddBtn->setEnabled(bStatus);
    mAIATypeCombo->setEnabled(bStatus);
    mAIATargetCombo->setEnabled(bStatus);
    mAIAText->setEnabled(bStatus);
    mAIATable->setEnabled(bStatus);
}

void MakeCertProfileDlg::clickAKIUse()
{
    bool bStatus = mAKIGroup->isChecked();

    mAKICriticalCheck->setEnabled(bStatus);
    mAKICertIssuerCheck->setEnabled(bStatus);
    mAKICertSerialCheck->setEnabled(bStatus);
}

void MakeCertProfileDlg::clickBCUse()
{
    bool bStatus = mBCGroup->isChecked();

    mBCCriticalCheck->setEnabled(bStatus);
    mBCCombo->setEnabled(bStatus);
    mBCPathLenText->setEnabled(bStatus);
}

void MakeCertProfileDlg::clickCRLDPUse()
{
    bool bStatus = mCRLDPGroup->isChecked();

    mCRLDPCriticalCheck->setEnabled( bStatus );
    mCRLDPCombo->setEnabled(bStatus);
    mCRLDPClearBtn->setEnabled(bStatus);
    mCRLDPAddBtn->setEnabled(bStatus);
    mCRLDPText->setEnabled(bStatus);
    mCRLDPTable->setEnabled(bStatus);
}

void MakeCertProfileDlg::clickEKUUse()
{
    bool bStatus = mEKUGroup->isChecked();

    mEKUCriticalCheck->setEnabled( bStatus );
    mEKUCombo->setEnabled(bStatus);
    mEKUClearBtn->setEnabled(bStatus);
    mEKUAddBtn->setEnabled(bStatus);
    mEKUList->setEnabled(bStatus);
}

void MakeCertProfileDlg::clickIANUse()
{
    bool bStatus = mIANGroup->isChecked();

    mIANCriticalCheck->setEnabled(bStatus);
    mIANCombo->setEnabled(bStatus);
    mIANText->setEnabled(bStatus);
    mIANTable->setEnabled(bStatus);
    mIANClearBtn->setEnabled( bStatus );
    mIANAddBtn->setEnabled(bStatus);
}

void MakeCertProfileDlg::clickKeyUsageUse()
{
    bool bStatus = mKeyUsageGroup->isChecked();

    mKeyUsageCriticalCheck->setEnabled( bStatus );
    mKeyUsageCombo->setEnabled( bStatus );
    mKeyUsageClearBtn->setEnabled( bStatus );
    mKeyUsageAddBtn->setEnabled( bStatus );
    mKeyUsageList->setEnabled( bStatus );
}

void MakeCertProfileDlg::clickNCUse()
{
    bool bStatus = mNCGroup->isChecked();

    mNCCriticalCheck->setEnabled(bStatus);
    mNCSubCombo->setEnabled(bStatus);
    mNCClearBtn->setEnabled(bStatus);
    mNCAddBtn->setEnabled(bStatus);
    mNCTypeCombo->setEnabled(bStatus);
    mNCSubText->setEnabled(bStatus);
    mNCMaxText->setEnabled(bStatus);
    mNCMinText->setEnabled(bStatus);
    mNCTable->setEnabled(bStatus);
}

void MakeCertProfileDlg::clickPolicyUse()
{
    bool bStatus = mPolicyGroup->isChecked();

    mPolicyCriticalCheck->setEnabled( bStatus );
    mPolicyClearBtn->setEnabled( bStatus );
    mPolicyAddBtn->setEnabled( bStatus );
    mPolicyOIDText->setEnabled( bStatus );
    mPolicyCPSText->setEnabled( bStatus );
    mPolicyUserNoticeText->setEnabled( bStatus );
    mPolicyTable->setEnabled( bStatus );
}

void MakeCertProfileDlg::clickPCUse()
{
    bool bStatus = mPCGroup->isChecked();

    mPCCriticalCheck->setEnabled(bStatus);
    mPCInhibitText->setEnabled(bStatus);
    mPCExplicitText->setEnabled(bStatus);
}

void MakeCertProfileDlg::clickPMUse()
{
    bool bStatus = mPMGroup->isChecked();

    mPMCriticalCheck->setEnabled(bStatus);
    mPMClearBtn->setEnabled( bStatus );
    mPMAddBtn->setEnabled(bStatus);
    mPMIssuerDomainPolicyText->setEnabled(bStatus);
    mPMSubjectDomainPolicyText->setEnabled(bStatus);
    mPMTable->setEnabled(bStatus);
}

void MakeCertProfileDlg::clickSKIUse()
{
    bool bStatus = mSKIGroup->isChecked();

    mSKICriticalCheck->setEnabled( bStatus );
}

void MakeCertProfileDlg::clickSANUse()
{
    bool bStatus = mSANGroup->isChecked();

    mSANCriticalCheck->setEnabled(bStatus);
    mSANCombo->setEnabled(bStatus);
    mSANClearBtn->setEnabled(bStatus);
    mSANAddBtn->setEnabled(bStatus);
    mSANText->setEnabled(bStatus);
    mSANTable->setEnabled(bStatus);
}

void MakeCertProfileDlg::clickExtensionsUse()
{
    bool bStatus = mExtensionsGroup->isChecked();

    mExtensionsAddBtn->setEnabled(bStatus);
    mExtensionsClearBtn->setEnabled(bStatus);
    mExtensionsOIDText->setEnabled(bStatus);
    mExtensionsCriticalCheck->setEnabled(bStatus);
    mExtensionsValueText->setEnabled(bStatus);
    mExtensionsTable->setEnabled(bStatus);
}

void MakeCertProfileDlg::addKeyUsage()
{
    QString strVal = mKeyUsageCombo->currentText();

    mKeyUsageList->addItem( strVal );
}

void MakeCertProfileDlg::addPolicy()
{
    QString strOID = mPolicyOIDText->text();
    QString strCPS = mPolicyCPSText->text();
    QString strUserNotice = mPolicyUserNoticeText->text();

    int row = mPolicyTable->rowCount();

    mPolicyTable->setRowCount( row + 1 );

    mPolicyTable->setRowHeight( row, 10 );
    mPolicyTable->setItem( row, 0, new QTableWidgetItem(strOID));
    mPolicyTable->setItem( row, 1, new QTableWidgetItem(strCPS));
    mPolicyTable->setItem( row, 2, new QTableWidgetItem(strUserNotice));
}

void MakeCertProfileDlg::addEKU()
{
    QString strVal = mEKUCombo->currentText();

    mEKUList->addItem( strVal );
}

void MakeCertProfileDlg::addCRLDP()
{
    QString strType = mCRLDPCombo->currentText();
    QString strVal = mCRLDPText->text();

    int row = mCRLDPTable->rowCount();
    mCRLDPTable->setRowCount( row + 1 );

    mCRLDPTable->setRowHeight( row, 10 );
    mCRLDPTable->setItem( row, 0, new QTableWidgetItem( strType ));
    mCRLDPTable->setItem( row, 1, new QTableWidgetItem( strVal ));
}

void MakeCertProfileDlg::addAIA()
{
    QString strTarget = mAIATargetCombo->currentText();
    QString strType = mAIATypeCombo->currentText();
    QString strVal = mAIAText->text();

    int row = mAIATable->rowCount();

    mAIATable->setRowCount( row + 1 );

    mAIATable->setRowHeight( row, 10 );
    mAIATable->setItem( row, 0, new QTableWidgetItem( strTarget ));
    mAIATable->setItem( row, 1, new QTableWidgetItem( strType) );
    mAIATable->setItem( row, 2, new QTableWidgetItem( strVal ));
}

void MakeCertProfileDlg::addSAN()
{
    QString strType = mSANCombo->currentText();
    QString strVal = mSANText->text();

    int row = mSANTable->rowCount();
    mSANTable->setRowCount( row + 1 );

    mSANTable->setRowHeight( row, 10 );
    mSANTable->setItem( row, 0, new QTableWidgetItem(strType));
    mSANTable->setItem( row, 1, new QTableWidgetItem(strVal));
}

void MakeCertProfileDlg::addIAN()
{
    QString strType = mIANCombo->currentText();
    QString strVal = mIANText->text();

    int row = mIANTable->rowCount();
    mIANTable->setRowCount( row + 1 );

    mIANTable->setRowHeight( row, 10 );
    mIANTable->setItem( row, 0, new QTableWidgetItem(strType));
    mIANTable->setItem( row, 1, new QTableWidgetItem(strVal));
}

void MakeCertProfileDlg::addPM()
{
    QString strIDP = mPMIssuerDomainPolicyText->text();
    QString strSDP = mPMSubjectDomainPolicyText->text();

    int row = mPMTable->rowCount();
    mPMTable->setRowCount( row + 1 );

    mPMTable->setRowHeight( row, 10 );
    mPMTable->setItem( row, 0, new QTableWidgetItem( "IssuerDomainPolicy"));
    mPMTable->setItem( row, 1, new QTableWidgetItem( strIDP));
    mPMTable->setItem( row, 2, new QTableWidgetItem( "SubjectDomainPolicy"));
    mPMTable->setItem( row, 3, new QTableWidgetItem( strSDP));
}

void MakeCertProfileDlg::addNC()
{
    QString strType = mNCTypeCombo->currentText();
    QString strSubType = mNCSubCombo->currentText();
    QString strVal = mNCSubText->text();
    QString strMax = mNCMaxText->text();
    QString strMin = mNCMinText->text();

    int row = mNCTable->rowCount();
    mNCTable->setRowCount( row + 1 );

    mNCTable->setRowHeight( row, 10 );
    mNCTable->setItem( row, 0, new QTableWidgetItem(strType));
    mNCTable->setItem( row, 1, new QTableWidgetItem(strSubType));
    mNCTable->setItem( row, 2, new QTableWidgetItem(strVal));
    mNCTable->setItem( row, 3, new QTableWidgetItem(strMin));
    mNCTable->setItem( row, 4, new QTableWidgetItem(strMax));
}

void MakeCertProfileDlg::addExtensions()
{
    QString strOID = mExtensionsOIDText->text();
    QString strValue = mExtensionsValueText->toPlainText();
    bool bCrit = mExtensionsCriticalCheck->isChecked();
    QString strCrit;

    if( bCrit )
        strCrit = "ture";
    else
        strCrit = "false";

    int row = mExtensionsTable->rowCount();
    mExtensionsTable->setRowCount( row + 1 );
    mExtensionsTable->setRowHeight( row, 10 );
    mExtensionsTable->setItem( row, 0, new QTableWidgetItem(strOID));
    mExtensionsTable->setItem( row, 1, new QTableWidgetItem(strCrit));
    mExtensionsTable->setItem( row, 2, new QTableWidgetItem(strValue));
}

void MakeCertProfileDlg::clearKeyUsage()
{
    mKeyUsageList->clear();
}

void MakeCertProfileDlg::clearPolicy()
{
    int nCnt = mPolicyTable->rowCount();

    for( int i=0; i < nCnt; i++ )
        mPolicyTable->removeRow(0);
}

void MakeCertProfileDlg::clearEKU()
{
    mEKUList->clear();
}

void MakeCertProfileDlg::clearCRLDP()
{
    int nCnt = mCRLDPTable->rowCount();

    for( int i=0; i < nCnt; i++)
        mCRLDPTable->removeRow(0);
}

void MakeCertProfileDlg::clearAIA()
{
    int nCnt = mAIATable->rowCount();

    for( int i=0; i < nCnt; i++)
        mAIATable->removeRow(0);
}

void MakeCertProfileDlg::clearSAN()
{
    int nCnt = mSANTable->rowCount();

    for( int i=0; i < nCnt; i++)
        mSANTable->removeRow(0);
}

void MakeCertProfileDlg::clearIAN()
{
    int nCnt = mIANTable->rowCount();

    for( int i=0; i < nCnt; i++)
        mIANTable->removeRow(0);
}

void MakeCertProfileDlg::clearPM()
{
    int nCnt = mPMTable->rowCount();

    for( int i=0; i < nCnt; i++)
        mPMTable->removeRow(0);
}

void MakeCertProfileDlg::clearNC()
{
    int nCnt = mNCTable->rowCount();

    for( int i=0; i < nCnt; i++)
        mNCTable->removeRow(0);
}

void MakeCertProfileDlg::clearExtensions()
{
    int nCount = mExtensionsTable->rowCount();

    for( int i = 0; i < nCount; i++ )
        mExtensionsTable->removeRow(0);
}

void MakeCertProfileDlg::saveAIAUse(int nProfileNum )
{
    JCC_ProfileExt   sProfileExt;
    memset( &sProfileExt, 0x00, sizeof(sProfileExt));

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

    JS_DB_setProfileExt( &sProfileExt, -1, nProfileNum, mAIACriticalCheck->isChecked(), "authorityInfoAccess", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCertProfileExt( nProfileNum, &sProfileExt );
    JS_DB_resetProfileExt( &sProfileExt );
}

void MakeCertProfileDlg::saveAKIUse(int nProfileNum )
{
    JCC_ProfileExt   sProfileExt;
    memset( &sProfileExt, 0x00, sizeof(sProfileExt));

    QString strVal;

    if( mAKICertIssuerCheck->isChecked() ) strVal += "ISSUER#";
    if( mAKICertSerialCheck->isChecked() ) strVal += "SERIAL#";

    JS_DB_setProfileExt( &sProfileExt, -1, nProfileNum, mAKICriticalCheck->isChecked(), "authorityKeyIdentifier", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCertProfileExt( nProfileNum, &sProfileExt );
    JS_DB_resetProfileExt( &sProfileExt );
}

void MakeCertProfileDlg::saveBCUse(int nProfileNum )
{
    JCC_ProfileExt   sProfileExt;
    memset( &sProfileExt, 0x00, sizeof(sProfileExt));

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

    JS_DB_setProfileExt( &sProfileExt, -1, nProfileNum, mBCCriticalCheck->isChecked(), "basicConstraints", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCertProfileExt( nProfileNum, &sProfileExt );
    JS_DB_resetProfileExt( &sProfileExt );
}

void MakeCertProfileDlg::saveCRLDPUse(int nProfileNum )
{
    JCC_ProfileExt   sProfileExt;
    memset( &sProfileExt, 0x00, sizeof(sProfileExt));

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

    JS_DB_setProfileExt( &sProfileExt, -1, nProfileNum, mCRLDPCriticalCheck->isChecked(), "crlDistributionPoints", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCertProfileExt( nProfileNum, &sProfileExt );
    JS_DB_resetProfileExt( &sProfileExt );
}

void MakeCertProfileDlg::saveEKUUse(int nProfileNum )
{
    JCC_ProfileExt   sProfileExt;
    memset( &sProfileExt, 0x00, sizeof(sProfileExt));

    QString strVal = "";

    for( int i=0; i < mEKUList->count(); i++ )
    {
        if( i != 0 ) strVal += "#";
        strVal += mEKUList->item(i)->text();
    }

    JS_DB_setProfileExt( &sProfileExt, -1, nProfileNum, mEKUCriticalCheck->isChecked(), "extendedKeyUsage", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCertProfileExt( nProfileNum, &sProfileExt );
    JS_DB_resetProfileExt( &sProfileExt );
}

void MakeCertProfileDlg::saveIANUse(int nProfileNum )
{
    JCC_ProfileExt   sProfileExt;
    memset( &sProfileExt, 0x00, sizeof(sProfileExt));

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

    JS_DB_setProfileExt( &sProfileExt, -1, nProfileNum, mIANCriticalCheck->isChecked(), "issuerAltName", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCertProfileExt( nProfileNum, &sProfileExt );
    JS_DB_resetProfileExt( &sProfileExt );
}

void MakeCertProfileDlg::saveKeyUsageUse(int nProfileNum )
{
    JCC_ProfileExt   sProfileExt;
    memset( &sProfileExt, 0x00, sizeof(sProfileExt));

    QString strValue;

    for( int i =0; i < mKeyUsageList->count(); i++ )
    {
        if( i != 0 ) strValue += "#";
        strValue += mKeyUsageList->item(i)->text();
    }

    JS_DB_setProfileExt( &sProfileExt, -1, nProfileNum, mKeyUsageCriticalCheck->isChecked(), "keyUsage", strValue.toStdString().c_str() );
    manApplet->ccClient()->addCertProfileExt( nProfileNum, &sProfileExt );
    JS_DB_resetProfileExt( &sProfileExt );
}

void MakeCertProfileDlg::saveNCUse(int nProfileNum )
{
    JCC_ProfileExt   sProfileExt;
    memset( &sProfileExt, 0x00, sizeof(sProfileExt));

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

    JS_DB_setProfileExt( &sProfileExt, -1, nProfileNum, mNCCriticalCheck->isChecked(), "nameConstraints", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCertProfileExt( nProfileNum, &sProfileExt );
    JS_DB_resetProfileExt( &sProfileExt );
}

void MakeCertProfileDlg::savePolicyUse(int nProfileNum )
{
    JCC_ProfileExt   sProfileExt;
    memset( &sProfileExt, 0x00, sizeof(sProfileExt));

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

    JS_DB_setProfileExt( &sProfileExt, -1, nProfileNum, mPolicyCriticalCheck->isChecked(), "certificatePolicies", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCertProfileExt( nProfileNum, &sProfileExt );
    JS_DB_resetProfileExt( &sProfileExt );
}

void MakeCertProfileDlg::savePCUse(int nProfileNum )
{
    JCC_ProfileExt   sProfileExt;
    memset( &sProfileExt, 0x00, sizeof(sProfileExt));

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

    JS_DB_setProfileExt( &sProfileExt, -1, nProfileNum, mPCCriticalCheck->isChecked(), "policyConstraints", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCertProfileExt( nProfileNum, &sProfileExt );
    JS_DB_resetProfileExt( &sProfileExt );
}

void MakeCertProfileDlg::savePMUse(int nProfileNum )
{
    JCC_ProfileExt   sProfileExt;
    memset( &sProfileExt, 0x00, sizeof(sProfileExt));

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

    JS_DB_setProfileExt( &sProfileExt, -1, nProfileNum, mPMCriticalCheck->isChecked(), "policyMappings", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCertProfileExt( nProfileNum, &sProfileExt );
    JS_DB_resetProfileExt( &sProfileExt );
}

void MakeCertProfileDlg::saveSKIUse(int nProfileNum )
{
    JCC_ProfileExt   sProfileExt;
    memset( &sProfileExt, 0x00, sizeof(sProfileExt));

    JS_DB_setProfileExt( &sProfileExt, -1, nProfileNum, mSKICriticalCheck->isChecked(), "subjectKeyIdentifier", NULL );
    manApplet->ccClient()->addCertProfileExt( nProfileNum, &sProfileExt );
    JS_DB_resetProfileExt( &sProfileExt );
}

void MakeCertProfileDlg::saveSANUse(int nProfileNum)
{
    JCC_ProfileExt   sProfileExt;
    memset( &sProfileExt, 0x00, sizeof(sProfileExt));

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

    JS_DB_setProfileExt( &sProfileExt, -1, nProfileNum, mSANCriticalCheck->isChecked(), "subjectAltName", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCertProfileExt( nProfileNum, &sProfileExt );
    JS_DB_resetProfileExt( &sProfileExt );
}

void MakeCertProfileDlg::saveExtensionsUse( int nProfileNum )
{
    JCC_ProfileExt   sProfileExt;
    memset( &sProfileExt, 0x00, sizeof(sProfileExt));

    int nCount = mExtensionsTable->rowCount();

    for( int i = 0; i < nCount; i++ )
    {
        JCC_ProfileExt   sProfileExt;
        memset( &sProfileExt, 0x00, sizeof(sProfileExt));

        bool bCrit = false;
        QString strOID = mExtensionsTable->takeItem( i, 0 )->text();
        QString strCrit = mExtensionsTable->takeItem( i, 1 )->text();
        QString strValue = mExtensionsTable->takeItem( i, 2)->text();

        if( strCrit == "true" ) bCrit = true;

        JS_DB_setProfileExt( &sProfileExt, -1, nProfileNum, bCrit, strOID.toStdString().c_str(), strValue.toStdString().c_str() );
        manApplet->ccClient()->addCertProfileExt( nProfileNum, &sProfileExt );
        JS_DB_resetProfileExt( &sProfileExt );
    }
}

void MakeCertProfileDlg::setAIAUse( JCC_ProfileExt *pProfileExt )
{
    mAIAGroup->setChecked(true);
    mAIACriticalCheck->setChecked( pProfileExt->bCritical );
    clickAIAUse();

    QString strVal = pProfileExt->pValue;

    QStringList valList = strVal.split("#");

    for( int i=0; i < valList.size(); i++ )
    {
        QString info = valList.at(i);
        QString strMethod = "";
        QString strType = "";
        QString strData = "";

        QStringList infoList = info.split("$");

        if( infoList.size() > 0 )
            strMethod = infoList.at(0);

        if(infoList.size() > 1)
            strType = infoList.at(1);

        if( infoList.size() > 2)
            strData = infoList.at(2);

        mAIATable->insertRow(i);
        mAIATable->setItem( i, 0, new QTableWidgetItem(strMethod));
        mAIATable->setItem( i, 1, new QTableWidgetItem(strType));
        mAIATable->setItem( i, 2, new QTableWidgetItem(strData));
    }
}

void MakeCertProfileDlg::setAKIUse( JCC_ProfileExt *pProfileExt )
{
    mAKIGroup->setChecked(true);
    mAKICriticalCheck->setChecked( pProfileExt->bCritical );
    clickAKIUse();

    QString strVal = pProfileExt->pValue;

    bool bStatus = strVal.contains("ISSUER");
    mAKICertIssuerCheck->setChecked(bStatus);

    bStatus = strVal.contains("SERIAL");
    mAKICertSerialCheck->setChecked(bStatus);
}

void MakeCertProfileDlg::setBCUse( JCC_ProfileExt *pProfileExt )
{
    mBCGroup->setChecked(true);
    mBCCriticalCheck->setChecked( pProfileExt->bCritical );
    clickBCUse();

    QString strVal = pProfileExt->pValue;
    QString strType;
    QString strLen;

    QStringList valList = strVal.split("#");
    if( valList.size() > 0 )
        strType= valList.at(0);

    if( valList.size() > 1 )
        strLen = valList.at(1);

    mBCCombo->setCurrentText( strType );
    mBCPathLenText->setText( strLen );
}

void MakeCertProfileDlg::setCRLDPUse( JCC_ProfileExt *pProfileExt )
{
    mCRLDPGroup->setChecked(true);
    mCRLDPCriticalCheck->setChecked( pProfileExt->bCritical );
    clickCRLDPUse();

    QString strVal = pProfileExt->pValue;

    QStringList valList = strVal.split("#");

    for( int i=0; i < valList.size(); i++ )
    {
        QString info = valList.at(i);
        QString strType;
        QString strData;

        QStringList typeData = info.split("$");

        if( typeData.size() > 0 )
            strType = typeData.at(0);

        if( typeData.size() > 1)
            strData = typeData.at(1);

        mCRLDPTable->insertRow(i);
        mCRLDPTable->setItem( i, 0, new QTableWidgetItem(strType));
        mCRLDPTable->setItem( i, 1, new QTableWidgetItem(strData));
    }
}

void MakeCertProfileDlg::setEKUUse( JCC_ProfileExt *pProfileExt )
{
    QString strVal = "";

    mEKUGroup->setChecked(true);
    mEKUCriticalCheck->setChecked( pProfileExt->bCritical );
    clickEKUUse();

    strVal = pProfileExt->pValue;
    QStringList valList = strVal.split("#");

    if( valList.size() > 0 ) mEKUList->insertItems( 0, valList );
}

void MakeCertProfileDlg::setIANUse( JCC_ProfileExt *pProfileExt )
{
    mIANGroup->setChecked(true);
    mIANCriticalCheck->setChecked( pProfileExt->bCritical );
    clickIANUse();

    QString strVal = pProfileExt->pValue;

    QStringList valList = strVal.split("#");

    for( int i=0; i < valList.size(); i++ )
    {
        QString info = valList.at(i);
        QString strType;
        QString strData;

        QStringList infoList = info.split("$");

        if( infoList.size() > 0 )
            strType = infoList.at(0);

        if( infoList.size() > 1 )
            strData = infoList.at(1);

        mIANTable->insertRow(i);
        mIANTable->setItem( i, 0, new QTableWidgetItem(strType));
        mIANTable->setItem(i, 1, new QTableWidgetItem(strData));
    }
}

void MakeCertProfileDlg::setKeyUsageUse( JCC_ProfileExt *pProfileExt )
{
    mKeyUsageGroup->setChecked(true);
    mKeyUsageCriticalCheck->setChecked( pProfileExt->bCritical );
    clickKeyUsageUse();

    QString strVal = pProfileExt->pValue;

    mKeyUsageList->clear();

    QStringList valList = strVal.split("#");
    if( valList.size() > 0 ) mKeyUsageList->insertItems(0, valList );
}

void MakeCertProfileDlg::setNCUse( JCC_ProfileExt *pProfileExt )
{
    mNCGroup->setChecked(true);
    mNCCriticalCheck->setChecked( pProfileExt->bCritical );
    clickNCUse();

    QString strVal = pProfileExt->pValue;

    QStringList valList = strVal.split("#");

    for( int i=0; i < valList.size(); i++ )
    {
        QString info = valList.at(i);
        QString strType;
        QString strKind;
        QString strData;

        QStringList infoList = info.split("$");

        if( infoList.size() > 0 )
            strType = infoList.at(0);

        if( infoList.size() > 1 )
            strKind = infoList.at(1);

        if( infoList.size() > 2 )
            strData = infoList.at(2);

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

void MakeCertProfileDlg::setPolicyUse( JCC_ProfileExt *pProfileExt )
{
    mPolicyGroup->setChecked(true);
    mPolicyCriticalCheck->setChecked( pProfileExt->bCritical );
    clickPolicyUse();

    QString strVal = pProfileExt->pValue;

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

void MakeCertProfileDlg::setPCUse( JCC_ProfileExt *pProfileExt )
{
    mPCGroup->setChecked(true);
    mPCCriticalCheck->setChecked( pProfileExt->bCritical );
    clickPCUse();

    QString strVal = pProfileExt->pValue;
    QStringList valList = strVal.split("#");

    for( int i=0; i < valList.size(); i++ )
    {
        QString info = valList.at(i);
        QString strType;
        QString strData;

        QStringList infoList = info.split("$");

        if( infoList.size() > 0 )
            strType = infoList.at(0);

        if( infoList.size() > 1 )
            strData = infoList.at(1);

        if( strType == "REP" )
            mPCExplicitText->setText( strData );
        else if( strType == "IPM" )
            mPCInhibitText->setText( strData );
    }
}

void MakeCertProfileDlg::setPMUse( JCC_ProfileExt *pProfileExt )
{
    mPMGroup->setChecked(true);
    mPMCriticalCheck->setChecked( pProfileExt->bCritical );
    clickPMUse();

    QString strVal = pProfileExt->pValue;

    QStringList valList = strVal.split("#");

    for( int i=0; i < valList.size(); i++ )
    {
        QString info = valList.at(i);
        QString strIDP;
        QString strSDP;

        QStringList infoList = info.split("$");

        if( infoList.size() > 0 )
            strIDP = infoList.at(0);

        if( infoList.size() > 1 )
            strSDP = infoList.at(1);

        mPMTable->insertRow(i);
        mPMTable->setItem(i,0,new QTableWidgetItem("issuerDomainPolicy"));
        mPMTable->setItem(i,1,new QTableWidgetItem(strIDP));
        mPMTable->setItem(i,2,new QTableWidgetItem("subjectDomainPolicy"));
        mPMTable->setItem(i,3,new QTableWidgetItem(strSDP));
    }
}

void MakeCertProfileDlg::setSKIUse( JCC_ProfileExt *pProfileExt )
{
    mSKIGroup->setChecked(true);
    mSKICriticalCheck->setChecked( pProfileExt->bCritical );
    clickSKIUse();
}

void MakeCertProfileDlg::setSANUse( JCC_ProfileExt *pProfileExt )
{
    mSANGroup->setChecked(true);
    mSANCriticalCheck->setChecked( pProfileExt->bCritical );
    clickSANUse();

    QString strVal = pProfileExt->pValue;

    QStringList valList = strVal.split("#");

    for( int i=0; i < valList.size(); i++ )
    {
        QString info = valList.at(i);
        QString strType;
        QString strData;

        QStringList infoList = info.split("$");

        if( infoList.size() > 0 )
            strType = infoList.at(0);

        if( infoList.size() > 1 )
            strData = infoList.at(1);

        mSANTable->insertRow(i);
        mSANTable->setItem( i, 0, new QTableWidgetItem(strType));
        mSANTable->setItem(i, 1, new QTableWidgetItem(strData));
    }
}

void MakeCertProfileDlg::setExtensionsUse( JCC_ProfileExt *pProfileExt )
{
    mExtensionsGroup->setChecked(true);
    clickExtensionsUse();

    QString strOID = pProfileExt->pSN;
    QString strValue = pProfileExt->pValue;
    QString strCrit;

    if( pProfileExt->bCritical )
        strCrit = "true";
    else
        strCrit = "false";

    int row = mExtensionsTable->rowCount();
    mExtensionsTable->setRowCount( row + 1 );
    mExtensionsTable->setRowHeight( row, 10 );
    mExtensionsTable->setItem( row, 0, new QTableWidgetItem(strOID));
    mExtensionsTable->setItem( row, 1, new QTableWidgetItem(strCrit));
    mExtensionsTable->setItem( row, 2, new QTableWidgetItem(strValue));
}

#include <QMenu>

#include "make_crl_profile_dlg.h"
#include "mainwindow.h"
#include "man_applet.h"
#include "common.h"
#include "cc_client.h"
#include "js_db.h"
#include "man_tree_item.h"

static QStringList sHashList = { "SHA1", "SHA224", "SHA256", "SHA384", "SHA512" };
static QStringList sTypeList = { "URI", "email", "DNS" };
static QStringList sVersionList = { "V1", "V2" };


MakeCRLProfileDlg::MakeCRLProfileDlg(QWidget *parent) :
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

    mExtendTab->layout()->setSpacing(5);
    mExtendTab->layout()->setMargin(5);

    mExtensionsGroup->layout()->setSpacing(5);
    mExtensionsGroup->layout()->setMargin(5);

    mCRLNumGroup->layout()->setSpacing(5);
    mCRLNumGroup->layout()->setMargin(5);

    mAKIGroup->layout()->setSpacing(5);
    mAKIGroup->layout()->setMargin(5);

    mIDPGroup->layout()->setSpacing(5);
    mIDPGroup->layout()->setMargin(5);

    mIANGroup->layout()->setSpacing(5);
    mIANGroup->layout()->setMargin(5);
#endif
    resize(minimumSizeHint().width(), minimumSizeHint().height());
}

MakeCRLProfileDlg::~MakeCRLProfileDlg()
{

}

void MakeCRLProfileDlg::setEdit( int nProfileNum )
{
    is_edit_ = true;
    profile_num_ = nProfileNum;

    loadProfile( profile_num_ );
}


void MakeCRLProfileDlg::initialize()
{
    mCRLTab->setCurrentIndex(0);

    defaultProfile();
}


void MakeCRLProfileDlg::loadProfile( int nProfileNum, bool bCopy )
{    
    JCC_CRLProfile sCRLProfile;
    memset( &sCRLProfile, 0x00, sizeof(sCRLProfile));
    manApplet->ccClient()->getCRLProfile( nProfileNum, &sCRLProfile );

    QString strName = sCRLProfile.pName;

    if( bCopy == true )
        mNameText->setText( strName + "_Copy" );
    else
        mNameText->setText( strName );

    mVersionCombo->setCurrentIndex( sCRLProfile.nVersion );
    mHashCombo->setCurrentText( sCRLProfile.pHash );

    if( sCRLProfile.tThisUpdate == 0 )
    {
        mUseFromNowCheck->setChecked(true);
        mValidDaysText->setText( QString("%1").arg(sCRLProfile.tNextUpdate));
    }
    else
    {
        mUseFromNowCheck->setChecked(false);

        QDateTime lastUpdate;
        QDateTime nextUpdate;

        lastUpdate.setTime_t( sCRLProfile.tThisUpdate );
        nextUpdate.setTime_t( sCRLProfile.tNextUpdate );

        mLastUpdateDateTime->setDateTime(lastUpdate);
        mNextUpdateDateTime->setDateTime(nextUpdate );
    }

    clickUseFromNow();

    JCC_ProfileExtList   *pProfileExtList = NULL;
    JCC_ProfileExtList   *pCurList = NULL;

    manApplet->ccClient()->getCRLProfileExtList( nProfileNum, &pProfileExtList );
    pCurList = pProfileExtList;

    while( pCurList )
    {
        if( strcasecmp( pCurList->sProfileExt.pSN, kExtNameCRLNum.toStdString().c_str() ) == 0 )
            setCRLNumUse( &pCurList->sProfileExt );
        else if( strcasecmp( pCurList->sProfileExt.pSN, kExtNameAKI.toStdString().c_str() ) == 0 )
            setAKIUse( &pCurList->sProfileExt );
        else if( strcasecmp( pCurList->sProfileExt.pSN, kExtNameIDP.toStdString().c_str() ) == 0 )
            setIDPUse( &pCurList->sProfileExt );
        else if( strcasecmp( pCurList->sProfileExt.pSN, kExtNameIAN.toStdString().c_str() ) == 0 )
            setIANUse( &pCurList->sProfileExt );
        else
            setExtensionsUse( &pCurList->sProfileExt );

        pCurList = pCurList->pNext;
    }

    JS_DB_resetCRLProfile( &sCRLProfile );
    if( pProfileExtList ) JS_DB_resetProfileExtList( &pProfileExtList );
}

void MakeCRLProfileDlg::defaultProfile()
{
    int rowCnt = 0;
    mNameText->setText("");

    mCRLNumText->setText("");
    mCRLNumGroup->setChecked(false);
    mCRLNumAutoCheck->setChecked(false);
    mCRLNumCriticalCheck->setChecked(false);

    mAKIGroup->setChecked(false);
    mAKICriticalCheck->setChecked(false);
    mAKICertIssuerCheck->setChecked(false);
    mAKICertSerialCheck->setChecked(false);

    rowCnt = mIDPTable->rowCount();
    for( int i=0; i < rowCnt; i++ )
        mIDPTable->removeRow(0);
    mIDPGroup->setChecked(false);
    mIDPCriticalCheck->setChecked(false);

    rowCnt = mIANTable->rowCount();
    for( int i=0; i < rowCnt; i++ )
        mIANTable->removeRow(0);
    mIANGroup->setChecked(false);
    mIANCriticalCheck->setChecked(false);
    mIANText->setText("");

    mVersionCombo->setCurrentIndex(1);
    mHashCombo->setCurrentIndex(2);

    mUseFromNowCheck->setChecked(true);
    clickUseFromNow();
    mValidDaysText->setText("10");
}

void MakeCRLProfileDlg::accept()
{
    QString strName = mNameText->text();

    if( strName.isEmpty() )
    {
        manApplet->warningBox( tr( "You have to insert name"), this );
        mNameText->setFocus();
        return;
    }

    int nProfileNum = manApplet->ccClient()->getNum( ITEM_TYPE_CRL_PROFILE );
    JCC_CRLProfile sCRLProfile;

    memset( &sCRLProfile, 0x00, sizeof(sCRLProfile) );
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

    JS_DB_setCRLProfile( &sCRLProfile,
                        nProfileNum,
                        strName.toStdString().c_str(),
                        mVersionCombo->currentIndex(),
                        nLastUpdate,
                        nNextUpdate,
                        mHashCombo->currentText().toStdString().c_str() );


    if( is_edit_ )
    {       
        manApplet->ccClient()->modCRLProfile( profile_num_, &sCRLProfile );
        manApplet->ccClient()->delCRLProfileExts( profile_num_ );
        nProfileNum = profile_num_;
    }
    else
    {
        manApplet->ccClient()->addCRLProfile( &sCRLProfile );
    }


    /* need to set extend fields here */

    if( mCRLNumGroup->isChecked() ) saveCRLNumUse( nProfileNum );
    if( mIDPGroup->isChecked() ) saveIDPUse( nProfileNum );
    if( mAKIGroup->isChecked() ) saveAKIUse( nProfileNum );
    if( mIANGroup->isChecked() ) saveIANUse( nProfileNum );

    /* ....... */

    JS_DB_resetCRLProfile( &sCRLProfile );
    manApplet->mainWindow()->createRightCRLProfileList();
    QDialog::accept();
}

void MakeCRLProfileDlg::initUI()
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

void MakeCRLProfileDlg::connectExtends()
{
    connect( mCRLNumGroup, SIGNAL(clicked()), this, SLOT(clickCRLNum()));
    connect( mAKIGroup, SIGNAL(clicked()), this, SLOT(clickAKI()));
    connect( mIDPGroup, SIGNAL(clicked()), this, SLOT(clickIDP()));
    connect( mIANGroup, SIGNAL(clicked()), this, SLOT(clickIAN()));
    connect( mUseFromNowCheck, SIGNAL(clicked()), this, SLOT(clickUseFromNow()));
    connect( mExtensionsGroup, SIGNAL(clicked()), this, SLOT(clickExtensionsUse()));

    connect( mIDPAddBtn, SIGNAL(clicked()), this, SLOT(addIDP()));
    connect( mIANAddBtn, SIGNAL(clicked()), this, SLOT(addIAN()));
    connect( mExtensionsAddBtn, SIGNAL(clicked()), this, SLOT(addExtensions()));

    connect( mIDPClearBtn, SIGNAL(clicked()), this, SLOT(clearIDP()));
    connect( mIANClearBtn, SIGNAL(clicked()), this, SLOT(clearIAN()));
    connect( mExtensionsClearBtn, SIGNAL(clicked()), this, SLOT(clearExtensions()));

    connect( mIANTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotIANMenuRequested(QPoint)));
    connect( mIDPTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotIDPMenuRequested(QPoint)));
    connect( mExtensionsTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotExtensionsMenuRequested(QPoint)));
}

void MakeCRLProfileDlg::setExtends()
{
    clickCRLNum();
    clickAKI();
    clickIDP();
    clickIAN();
    clickExtensionsUse();
}

void MakeCRLProfileDlg::setTableMenus()
{
    QStringList sDPNLabels = { tr("Type"), tr("Value") };
    mIDPTable->setColumnCount(2);
    mIDPTable->horizontalHeader()->setStretchLastSection(true);
    mIDPTable->setHorizontalHeaderLabels(sDPNLabels);
    mIDPTable->verticalHeader()->setVisible(false);
    mIDPTable->horizontalHeader()->setStyleSheet( kTableStyle );
    mIDPTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mIDPTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mIDPTable->setColumnWidth(0, 60);

    QStringList sIANLabels = { tr("Type"), tr("Value") };
    mIANTable->setColumnCount(2);
    mIANTable->horizontalHeader()->setStretchLastSection(true);
    mIANTable->setHorizontalHeaderLabels(sIANLabels);
    mIANTable->verticalHeader()->setVisible(false);
    mIANTable->horizontalHeader()->setStyleSheet( kTableStyle );
    mIANTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mIANTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mIANTable->setColumnWidth(0, 60);

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

void MakeCRLProfileDlg::slotIANMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    QAction *delAct = new QAction( tr("Delete"), this );
    connect( delAct, SIGNAL(triggered()), this, SLOT(deleteIANMenu()));

    menu->addAction( delAct );
    menu->popup( mIANTable->viewport()->mapToGlobal(pos));
}

void MakeCRLProfileDlg::slotIDPMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    QAction *delAct = new QAction( tr("Delete"), this );
    connect( delAct, SIGNAL(triggered()), this, SLOT(deleteIDPMenu()));

    menu->addAction( delAct );
    menu->popup( mIDPTable->viewport()->mapToGlobal(pos));
}

void MakeCRLProfileDlg::deleteIANMenu()
{
    QModelIndex idx = mIANTable->currentIndex();

    mIANTable->removeRow( idx.row() );
}

void MakeCRLProfileDlg::deleteIDPMenu()
{
    QModelIndex idx = mIDPTable->currentIndex();

    mIDPTable->removeRow( idx.row() );
}

void MakeCRLProfileDlg::slotExtensionsMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    QAction *delAct = new QAction( tr("Delete"), this );
    connect( delAct, SIGNAL(triggered()), this, SLOT(deleteExtensionsMenu()));

    menu->addAction( delAct );
    menu->popup( mExtensionsTable->viewport()->mapToGlobal(pos));
}

void MakeCRLProfileDlg::deleteExtensionsMenu()
{
    QModelIndex idx = mExtensionsTable->currentIndex();
    mExtensionsTable->removeRow( idx.row() );
}


void MakeCRLProfileDlg::clickCRLNum()
{
    bool bStatus = mCRLNumGroup->isChecked();

    mCRLNumCriticalCheck->setEnabled(bStatus);
    mCRLNumText->setEnabled(bStatus);
    mCRLNumAutoCheck->setEnabled(bStatus);
}

void MakeCRLProfileDlg::clickAKI()
{
    bool bStatus = mAKIGroup->isChecked();

    mAKICriticalCheck->setEnabled(bStatus);
    mAKICertIssuerCheck->setEnabled(bStatus);
    mAKICertSerialCheck->setEnabled(bStatus);
}

void MakeCRLProfileDlg::clickIDP()
{
    bool bStatus = mIDPGroup->isChecked();

    mIDPCriticalCheck->setEnabled(bStatus);
    mIDPClearBtn->setEnabled(bStatus);
    mIDPAddBtn->setEnabled(bStatus);
    mIDPText->setEnabled(bStatus);
    mIDPTable->setEnabled(bStatus);
    mIDPCombo->setEnabled(bStatus);
}

void MakeCRLProfileDlg::clickIAN()
{
    bool bStatus = mIANGroup->isChecked();

    mIANCriticalCheck->setEnabled(bStatus);
    mIANText->setEnabled(bStatus);
    mIANCombo->setEnabled(bStatus);
    mIANTable->setEnabled(bStatus);
    mIANClearBtn->setEnabled(bStatus);
    mIANAddBtn->setEnabled(bStatus);
}

void MakeCRLProfileDlg::clickUseFromNow()
{
    bool bStatus = mUseFromNowCheck->isChecked();

    mValidDaysText->setEnabled( bStatus );
    mLastUpdateDateTime->setEnabled( !bStatus );
    mNextUpdateDateTime->setEnabled( !bStatus );
}

void MakeCRLProfileDlg::clickExtensionsUse()
{
    bool bStatus = mExtensionsGroup->isChecked();

    mExtensionsAddBtn->setEnabled(bStatus);
    mExtensionsClearBtn->setEnabled(bStatus);
    mExtensionsOIDText->setEnabled(bStatus);
    mExtensionsCriticalCheck->setEnabled(bStatus);
    mExtensionsValueText->setEnabled(bStatus);
    mExtensionsTable->setEnabled(bStatus);
}

void MakeCRLProfileDlg::addIDP()
{
    QString strType = mIDPCombo->currentText();
    QString strVal = mIDPText->text();

    int row = mIDPTable->rowCount();
    mIDPTable->setRowCount( row + 1 );

    mIDPTable->setRowHeight( row, 10 );
    mIDPTable->setItem( row, 0, new QTableWidgetItem( strType ));
    mIDPTable->setItem( row, 1, new QTableWidgetItem( strVal ));
}

void MakeCRLProfileDlg::addIAN()
{
    QString strType = mIANCombo->currentText();
    QString strVal = mIANText->text();

    int row = mIANTable->rowCount();
    mIANTable->setRowCount( row + 1 );

    mIANTable->setRowHeight( row, 10 );
    mIANTable->setItem( row, 0, new QTableWidgetItem( strType ));
    mIANTable->setItem( row, 1, new QTableWidgetItem( strVal ));
}

void MakeCRLProfileDlg::addExtensions()
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

void MakeCRLProfileDlg::clearIDP()
{
    int nCnt = mIDPTable->rowCount();

    for( int i=0; i < nCnt; i++)
        mIDPTable->removeRow(0);
}

void MakeCRLProfileDlg::clearIAN()
{
    int nCnt = mIANTable->rowCount();

    for( int i=0; i < nCnt; i++)
        mIANTable->removeRow(0);
}

void MakeCRLProfileDlg::clearExtensions()
{
    int nCount = mExtensionsTable->rowCount();

    for( int i = 0; i < nCount; i++ )
        mExtensionsTable->removeRow(0);
}

void MakeCRLProfileDlg::saveCRLNumUse( int nProfileNum )
{
    JCC_ProfileExt   sProfileExt;
    memset( &sProfileExt, 0x00, sizeof(sProfileExt));

    QString strVal;

    if( mCRLNumAutoCheck->isChecked() )
        strVal = "auto";
    else {
        strVal = mCRLNumText->text();
    }

    JS_DB_setProfileExt( &sProfileExt, -1, nProfileNum, mCRLNumCriticalCheck->isChecked(), "crlNumber", strVal.toStdString().c_str() );

    manApplet->ccClient()->addCRLProfileExt( nProfileNum, &sProfileExt );
    JS_DB_resetProfileExt( &sProfileExt );
}

void MakeCRLProfileDlg::saveAKIUse( int nProfileNum )
{
    JCC_ProfileExt   sProfileExt;
    memset( &sProfileExt, 0x00, sizeof(sProfileExt));

    QString strVal;

    if( mAKICertIssuerCheck->isChecked() )
        strVal += "ISSUER#";

    if( mAKICertSerialCheck->isChecked() )
        strVal += "SERIAL#";

    JS_DB_setProfileExt( &sProfileExt, -1, nProfileNum, mAKICriticalCheck->isChecked(), "authorityKeyIdentifier", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCRLProfileExt( nProfileNum, &sProfileExt );
    JS_DB_resetProfileExt( &sProfileExt );
}

void MakeCRLProfileDlg::saveIDPUse( int nProfileNum )
{
    JCC_ProfileExt sProfileExt;
    memset( &sProfileExt, 0x00, sizeof(sProfileExt));

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

    JS_DB_setProfileExt( &sProfileExt, -1, nProfileNum, mIDPCriticalCheck->isChecked(), "issuingDistributionPoint", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCRLProfileExt( nProfileNum, &sProfileExt );
    JS_DB_resetProfileExt( &sProfileExt );
}

void MakeCRLProfileDlg::saveIANUse( int nProfileNum )
{
    JCC_ProfileExt   sProfileExt;
    memset( &sProfileExt, 0x00, sizeof(sProfileExt));

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

    JS_DB_setProfileExt( &sProfileExt, -1, nProfileNum, mIANCriticalCheck->isChecked(), "issuerAltName", strVal.toStdString().c_str() );
    manApplet->ccClient()->addCRLProfileExt( nProfileNum, &sProfileExt );
    JS_DB_resetProfileExt( &sProfileExt );
}

void MakeCRLProfileDlg::saveExtensionsUse( int nProfileNum )
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
        manApplet->ccClient()->addCRLProfileExt( nProfileNum, &sProfileExt );
        JS_DB_resetProfileExt( &sProfileExt );
    }
}

void MakeCRLProfileDlg::setCRLNumUse( JCC_ProfileExt *pProfileExt )
{
    mCRLNumGroup->setChecked(true);
    mCRLNumCriticalCheck->setChecked( pProfileExt->bCritical );
    clickCRLNum();

    QString strVal = pProfileExt->pValue;

    if( strVal == "auto" )
        mCRLNumAutoCheck->setChecked(true);
    else
        mCRLNumText->setText( strVal );
}

void MakeCRLProfileDlg::setAKIUse( JCC_ProfileExt *pProfileExt )
{
    mAKIGroup->setChecked(true);
    mAKICriticalCheck->setChecked( pProfileExt->bCritical );
    clickAKI();

    QString strVal = pProfileExt->pValue;

    mAKICertIssuerCheck->setChecked( strVal.contains("ISSUER") );
    mAKICertSerialCheck->setChecked( strVal.contains("SERIAL") );
}

void MakeCRLProfileDlg::setIDPUse( JCC_ProfileExt *pProfileExt )
{
    mIDPGroup->setChecked(true);
    mIDPCriticalCheck->setChecked( pProfileExt->bCritical );
    clickIDP();

    QString strVal = pProfileExt->pValue;

    QStringList valList = strVal.split("#");
    for( int i=0; i < valList.size(); i++ )
    {
        QString info = valList.at(i);
        QString strType;
        QString strData;

        QStringList infoList = info.split("$");
        if( infoList.size() > 0)
            strType = infoList.at(0);

        if( infoList.size() > 1)
            strData = infoList.at(1);

        mIDPTable->insertRow(i);
        mIDPTable->setItem(i, 0, new QTableWidgetItem(strType));
        mIDPTable->setItem(i, 1, new QTableWidgetItem(strData));
    }
}

void MakeCRLProfileDlg::setIANUse( JCC_ProfileExt *pProfileExt )
{
    mIANGroup->setChecked(true);
    mIANCriticalCheck->setChecked( pProfileExt->bCritical );
    clickIAN();

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
        mIANTable->setItem( i, 1, new QTableWidgetItem(strData));
    }
}

void MakeCRLProfileDlg::setExtensionsUse( JCC_ProfileExt *pProfileExt )
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

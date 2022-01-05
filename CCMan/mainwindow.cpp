#include <QToolBar>
#include <QTableWidget>
#include <QtWidgets>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "man_applet.h"

#include "man_tree_item.h"
#include "man_tree_model.h"
#include "man_tree_view.h"
#include "man_right_widget.h"
#include "search_menu.h"
#include "reg_user_dlg.h"
#include "cc_client.h"
#include "settings_dlg.h"
#include "about_dlg.h"
#include "settings_mgr.h"
#include "cert_info_dlg.h"
#include "crl_info_dlg.h"

#include "js_db.h"
#include "js_db_data.h"
#include "make_cert_profile_dlg.h"
#include "make_crl_profile_dlg.h"
#include "signer_dlg.h"
#include "revoke_cert_dlg.h"
#include "issue_cert_dlg.h"
#include "issue_crl_dlg.h"
#include "js_pki.h"
#include "js_pki_x509.h"
#include "js_pki_ext.h"
#include "common.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initialize();

    createActions();
    createStatusBar();
    createTreeMenu();

    setUnifiedTitleAndToolBarOnMac(true);
}

MainWindow::~MainWindow()
{

}

void MainWindow::showWindow()
{
    showNormal();
    show();
    raise();
    activateWindow();
}

void MainWindow::initialize()
{
    hsplitter_ = new QSplitter(Qt::Horizontal);
    vsplitter_ = new QSplitter(Qt::Vertical );

    left_tree_ = new ManTreeView(this);
    left_model_ = new ManTreeModel(this);
    right_menu_ = new SearchMenu;
    log_text_ = new QTextEdit;

    right_table_ = new ManRightWidget;

    right_menu_->setMaximumHeight(20);
    left_tree_->setModel(left_model_);

    log_text_->setFont( QFont("굴림체") );
    log_text_->setReadOnly(true);

    right_table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    right_table_->setEditTriggers(QAbstractItemView::NoEditTriggers);

    hsplitter_->addWidget(left_tree_);
    hsplitter_->addWidget(vsplitter_);

    vsplitter_->addWidget(right_table_);
    vsplitter_->addWidget(right_menu_);
    vsplitter_->addWidget(log_text_);

    QList<int> vsizes;
    vsizes << 1200 << 10 << 500;
    vsplitter_->setSizes(vsizes);

    QList<int> hsizes;
    hsizes << 500 << 1200;
    hsplitter_->setSizes( hsizes );
    resize(1024, 768);

    setCentralWidget(hsplitter_);

    connect( left_tree_, SIGNAL(clicked(QModelIndex)), this, SLOT(treeMenuClick(QModelIndex)));
    connect( right_table_, SIGNAL(clicked(QModelIndex)), this, SLOT(rightTableClick(QModelIndex)));

    right_table_->setContextMenuPolicy(Qt::CustomContextMenu);
    connect( right_table_, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showRightMenu(QPoint)));
}

void MainWindow::log( const QString strLog, QColor cr )
{
    QTextCursor cursor = log_text_->textCursor();

    QTextCharFormat format;
    format.setForeground( cr );
    cursor.mergeCharFormat(format);

    cursor.insertText( strLog );

    log_text_->setTextCursor( cursor );
    log_text_->repaint();
}

void MainWindow::createActions()
{
    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));
    QToolBar *toolsToolBar = addToolBar(tr("Tools"));

    const QIcon regIcon = QIcon::fromTheme( "reg-user", QIcon(":/images/user_add.png"));
    QAction *regUserAct = new QAction( regIcon, tr("&RegisterUser"), this );
    regUserAct->setStatusTip( tr("Register a user"));
    connect( regUserAct, &QAction::triggered, this, &MainWindow::regUser);
    toolsMenu->addAction(regUserAct);
    toolsToolBar->addAction(regUserAct);

    QAction *makeCertProfileAct = toolsMenu->addAction(tr("&MakeCertProfile"), this, &MainWindow::makeCertProfile);
    makeCertProfileAct->setStatusTip(tr( "Make certificate profile"));

    QAction *makeCRLProfileAct = toolsMenu->addAction(tr("&MakeCRLProfile"), this, &MainWindow::makeCRLProfile);
    makeCRLProfileAct->setStatusTip(tr( "Make CRL Profile"));

    QAction *regSignerAct = toolsMenu->addAction(tr("&RegSigner"), this, &MainWindow::regSigner);
    regSignerAct->setStatusTip(tr("Register Signer"));

    QAction *issueCertAct = toolsMenu->addAction(tr("&IssueCert"), this, &MainWindow::issueCert);
    issueCertAct->setStatusTip(tr("Issue certificate") );

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QToolBar *helpToolBar = addToolBar(tr("Help"));

    QAction *aboutAct = helpMenu->addAction(tr("About"), this, &MainWindow::about );
    aboutAct->setStatusTip( tr("About CCMan"));

    QAction *settingsAct = helpMenu->addAction(tr("Settings"), this, &MainWindow::settings );
    settingsAct->setStatusTip(tr("Settings CCMan"));
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createTableMenu()
{

}

void MainWindow::showRightMenu(QPoint point)
{
//    QTableWidgetItem* item = right_table_->itemAt(point);

    QMenu menu(this);

    if( right_table_->type() == ITEM_TYPE_USER )
    {
        menu.addAction( tr("DeleteUser"), this, &MainWindow::deleteUser );
        menu.addAction( tr("ModifyUser"), this, &MainWindow::modifyUser );
    }
    else if( right_table_->type() == ITEM_TYPE_CERT_PROFILE )
    {
        menu.addAction( tr("ModifyCertProfile"), this, &MainWindow::modifyCertProfile );
        menu.addAction( tr("DeleteCertProfile"), this, &MainWindow::deleteCertProfile );
    }
    else if( rightType() == ITEM_TYPE_CRL_PROFILE )
    {
        menu.addAction( tr("ModifyCRLProfile"), this, &MainWindow::modifyCRLProfile );
        menu.addAction( tr("DeleteCRLProfile"), this, &MainWindow::deleteCRLProfile );
    }
    else if( rightType() == ITEM_TYPE_REG_SIGNER || rightType() == ITEM_TYPE_OCSP_SIGNER )
    {
        menu.addAction( tr("DeleteSigner"), this, &MainWindow::deleteSigner );
    }
    else if( rightType() == ITEM_TYPE_CERT )
    {
        menu.addAction( tr("RevokeCert"), this, &MainWindow::revokeCert );
        menu.addAction( tr("PublishCert"), this, &MainWindow::publishLDAP );
        menu.addAction( tr("CertInfo"), this, &MainWindow::certInfo );
        menu.addAction( tr("CertStatus"), this, &MainWindow::certStatus );
    }
    else if( rightType() == ITEM_TYPE_CRL )
    {
        menu.addAction( tr("PublishCRL"), this, &MainWindow::publishLDAP );
        menu.addAction( tr("CRLInfo"), this, &MainWindow::crlInfo );
    }
    else if( rightType() == ITEM_TYPE_REVOKE )
    {
        menu.addAction( tr("DeleteRevoke"), this, &MainWindow::deleteRevoke );
    }

    menu.exec(QCursor::pos());
}

void MainWindow::treeMenuClick(QModelIndex index )
{
    int nType = -1;

    ManTreeItem *pItem = (ManTreeItem *)left_model_->itemFromIndex(index);

    if( pItem == NULL ) return;

    right_menu_->setCurPage(0);

    nType = pItem->type();
    right_table_->setType(nType);

    createRightList( nType );
}

void MainWindow::rightTableClick(QModelIndex index )
{
    int row = index.row();
    int col = index.column();

    QString strVal;

    strVal = QString( "row: %1 column %2").arg(row).arg(col);
    QTableWidgetItem* item = right_table_->item(row, 0);

    int nSeq = item->text().toInt();
    int nType = right_table_->type();

    log_text_->setText( strVal );

    if( nType == ITEM_TYPE_ADMIN )
        logAdmin( nSeq );
    else if( nType == ITEM_TYPE_USER )
        logUser( nSeq );
    else if( nType == ITEM_TYPE_CERT_PROFILE )
        logCertProfile( nSeq );
    else if( nType == ITEM_TYPE_CRL_PROFILE )
        logCRLProfile( nSeq );
    else if( nType == ITEM_TYPE_REG_SIGNER || nType == ITEM_TYPE_OCSP_SIGNER )
        logSigner( nSeq );
    else if( nType == ITEM_TYPE_CERT )
        logCert( nSeq );
    else if( nType == ITEM_TYPE_CRL )
        logCRL( nSeq );
    else if( nType == ITEM_TYPE_REVOKE )
        logRevoked( nSeq );
    else if( nType == ITEM_TYPE_CA )
        logCA( row );
}

void MainWindow::logAdmin( int nSeq )
{
    QString strMsg;
    QString strPart;

    JCC_Admin sAdmin;
    memset( &sAdmin, 0x00, sizeof(sAdmin));

    manApplet->ccClient()->getAdmin( nSeq, &sAdmin );

    logClear();

    manApplet->log( "========================================================================\n" );
    manApplet->log( "== Admin Information\n" );
    manApplet->log( "========================================================================\n" );
    manApplet->log( QString("Seq          : %1\n").arg(sAdmin.nSeq));
    manApplet->log( QString("Status       : %1\n").arg(sAdmin.nStatus));
    manApplet->log( QString("Type         : %1\n").arg(sAdmin.nType));
    manApplet->log( QString("Name         : %1\n").arg(sAdmin.pName));
    manApplet->log( QString("Password     : %1\n").arg(sAdmin.pPassword));
    manApplet->log( QString("Email        : %1\n").arg(sAdmin.pEmail));

    logCursorTop();
    JS_DB_resetAdmin( &sAdmin );
}

void MainWindow::logUser( int nSeq )
{
    QString strMsg;
    QString strPart;

    JCC_User sUser;
    memset( &sUser, 0x00, sizeof(sUser));

    char sRegTime[64];

    manApplet->ccClient()->getUser( nSeq, &sUser );

    logClear();

    manApplet->log( "========================================================================\n" );
    manApplet->log( "== User Information\n" );
    manApplet->log( "========================================================================\n" );
    manApplet->log( QString("Num           : %1\n").arg(sUser.nNum));
    manApplet->log( QString("RegTime       : %1\n").arg(getDateTime(sUser.nRegTime)));
    manApplet->log( QString("Name          : %1\n").arg(sUser.pName));
    manApplet->log( QString("SSN           : %1\n").arg(sUser.pSSN));
    manApplet->log( QString("Email         : %1\n").arg(sUser.pEmail));
    manApplet->log( QString("Status        : %1\n").arg(sUser.nStatus));
    manApplet->log( QString("RefNum        : %1\n").arg(sUser.pRefNum));
    manApplet->log( QString("AuthCode      : %1\n").arg(sUser.pAuthCode));

    logCursorTop();
    JS_DB_resetUser( &sUser );
/*
    strMsg = "[ User information ]\n";

    strPart = QString( "Num: %1\n").arg( sUser.nNum);
    strMsg += strPart;

    JS_UTIL_getDateTime( sUser.nRegTime, sRegTime );
    strPart = QString( "RegTime: %1\n").arg(sRegTime);
    strMsg += strPart;

    strPart = QString( "Name: %1\n").arg( sUser.pName );
    strMsg += strPart;

    strPart = QString( "SSN: %1\n").arg( sUser.pSSN );
    strMsg += strPart;

    strPart = QString( "Email: %1\n").arg( sUser.pEmail );
    strMsg += strPart;

    strPart = QString( "Status: %1\n").arg( sUser.nStatus );
    strMsg += strPart;

    strPart = QString( "RefNum: %1\n").arg( sUser.pRefNum );
    strMsg += strPart;

    strPart = QString( "AuthCode: %1\n").arg( sUser.pAuthCode );
    strMsg += strPart;

    JS_DB_resetUser( &sUser );
    log_text_->setText( strMsg );
    */
}

void MainWindow::logCertProfile( int nNum )
{
    QString strMsg;
    QString strPart;

    JCC_CertProfile  sCertProfile;
    JCC_ProfileExtList  *pProfileExtList = NULL;
    JCC_ProfileExtList *pCurList = NULL;

    memset( &sCertProfile, 0x00, sizeof(sCertProfile));
    manApplet->ccClient()->getCertProfile( nNum, &sCertProfile );


    logClear();
    manApplet->log( "========================================================================\n" );
    manApplet->log( "== Certificate Profile Information\n" );
    manApplet->log( "========================================================================\n" );
    manApplet->log( QString("Num         : %1\n").arg(sCertProfile.nNum));
    manApplet->log( QString("Name        : %1\n").arg(sCertProfile.pName));
    manApplet->log( QString("Version     : %1\n").arg(sCertProfile.nVersion));
    manApplet->log( QString("NotBefore   : %1\n").arg(sCertProfile.nNotBefore));
    manApplet->log( QString("NotAfter    : %1\n").arg(sCertProfile.nNotAfter));
    manApplet->log( QString("Hash        : %1\n").arg(sCertProfile.pHash));
    manApplet->log( QString("DNTemplate  : %1\n").arg(sCertProfile.pDNTemplate));
    manApplet->log( "======================= Extension Information ==========================\n" );

    manApplet->ccClient()->getCertProfileExtList( nNum, &pProfileExtList );
    pCurList = pProfileExtList;

    while( pCurList )
    {
        manApplet->log( QString( "%1 || %2 || %3 || %4\n")
                .arg(pCurList->sProfileExt.nSeq)
                .arg(pCurList->sProfileExt.bCritical )
                .arg(pCurList->sProfileExt.pSN)
                .arg(pCurList->sProfileExt.pValue) );

        pCurList = pCurList->pNext;
    }


    logCursorTop();
    JS_DB_resetCertProfile( &sCertProfile );
    if( pProfileExtList ) JS_DB_resetProfileExtList( &pProfileExtList );
}

void MainWindow::logCRLProfile( int nNum )
{
    QString strMsg;
    QString strPart;

    JCC_CRLProfile   sCRLProfile;
    JCC_ProfileExtList  *pProfileExtList = NULL;
    JCC_ProfileExtList  *pCurList = NULL;

    memset( &sCRLProfile, 0x00, sizeof(sCRLProfile));
    manApplet->ccClient()->getCRLProfile( nNum, &sCRLProfile );

    logClear();
    manApplet->log( "========================================================================\n" );
    manApplet->log( "== CRL Profile Information\n" );
    manApplet->log( "========================================================================\n" );
    manApplet->log( QString("Num          : %1\n").arg(sCRLProfile.nNum));
    manApplet->log( QString("Name         : %1\n").arg(sCRLProfile.pName));
    manApplet->log( QString("Version      : %1\n").arg(sCRLProfile.nVersion));
    manApplet->log( QString("LastUpdate   : %1\n").arg(sCRLProfile.nLastUpdate));
    manApplet->log( QString("NextUpdate   : %1\n").arg(sCRLProfile.nNextUpdate));
    manApplet->log( QString("Hash         : %1\n").arg(sCRLProfile.pHash));
    manApplet->log( "======================= Extension Information ==========================\n" );

    manApplet->ccClient()->getCRLProfileExtList( nNum, &pProfileExtList );
    pCurList = pProfileExtList;

    while( pCurList )
    {
        manApplet->log( QString( "%1 || %2 || %3 || %4\n")
                .arg(pCurList->sProfileExt.nSeq)
                .arg(pCurList->sProfileExt.bCritical )
                .arg(pCurList->sProfileExt.pSN)
                .arg(pCurList->sProfileExt.pValue) );

        pCurList = pCurList->pNext;
    }

    logCursorTop();

    JS_DB_resetCRLProfile( &sCRLProfile );
    if( pProfileExtList ) JS_DB_resetProfileExtList( &pProfileExtList );
}

void MainWindow::logSigner(int nNum)
{
    QString strMsg;
    QString strPart;

    JCC_Signer  sSigner;
    memset( &sSigner, 0x00, sizeof(sSigner));

    char    sRegTime[64];

    manApplet->ccClient()->getSigner( nNum, &sSigner );

    manApplet->mainWindow()->logClear();
    manApplet->log( "========================================================================\n" );
    manApplet->log( "== Signer Information\n" );
    manApplet->log( "========================================================================\n" );
    manApplet->log( QString("Num          : %1\n").arg( sSigner.nNum));
    manApplet->log( QString("RegTime      : %1\n").arg(getDateTime(sSigner.nRegTime)));
    manApplet->log( QString("Type         : %1\n").arg( sSigner.nType ));
    manApplet->log( QString("DN           : %1\n").arg( sSigner.pDN));
    manApplet->log( QString("DNHash       : %1\n").arg( sSigner.pDNHash));
    manApplet->log( QString("Cert         : %1\n").arg( sSigner.pCert ));
    manApplet->log( QString("Status       : %1\n").arg( sSigner.nStatus));
    manApplet->log( QString("Desc         : %1\n").arg( sSigner.pDesc ));

    logCursorTop();
    JS_DB_resetSigner( &sSigner );
}

void MainWindow::logCert( int nNum )
{
    QString strMsg;
    QString strPart;

    JCC_Cert    sCert;
    memset( &sCert, 0x00, sizeof(sCert));

    char    sRegDate[64];

    manApplet->ccClient()->getCert( nNum, &sCert );

    manApplet->mainWindow()->logClear();
    manApplet->log( "========================================================================\n" );
    manApplet->log( "== Certificate Information\n" );
    manApplet->log( "========================================================================\n" );
    manApplet->log( QString("Num           : %1\n").arg(sCert.nNum));
    JS_UTIL_getDateTime( sCert.nRegTime, sRegDate );
    manApplet->log( QString("RegDate       : %1\n").arg(sRegDate));
    manApplet->log( QString("KeyNum        : %1\n").arg(sCert.nKeyNum));
    manApplet->log( QString("UserNum       : %1\n").arg(sCert.nUserNum ));
    manApplet->log( QString("SignAlgorithm : %1\n").arg(sCert.pSignAlg));
    manApplet->log( QString("Certificate   : %1\n").arg(sCert.pCert));
    manApplet->log( QString("IsCA          : %1\n").arg(sCert.bCA));
    manApplet->log( QString("IsSelf        : %1\n").arg(sCert.bSelf));
    manApplet->log( QString("SubjectDN     : %1\n").arg(sCert.pSubjectDN));
    manApplet->log( QString("IssuerNum     : %1\n").arg(sCert.nIssuerNum));
    manApplet->log( QString("Status        : %1\n").arg(sCert.nStatus));
    manApplet->log( QString("Serial        : %1\n").arg(sCert.pSerial));
    manApplet->log( QString("DNHash        : %1\n").arg(sCert.pDNHash));
    manApplet->log( QString("KeyHash       : %1\n").arg(sCert.pKeyHash));
    manApplet->log( QString("CRLDP         : %1\n").arg(sCert.pCRLDP));

    logCursorTop();
    JS_DB_resetCert( &sCert );
}

void MainWindow::logCRL( int nNum )
{
    QString strMsg;
    QString strPart;

    JCC_CRL sCRL;
    memset( &sCRL, 0x00, sizeof(sCRL));

    char    sRegTime[64];

    manApplet->ccClient()->getCRL( nNum, &sCRL );

    manApplet->mainWindow()->logClear();
    manApplet->log( "========================================================================\n" );
    manApplet->log( "== CRL Information\n" );
    manApplet->log( "========================================================================\n" );
    manApplet->log( QString("Num           : %1\n").arg(sCRL.nNum));
    JS_UTIL_getDateTime( sCRL.nRegTime, sRegTime );
    manApplet->log( QString("RegTime       : %1\n").arg(sRegTime));
    manApplet->log( QString("IssuerNum     : %1\n").arg(sCRL.nIssuerNum));
    manApplet->log( QString("SignAlgorithm : %1\n").arg(sCRL.pSignAlg));
    manApplet->log( QString("CRLDP         : %1\n").arg(sCRL.pCRLDP));
    manApplet->log( QString("CRL           : %1\n").arg(sCRL.pCRL));

    logCursorTop();
    JS_DB_resetCRL( &sCRL );
}

void MainWindow::logRevoked( int nSeq )
{
    QString strMsg;
    QString strPart;

    JCC_Revoked sRevoked;
    memset( &sRevoked, 0x00, sizeof(sRevoked));

    manApplet->ccClient()->getRevoked( nSeq, &sRevoked );

    manApplet->mainWindow()->logClear();
    manApplet->log( "========================================================================\n" );
    manApplet->log( "== Revoke Information\n" );
    manApplet->log( "========================================================================\n" );
    manApplet->log( QString("Seq          : %1\n").arg( sRevoked.nSeq));
    manApplet->log( QString("CertNum      : %1\n").arg( sRevoked.nCertNum));
    manApplet->log( QString("IssuerNum    : %1\n").arg( sRevoked.nIssuerNum));
    manApplet->log( QString("Serial       : %1\n").arg( sRevoked.pSerial));
    manApplet->log( QString("RevokeDate   : %1\n").arg( getDateTime( sRevoked.nRevokedDate )));
    manApplet->log( QString("Reason       : %1\n").arg( sRevoked.nReason));
    manApplet->log( QString("CRLDP        : %1\n").arg( sRevoked.pCRLDP ));

    logCursorTop();
    JS_DB_resetRevoked( &sRevoked );
}

void MainWindow::logCA( int row )
{
    QString strVal;
    QString strPart;

    QTableWidgetItem* leftItem = right_table_->item(row, 0);
    QTableWidgetItem* rightItem = right_table_->item(row, 1);

    strVal = leftItem->text();
    strVal += "\n\n";
    strVal += rightItem->text();

    log_text_->setText( strVal );
}

void MainWindow::createTreeMenu()
{
    left_model_->clear();
    left_tree_->header()->setVisible(false);

    ManTreeItem *pRootItem = (ManTreeItem *)left_model_->invisibleRootItem();


    ManTreeItem *pTopItem = new ManTreeItem( QString( "CertificateAuthority" ) );
    pTopItem->setIcon(QIcon(":/images/man.png"));
    pRootItem->insertRow( 0, pTopItem );

    ManTreeItem *pAdminItem = new ManTreeItem( QString("Admin") );
    pAdminItem->setIcon(QIcon(":/images/admin.png"));
    pAdminItem->setType( ITEM_TYPE_ADMIN );
    pTopItem->appendRow( pAdminItem );

    ManTreeItem *pUserItem = new ManTreeItem( QString("User") );
    pUserItem->setIcon(QIcon(":/images/user.jpg"));
    pUserItem->setType( ITEM_TYPE_USER );
    pTopItem->appendRow( pUserItem );

    ManTreeItem *pCertProfileItem = new ManTreeItem( QString("CertProfile"));
    pCertProfileItem->setIcon( QIcon(":/images/policy.png"));
    pCertProfileItem->setType( ITEM_TYPE_CERT_PROFILE );
    pTopItem->appendRow( pCertProfileItem );

    ManTreeItem *pCRLProfileItem = new ManTreeItem( QString("CRLProfile"));
    pCRLProfileItem->setIcon( QIcon(":/images/policy.png"));
    pCRLProfileItem->setType( ITEM_TYPE_CRL_PROFILE );
    pTopItem->appendRow( pCRLProfileItem );

    ManTreeItem *pRegSignerItem = new ManTreeItem( QString("RegSigner"));
    pRegSignerItem->setIcon( QIcon(":/images/reg_signer.png"));
    pRegSignerItem->setType( ITEM_TYPE_REG_SIGNER );
    pTopItem->appendRow( pRegSignerItem );

    ManTreeItem *pOCSPSignerItem = new ManTreeItem( QString("OCSPSigner"));
    pOCSPSignerItem->setIcon( QIcon(":/images/ocsp_signer.png"));
    pOCSPSignerItem->setType( ITEM_TYPE_OCSP_SIGNER );
    pTopItem->appendRow( pOCSPSignerItem );

    ManTreeItem *pCAItem = new ManTreeItem( QString( "CA" ) );
    pCAItem->setType( ITEM_TYPE_CA );
    pCAItem->setIcon( QIcon(":/images/ca.png"));
    pTopItem->appendRow( pCAItem );

    ManTreeItem *pCertItem = new ManTreeItem( QString("Certificate"));
    pCertItem->setType( ITEM_TYPE_CERT );
    pCertItem->setIcon(QIcon(":/images/cert.png"));
    pTopItem->appendRow( pCertItem );

    ManTreeItem *pCRLItem = new ManTreeItem( QString("CRL") );
    pCRLItem->setType( ITEM_TYPE_CRL );
    pCRLItem->setIcon(QIcon(":/images/crl.png"));
    pTopItem->appendRow( pCRLItem );

    ManTreeItem *pRevokeItem = new ManTreeItem( QString("Revoke"));
    pRevokeItem->setType( ITEM_TYPE_REVOKE );
    pRevokeItem->setIcon(QIcon(":/images/revoke.png"));
    pTopItem->appendRow( pRevokeItem );

    QModelIndex ri = left_model_->index(0,0);
    left_tree_->expand(ri);
}

void MainWindow::createRightList(int nItemType)
{
    if( nItemType == ITEM_TYPE_CERT_PROFILE ||
            nItemType == ITEM_TYPE_CRL_PROFILE ||
            nItemType == ITEM_TYPE_REG_SIGNER ||
            nItemType == ITEM_TYPE_OCSP_SIGNER )
    {
        right_menu_->hide();
    }
    else
    {
        right_menu_->show();
    }

    if( nItemType == ITEM_TYPE_ADMIN )
        createRightAdminList();
    else if( nItemType == ITEM_TYPE_USER )
        createRightUserList();
    else if( nItemType == ITEM_TYPE_CERT_PROFILE )
        createRightCertProfileList();
    else if( nItemType == ITEM_TYPE_CRL_PROFILE )
        createRightCRLProfileList();
    else if( nItemType == ITEM_TYPE_REG_SIGNER || nItemType == ITEM_TYPE_OCSP_SIGNER )
        createRightSignerList( nItemType );
    else if( nItemType == ITEM_TYPE_CERT )
        createRightCertList();
    else if( nItemType == ITEM_TYPE_CRL )
        createRightCRLList();
    else if( nItemType == ITEM_TYPE_REVOKE )
        createRightRevokedList();
    else if( nItemType == ITEM_TYPE_CA )
        createRightCA();
}

void MainWindow::createRightAdminList()
{
    int i = 0;
    removeAllRight();
    JDB_AdminList  *pAdminList = NULL;
    JDB_AdminList  *pCurList = NULL;

    QStringList titleList = { tr("Seq"), tr("Status"), tr("Type"), tr("Name"), tr("Password"), tr("Email") };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setType( ITEM_TYPE_ADMIN );

    right_table_->setColumnCount(titleList.size());
    right_table_->setHorizontalHeaderLabels( titleList );
    right_table_->verticalHeader()->setVisible(false);

    manApplet->ccClient()->getAdminList( &pAdminList );
    pCurList = pAdminList;

    while( pCurList )
    {
        right_table_->insertRow(i);

        right_table_->setRowHeight(i, 10 );
        right_table_->setItem( i, 0, new QTableWidgetItem( QString("%1").arg( pCurList->sAdmin.nSeq ) ));
        right_table_->setItem( i, 1, new QTableWidgetItem( QString("%1").arg( pCurList->sAdmin.nStatus ) ));
        right_table_->setItem( i, 2, new QTableWidgetItem( QString("%1").arg( pCurList->sAdmin.nType )));
        right_table_->setItem( i, 3, new QTableWidgetItem( QString("%1").arg( pCurList->sAdmin.pName )));
        right_table_->setItem( i, 4, new QTableWidgetItem( QString("%1").arg( pCurList->sAdmin.pPassword )));
        right_table_->setItem( i, 5, new QTableWidgetItem( pCurList->sAdmin.pEmail ));

        pCurList = pCurList->pNext;
        i++;
    }

    if( pAdminList ) JS_DB_resetAdminList( &pAdminList );
}

void MainWindow::createRightUserList()
{
    int i = 0;
    int nLimit = manApplet->settingsMgr()->listCount();
    int nPage = right_menu_->curPage();
    int nOffset = nPage * nLimit;
    int nTotalCnt = manApplet->ccClient()->getCount( ITEM_TYPE_USER );

    right_menu_->setLimit( nLimit );
    right_menu_->setTotalCount( nTotalCnt );


    JDB_UserList    *pDBUserList = NULL;
    JDB_UserList    *pCurList = NULL;

    removeAllRight();

    QStringList titleList = { "Num", "RegTime", "Name", "SSN", "Email", "Status", "RefNum", "SecretCode" };


    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setType( ITEM_TYPE_USER );

    right_table_->setColumnCount(titleList.size());
    right_table_->setHorizontalHeaderLabels(titleList);
    right_table_->verticalHeader()->setVisible(false);

    manApplet->ccClient()->getUserList( nOffset, nLimit, &pDBUserList );
    pCurList = pDBUserList;

    while( pCurList )
    {
        char sRegTime[64];
        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        JS_UTIL_getDateTime( pCurList->sUser.nRegTime, sRegTime );

        right_table_->setItem(i,0, new QTableWidgetItem(QString("%1").arg( pCurList->sUser.nNum )));
        right_table_->setItem(i,1, new QTableWidgetItem(QString("%1").arg( sRegTime )));
        right_table_->setItem(i,2, new QTableWidgetItem(QString("%1").arg( pCurList->sUser.pName )));
        right_table_->setItem(i,3, new QTableWidgetItem(QString("%1").arg( pCurList->sUser.pSSN )));
        right_table_->setItem(i,4, new QTableWidgetItem(QString("%1").arg( pCurList->sUser.pEmail )));
        right_table_->setItem(i,5, new QTableWidgetItem(QString("%1").arg( pCurList->sUser.nStatus )));
        right_table_->setItem(i,6, new QTableWidgetItem(QString("%1").arg( pCurList->sUser.pRefNum )));
        right_table_->setItem(i,7, new QTableWidgetItem(QString("%1").arg( pCurList->sUser.pAuthCode )));

        pCurList = pCurList->pNext;
        i++;
    }

    right_menu_->updatePageLabel();
    if( pDBUserList ) JS_DB_resetUserList( &pDBUserList );
}

void MainWindow::createRightCertProfileList()
{
    int i = 0;
    removeAllRight();
    JDB_CertProfileList  *pCertProfileList = NULL;
    JDB_CertProfileList  *pCurList = NULL;

    QStringList titleList = { "Num", "Name", "Version", "NotBefore", "NotAfter", "Hash", "DNTemplate" };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setType( ITEM_TYPE_CERT_PROFILE );

    right_table_->setColumnCount(titleList.size());
    right_table_->setHorizontalHeaderLabels( titleList );
    right_table_->verticalHeader()->setVisible(false);

    manApplet->ccClient()->getCertProfileList( &pCertProfileList );
    pCurList = pCertProfileList;

    while( pCurList )
    {
        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        right_table_->setItem( i, 0, new QTableWidgetItem( QString("%1").arg( pCurList->sCertProfile.nNum ) ));
        right_table_->setItem( i, 1, new QTableWidgetItem( pCurList->sCertProfile.pName ));
        right_table_->setItem( i, 2, new QTableWidgetItem( QString("%1").arg( pCurList->sCertProfile.nVersion )));
        right_table_->setItem( i, 3, new QTableWidgetItem( QString("%1").arg( pCurList->sCertProfile.nNotBefore )));
        right_table_->setItem( i, 4, new QTableWidgetItem( QString("%1").arg( pCurList->sCertProfile.nNotAfter )));
        right_table_->setItem( i, 5, new QTableWidgetItem( pCurList->sCertProfile.pHash ));
        right_table_->setItem( i, 6, new QTableWidgetItem( pCurList->sCertProfile.pDNTemplate ));

        pCurList = pCurList->pNext;
        i++;
    }

    if( pCertProfileList ) JS_DB_resetCertProfileList( &pCertProfileList );
}

void MainWindow::createRightCRLProfileList()
{
    int i = 0;
    removeAllRight();

    JDB_CRLProfileList   *pCRLProfileList = NULL;
    JDB_CRLProfileList   *pCurList = NULL;

    QStringList titleList = { "Num", "Name", "Version", "LastUpdate", "NextUpdate", "Hash" };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setType(ITEM_TYPE_CRL_PROFILE);

    right_table_->setColumnCount( titleList.size() );
    right_table_->setHorizontalHeaderLabels( titleList );
    right_table_->verticalHeader()->setVisible(false);

    manApplet->ccClient()->getCRLProfileList( &pCRLProfileList );
    pCurList = pCRLProfileList;

    while( pCurList )
    {
        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        right_table_->setItem( i, 0, new QTableWidgetItem( QString("%1").arg( pCurList->sCRLProfile.nNum )) );
        right_table_->setItem( i, 1, new QTableWidgetItem( pCurList->sCRLProfile.pName) );
        right_table_->setItem( i, 2, new QTableWidgetItem( QString("%1").arg( pCurList->sCRLProfile.nVersion )) );
        right_table_->setItem( i, 3, new QTableWidgetItem( QString("%1").arg( pCurList->sCRLProfile.nLastUpdate )) );
        right_table_->setItem( i, 4, new QTableWidgetItem( QString("%1").arg( pCurList->sCRLProfile.nNextUpdate )) );
        right_table_->setItem( i, 5, new QTableWidgetItem( pCurList->sCRLProfile.pHash) );

        pCurList = pCurList->pNext;
        i++;
    }

    if( pCRLProfileList ) JS_DB_resetCRLProfileList( &pCRLProfileList );
}

void MainWindow::createRightSignerList( int nItemType )
{
    int i = 0;
    int nType = -1;
    if( nItemType == ITEM_TYPE_REG_SIGNER )
        nType = 0;
    else if( nItemType == ITEM_TYPE_OCSP_SIGNER )
        nType = 1;

    JCC_SignerList  *pSignerList = NULL;
    JCC_SignerList  *pCurList = NULL;

    removeAllRight();

    QStringList titleList = { "Num", "RegTime", "Type", "DN", "Status", "Cert" };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setType( nItemType );

    right_table_->setColumnCount(titleList.size());
    right_table_->setHorizontalHeaderLabels(titleList);
    right_table_->verticalHeader()->setVisible(false);


    manApplet->ccClient()->getSignerList( nType, &pSignerList );
    pCurList = pSignerList;

    while( pCurList )
    {
        char sRegTime[64];
        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        JS_UTIL_getDateTime( pCurList->sSigner.nRegTime, sRegTime );

        right_table_->setItem(i,0, new QTableWidgetItem(QString("%1").arg( pCurList->sSigner.nNum )));
        right_table_->setItem(i,1, new QTableWidgetItem(QString("%1").arg( sRegTime )));
        right_table_->setItem(i,2, new QTableWidgetItem(QString("%1").arg( pCurList->sSigner.nType )));
        right_table_->setItem(i,3, new QTableWidgetItem(QString("%1").arg( pCurList->sSigner.pDN )));
        right_table_->setItem(i,4, new QTableWidgetItem(QString("%1").arg( pCurList->sSigner.nStatus )));
        right_table_->setItem(i,5, new QTableWidgetItem(QString("%1").arg( pCurList->sSigner.pCert )));

        pCurList = pCurList->pNext;
        i++;
    }

    if( pSignerList ) JS_DB_resetSignerList( &pSignerList );
}

void MainWindow::createRightCertList()
{
    removeAllRight();

    int i = 0;
    int nLimit = manApplet->settingsMgr()->listCount();
    int nPage = right_menu_->curPage();
    int nOffset = nPage * nLimit;
    int nTotalCnt = manApplet->ccClient()->getCount( ITEM_TYPE_CERT );
    char    sRegTime[64];

    right_menu_->setLimit( nLimit );
    right_menu_->setTotalCount( nTotalCnt );

    QStringList titleList = { "Num", "RegTime", "Serial", "SignAlg", "SubjectDN" };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setColumnCount( titleList.size() );
    right_table_->setHorizontalHeaderLabels( titleList );
    right_table_->verticalHeader()->setVisible(false);

    JDB_CertList    *pCertList = NULL;
    JDB_CertList    *pCurList = NULL;

    manApplet->ccClient()->getCertList( nOffset, nLimit, &pCertList );

    pCurList = pCertList;

    while( pCurList )
    {
        QString strDNInfo;
        if( pCurList->sCert.bSelf ) strDNInfo += "[Self]";
        if( pCurList->sCert.bCA ) strDNInfo += "[CA]";
        strDNInfo += QString( "[%1] " ).arg( pCurList->sCert.nStatus );
        strDNInfo += pCurList->sCert.pSubjectDN;

        JS_UTIL_getDateTime( pCurList->sCert.nRegTime, sRegTime );
        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        right_table_->setItem( i, 0, new QTableWidgetItem( QString("%1").arg( pCurList->sCert.nNum) ));
        right_table_->setItem( i, 1, new QTableWidgetItem( QString("%1").arg( sRegTime ) ));
        right_table_->setItem( i, 2, new QTableWidgetItem( QString("%1").arg(pCurList->sCert.pSerial)));
        right_table_->setItem( i, 3, new QTableWidgetItem( pCurList->sCert.pSignAlg ));
        right_table_->setItem( i, 4, new QTableWidgetItem( strDNInfo ));

        pCurList = pCurList->pNext;
        i++;
    }

    right_menu_->updatePageLabel();
    if( pCertList ) JS_DB_resetCertList( &pCertList );
}

void MainWindow::createRightCRLList()
{
    removeAllRight();

    int i = 0;
    int nLimit = manApplet->settingsMgr()->listCount();
    int nPage = right_menu_->curPage();
    int nOffset = nPage * nLimit;
    int nTotalCnt = manApplet->ccClient()->getCount( ITEM_TYPE_CRL );
    char    sRegTime[64];

    right_menu_->setLimit( nLimit );
    right_menu_->setTotalCount( nTotalCnt );

    QStringList titleList = { "Num", "RegTime", "IssuerNum", "SignAlg", "CRLDP" };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setColumnCount( titleList.size() );
    right_table_->setHorizontalHeaderLabels(titleList);
    right_table_->verticalHeader()->setVisible(false);

    JDB_CRLList     *pCRLList = NULL;
    JDB_CRLList     *pCurList = NULL;

    manApplet->ccClient()->getCRLList( nOffset, nLimit, &pCRLList );

    pCurList = pCRLList;

    while( pCurList )
    {
        JS_UTIL_getDateTime( pCurList->sCRL.nRegTime, sRegTime );

        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        right_table_->setItem( i, 0, new QTableWidgetItem( QString("%1").arg( pCurList->sCRL.nNum )));
        right_table_->setItem( i, 1, new QTableWidgetItem( QString("%1").arg( sRegTime )));
        right_table_->setItem( i, 2, new QTableWidgetItem(QString("%1").arg( pCurList->sCRL.nIssuerNum )));
        right_table_->setItem( i, 3, new QTableWidgetItem( pCurList->sCRL.pSignAlg ));
        right_table_->setItem( i, 4, new QTableWidgetItem( pCurList->sCRL.pCRLDP ));

        pCurList = pCurList->pNext;
        i++;
    }

    right_menu_->updatePageLabel();
    if( pCRLList ) JS_DB_resetCRLList( &pCRLList );
}

void MainWindow::createRightRevokedList()
{
    removeAllRight();

    int i = 0;
    int nLimit = manApplet->settingsMgr()->listCount();
    int nPage = right_menu_->curPage();
    int nOffset = nPage * nLimit;
    int nTotalCnt = manApplet->ccClient()->getCount( ITEM_TYPE_REVOKE );
    char    sDateTime[64];

    right_menu_->setLimit( nLimit );
    right_menu_->setTotalCount( nTotalCnt );

    QStringList titleList = { "Num", "CertNum", "IssuerNum", "Serial", "RevokedDate", "Reason", "CRLDP" };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setColumnCount( titleList.size() );
    right_table_->setHorizontalHeaderLabels(titleList);
    right_table_->verticalHeader()->setVisible(false);

    JCC_RevokedList     *pRevokedList = NULL;
    JCC_RevokedList     *pCurList = NULL;

    manApplet->ccClient()->getRevokedList( nOffset, nLimit, &pRevokedList );

    pCurList = pRevokedList;

    while( pCurList )
    {
        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        JS_UTIL_getDateTime( pCurList->sRevoked.nRevokedDate, sDateTime );

        right_table_->setItem(i,0, new QTableWidgetItem(QString("%1").arg( pCurList->sRevoked.nSeq )));
        right_table_->setItem(i,1, new QTableWidgetItem(QString("%1").arg( pCurList->sRevoked.nCertNum )));
        right_table_->setItem(i,2, new QTableWidgetItem(QString("%1").arg( pCurList->sRevoked.nIssuerNum )));
        right_table_->setItem(i,3, new QTableWidgetItem(QString("%1").arg( pCurList->sRevoked.pSerial )));
        right_table_->setItem(i,4, new QTableWidgetItem(QString("%1").arg( sDateTime )));
        right_table_->setItem(i,5, new QTableWidgetItem(QString("%1").arg( JS_PKI_getRevokeReasonName( pCurList->sRevoked.nReason ))));
        right_table_->setItem(i,6, new QTableWidgetItem(QString("%1").arg( pCurList->sRevoked.pCRLDP )));

        pCurList = pCurList->pNext;
        i++;
    }


    right_menu_->updatePageLabel();
    if( pRevokedList ) JS_DB_resetRevokedList( &pRevokedList );
}

void MainWindow::createRightCA()
{
    int     i = 0;
    removeAllRight();

    QStringList titleList = { "Name", "Values" };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setColumnCount(titleList.size());
    right_table_->setHorizontalHeaderLabels( titleList );
    right_table_->verticalHeader()->setVisible(false);

    BIN     binCert = {0,0};
    JCertInfo   sCertInfo;
    JExtensionInfoList  *pExtInfoList = NULL;
    JExtensionInfoList  *pCurList = NULL;

    char    sNotBefore[64];
    char    sNotAfter[64];

    JCC_NameVal sNameVal;
    memset( &sNameVal, 0x00, sizeof(sNameVal));
    memset( &sCertInfo, 0x00, sizeof(sCertInfo));

    manApplet->ccClient()->getCA( &sNameVal );
    JS_BIN_decodeHex( sNameVal.pValue, &binCert );

    JS_PKI_getCertInfo( &binCert, &sCertInfo, &pExtInfoList );

    right_table_->insertRow(i);
    right_table_->setRowHeight(i, 10 );
    right_table_->setItem( i, 0, new QTableWidgetItem( "Version" ));
    right_table_->setItem( i, 1, new QTableWidgetItem(QString("%1").arg(sCertInfo.nVersion)));

    i++;
    right_table_->insertRow(i);
    right_table_->setRowHeight(i, 10 );
    right_table_->setItem( i, 0, new QTableWidgetItem( "Serial"));
    right_table_->setItem( i, 1, new QTableWidgetItem(QString("%1").arg(sCertInfo.pSerial)));

    i++;
    right_table_->insertRow(i);
    right_table_->setRowHeight(i, 10 );
    right_table_->setItem( i, 0, new QTableWidgetItem( "SignAlgorithm"));
    right_table_->setItem( i, 1, new QTableWidgetItem(QString("%1").arg(sCertInfo.pSignAlgorithm)));

    i++;
    right_table_->insertRow(i);
    right_table_->setRowHeight(i, 10 );
    right_table_->setItem( i, 0, new QTableWidgetItem( "IssuerName"));
    right_table_->setItem( i, 1, new QTableWidgetItem(QString("%1").arg(sCertInfo.pIssuerName)));

    i++;
    right_table_->insertRow(i);
    right_table_->setRowHeight(i, 10 );
    right_table_->setItem( i, 0, new QTableWidgetItem( "SubjectName"));
    right_table_->setItem( i, 1, new QTableWidgetItem(QString("%1").arg(sCertInfo.pSubjectName)));

    i++;
    JS_UTIL_getDateTime( sCertInfo.uNotBefore, sNotBefore );
    right_table_->insertRow(i);
    right_table_->setRowHeight(i, 10 );
    right_table_->setItem( i, 0, new QTableWidgetItem( "NotBefore"));
    right_table_->setItem( i, 1, new QTableWidgetItem(QString("%1").arg(sNotBefore)));

    i++;
    JS_UTIL_getDateTime( sCertInfo.uNotAfter, sNotAfter );
    right_table_->insertRow(i);
    right_table_->setRowHeight(i, 10 );
    right_table_->setItem( i, 0, new QTableWidgetItem( "NotAfter"));
    right_table_->setItem( i, 1, new QTableWidgetItem(QString("%1").arg(sNotAfter)));

    i++;
    right_table_->insertRow(i);
    right_table_->setRowHeight(i, 10 );
    right_table_->setItem( i, 0, new QTableWidgetItem( "PublicKey"));
    right_table_->setItem( i, 1, new QTableWidgetItem(QString("%1").arg(sCertInfo.pPublicKey)));

    i++;
    right_table_->insertRow(i);
    right_table_->setRowHeight(i, 10 );
    right_table_->setItem( i, 0, new QTableWidgetItem( "DNHash"));
    right_table_->setItem( i, 1, new QTableWidgetItem(QString("%1").arg(sCertInfo.pDNHash)));

    i++;
    right_table_->insertRow(i);
    right_table_->setRowHeight(i, 10 );
    right_table_->setItem( i, 0, new QTableWidgetItem( "Signature"));
    right_table_->setItem( i, 1, new QTableWidgetItem(QString("%1").arg(sCertInfo.pSignature)));

    pCurList = pExtInfoList;

    while( pCurList )
    {
        JDB_ProfileExt   sDBExt;

        memset( &sDBExt, 0x00, sizeof(sDBExt));

        JS_PKI_transExtensionToDBRec( &pCurList->sExtensionInfo, &sDBExt );
        i++;
        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );
        right_table_->setItem( i, 0, new QTableWidgetItem( QString("%1").arg( sDBExt.pSN )));
        right_table_->setItem( i, 1, new QTableWidgetItem( QString("%1").arg( sDBExt.pValue )));

        pCurList = pCurList->pNext;
        JS_DB_resetProfileExt( &sDBExt );
    }

    JS_CC_resetNameVal( &sNameVal );
    JS_PKI_resetCertInfo( &sCertInfo );
    if( pExtInfoList ) JS_PKI_resetExtensionInfoList( &pExtInfoList );
}

void MainWindow::logClear()
{
    log_text_->clear();
}

void MainWindow::logCursorTop()
{
    log_text_->moveCursor(QTextCursor::Start);
}

void MainWindow::removeAllRight()
{
    log_text_->setText("");

    int rowCnt = right_table_->rowCount();

    for( int i=0; i < rowCnt; i++ )
        right_table_->removeRow(0);
}

void MainWindow::regUser()
{
    RegUserDlg regUserDlg;
    regUserDlg.exec();
}

void MainWindow::deleteUser()
{
    int row = right_table_->currentRow();
    if( row < 0 ) return;

    QTableWidgetItem* item = right_table_->item( row, 0 );
    int nSeq = item->text().toInt();

    manApplet->ccClient()->delUser( nSeq );

    createRightUserList();
}

void MainWindow::modifyUser()
{
    int row = right_table_->currentRow();
    if( row < 0 ) return;

    QTableWidgetItem* item = right_table_->item( row, 0 );
    int nSeq = item->text().toInt();
}

void MainWindow::makeCertProfile()
{
    MakeCertProfileDlg makeCertProfileDlg;
    makeCertProfileDlg.exec();
}

void MainWindow::makeCRLProfile()
{
    MakeCRLProfileDlg makeCRLProfileDlg;
    makeCRLProfileDlg.exec();
}

void MainWindow::regSigner()
{
    SignerDlg signerDlg;

    if( rightType() == ITEM_TYPE_OCSP_SIGNER )
        signerDlg.setType(1);
    else
        signerDlg.setType(0);

    signerDlg.exec();
}

void MainWindow::modifyCertProfile()
{
    int row = right_table_->currentRow();
    QTableWidgetItem* item = right_table_->item( row, 0 );

    int num = item->text().toInt();

    MakeCertProfileDlg makeCertProfileDlg;
    makeCertProfileDlg.setEdit(true);
    makeCertProfileDlg.setProfileNum(num);

    makeCertProfileDlg.exec();
}

void MainWindow::modifyCRLProfile()
{
    int row = right_table_->currentRow();
    QTableWidgetItem* item = right_table_->item( row, 0 );

    int num = item->text().toInt();

    MakeCRLProfileDlg makeCRLProfileDlg;
    makeCRLProfileDlg.setEdit(true);
    makeCRLProfileDlg.setProfileNum(num);
    makeCRLProfileDlg.exec();
}

void MainWindow::deleteCertProfile()
{
    int row = right_table_->currentRow();
    QTableWidgetItem* item = right_table_->item( row, 0 );

    int num = item->text().toInt();

    manApplet->ccClient()->delCertProfile( num );
    createRightCertProfileList();
}

void MainWindow::deleteCRLProfile()
{
    int row = right_table_->currentRow();
    QTableWidgetItem* item = right_table_->item( row, 0 );

    int num = item->text().toInt();

    manApplet->ccClient()->delCRLProfile( num );
    createRightCRLProfileList();
}

void MainWindow::deleteSigner()
{
    int row = right_table_->currentRow();
    QTableWidgetItem* item = right_table_->item( row, 0 );

    int num = item->text().toInt();

    manApplet->ccClient()->delSigner( num );
    createRightSignerList( rightType() );
}

void MainWindow::revokeCert()
{
    int row = right_table_->currentRow();
    if( row < 0 ) return;

    QTableWidgetItem* item = right_table_->item( row, 0 );
    int num = item->text().toInt();

    RevokeCertDlg revokeCertDlg;
    revokeCertDlg.setCertNum(num);
    revokeCertDlg.exec();
}

void MainWindow::deleteRevoke()
{
    int row = right_table_->currentRow();
    QTableWidgetItem* item = right_table_->item( row, 0 );

    int num = item->text().toInt();

    manApplet->ccClient()->delRevoked( num );
    createRightRevokedList();
}

int MainWindow::rightType()
{
    return right_table_->type();
}

int MainWindow::rightCount()
{
    return right_table_->rowCount();
}

void MainWindow::issueCert()
{
    IssueCertDlg issueCertDlg;
    issueCertDlg.exec();
}

void MainWindow::issueCRL()
{
    IssueCRLDlg issueCRLDlg;
    issueCRLDlg.exec();
}

void MainWindow::publishLDAP()
{
    int ret = 0;
    int num = -1;
    int nItemType = rightType();
    JCC_CodeMsg sCodeMsg;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    if( nItemType == ITEM_TYPE_CRL || nItemType == ITEM_TYPE_CERT )
    {
        int row = right_table_->currentRow();
        QTableWidgetItem* item = right_table_->item( row, 0 );

        num = item->text().toInt();
    }

    ret = manApplet->ccClient()->publishClient( nItemType, num, &sCodeMsg );
    if( ret == 0 && sCodeMsg.nCode == 0 )
    {
        manApplet->messageBox( "success to publish ldap\n" );
    }

    JS_CC_resetCodeMsg( &sCodeMsg );
}

void MainWindow::about()
{
    AboutDlg aboutDlg;
    aboutDlg.exec();
}

void MainWindow::settings()
{
    SettingsDlg settingsDlg;
    settingsDlg.exec();
}

void MainWindow::certInfo()
{
    int row = right_table_->currentRow();
    if( row < 0 ) return;

    QTableWidgetItem* item = right_table_->item( row, 0 );
    int num = item->text().toInt();

    CertInfoDlg certInfoDlg;
    certInfoDlg.setCertNum( num );
    certInfoDlg.exec();
}

void MainWindow::crlInfo()
{
    int row = right_table_->currentRow();
    if( row < 0 ) return;

    QTableWidgetItem* item = right_table_->item( row, 0 );
    int num = item->text().toInt();

    CRLInfoDlg crlInfoDlg;
    crlInfoDlg.setCRLNum( num );
    crlInfoDlg.exec();
}

void MainWindow::certStatus()
{
    int ret = 0;
    JCC_Cert    sCert;
    JCC_CertStatus  sCertStatus;
    QString strStatus;
    char    sRevokedDate[64];
    const char  *pReason = NULL;

    memset( &sCert, 0x00, sizeof(sCert));
    memset( &sCertStatus, 0x00, sizeof(sCertStatus));

    int row = right_table_->currentRow();
    if( row < 0 ) return;

    QTableWidgetItem* item = right_table_->item( row, 0 );
    int num = item->text().toInt();

    ret = manApplet->ccClient()->getCert( num, &sCert );
    if( ret != 0 )
    {
        manApplet->warningBox( tr("fail to get certificate information" ), this );
        goto end;
    }

    ret = manApplet->ccClient()->getCertStatus( sCert.pSerial, &sCertStatus );
    if( ret != 0 )
    {
        manApplet->warningBox( tr("fail to get certificate status"), this );
        goto end;
    }

    if( sCertStatus.nStatus == 0 )
    {
        strStatus = "Good";
    }
    else
    {
        JS_UTIL_getDateTime( sCertStatus.nRevokedDate, sRevokedDate );
        pReason = JS_PKI_getRevokeReasonName( sCertStatus.nReason );
        strStatus = QString( "Revoked Reason:%1 RevokedDate: %2" ).arg( pReason ).arg( sRevokedDate );
    }

    manApplet->messageBox( strStatus, this );

end :
    JS_DB_resetCert( &sCert );
    JS_CC_resetCertStatus( &sCertStatus );
}

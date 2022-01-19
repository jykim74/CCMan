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
#include "user_dlg.h"
#include "cc_client.h"
#include "settings_dlg.h"
#include "about_dlg.h"
#include "settings_mgr.h"
#include "cert_info_dlg.h"
#include "crl_info_dlg.h"
#include "admin_dlg.h"
#include "user_dlg.h"

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
#include "stat_form.h"
#include "js_kms.h"
#include "js_gen.h"


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

    QWidget *rightWidget = new QWidget;
    stack_ = new QStackedLayout;
    stat_ = new StatForm;

    stack_->addWidget( vsplitter_ );
    stack_->addWidget( stat_ );
    rightWidget->setLayout( stack_ );

    hsplitter_->addWidget(left_tree_);
//    hsplitter_->addWidget(vsplitter_);
    hsplitter_->addWidget( rightWidget );

    vsplitter_->addWidget(right_table_);
    vsplitter_->addWidget(right_menu_);
    vsplitter_->addWidget(log_text_);

    QList<int> vsizes;
    vsizes << 760 << 10 << 600;
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

    if( right_table_->type() == ITEM_TYPE_ADMIN )
    {
        menu.addAction( tr("ModifyAdmin"), this, &MainWindow::modifyAdmin );
    }
    else if( right_table_->type() == ITEM_TYPE_USER )
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
    else if( nType == ITEM_TYPE_KMS )
        logKMS( nSeq );
    else if( nType == ITEM_TYPE_TSP )
        logTSP( nSeq );
    else if( nType == ITEM_TYPE_AUDIT )
        logAudit( nSeq );
}

void MainWindow::logAdmin( int nSeq )
{
    JCC_Admin sAdmin;
    memset( &sAdmin, 0x00, sizeof(sAdmin));

    manApplet->ccClient()->getAdmin( nSeq, &sAdmin );

    logClear();

    manApplet->log( "========================================================================\n" );
    manApplet->log( "== Admin Information\n" );
    manApplet->log( "========================================================================\n" );
    manApplet->log( QString("Seq          : %1\n").arg(sAdmin.nSeq));
    manApplet->log( QString("Status       : %1 - %2\n").arg(sAdmin.nStatus).arg(getStatusName(sAdmin.nStatus)));
    manApplet->log( QString("Type         : %1 - %2\n").arg(sAdmin.nType).arg(getAdminTypeName(sAdmin.nType)));
    manApplet->log( QString("Name         : %1\n").arg(sAdmin.pName));
    manApplet->log( QString("Password     : %1\n").arg(sAdmin.pPassword));
    manApplet->log( QString("Email        : %1\n").arg(sAdmin.pEmail));

    logCursorTop();
    JS_DB_resetAdmin( &sAdmin );
}

void MainWindow::logUser( int nSeq )
{
    JCC_User sUser;
    memset( &sUser, 0x00, sizeof(sUser));

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
    manApplet->log( QString("Status        : %1 - %2\n").arg(sUser.nStatus).arg( getUserStatusName(sUser.nStatus)));
    manApplet->log( QString("RefNum        : %1\n").arg(sUser.pRefNum));
    manApplet->log( QString("AuthCode      : %1\n").arg(sUser.pAuthCode));

    logCursorTop();
    JS_DB_resetUser( &sUser );
}

void MainWindow::logCertProfile( int nNum )
{
    JCC_CertProfile  sCertProfile;
    JCC_ProfileExtList  *pProfileExtList = NULL;
    JCC_ProfileExtList *pCurList = NULL;

    memset( &sCertProfile, 0x00, sizeof(sCertProfile));
    manApplet->ccClient()->getCertProfile( nNum, &sCertProfile );

    QString strVersion;
    QString strNotBefore;
    QString strNotAfter;
    QString strDNTemplate;

    strVersion = QString( "V%1" ).arg( sCertProfile.nVersion + 1);

    if( sCertProfile.nNotBefore == 0 )
    {
        strNotBefore = "GenTime";
        strNotAfter = QString( "%1 Days" ).arg( sCertProfile.nNotAfter );
    }
    else
    {
        strNotBefore = getDateTime( sCertProfile.nNotBefore );
        strNotAfter = getDateTime( sCertProfile.nNotAfter );
    }

    if( strcasecmp( sCertProfile.pDNTemplate, "#CSR" ) == 0 )
        strDNTemplate = "Use CSR DN";
    else
        strDNTemplate = sCertProfile.pDNTemplate;

    logClear();
    manApplet->log( "========================================================================\n" );
    manApplet->log( "== Certificate Profile Information\n" );
    manApplet->log( "========================================================================\n" );
    manApplet->log( QString("Num         : %1\n").arg(sCertProfile.nNum));
    manApplet->log( QString("Name        : %1\n").arg(sCertProfile.pName));
    manApplet->log( QString("Version     : %1 - %2\n").arg(sCertProfile.nVersion).arg(strVersion));
    manApplet->log( QString("NotBefore   : %1 - %2\n").arg(sCertProfile.nNotBefore).arg(strNotBefore));
    manApplet->log( QString("NotAfter    : %1 - %2\n").arg(sCertProfile.nNotAfter).arg(strNotAfter));
    manApplet->log( QString("Hash        : %1\n").arg(sCertProfile.pHash));
    manApplet->log( QString("DNTemplate  : %1 - %2\n").arg(sCertProfile.pDNTemplate).arg(strDNTemplate));
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
    JCC_CRLProfile   sCRLProfile;
    JCC_ProfileExtList  *pProfileExtList = NULL;
    JCC_ProfileExtList  *pCurList = NULL;

    memset( &sCRLProfile, 0x00, sizeof(sCRLProfile));
    manApplet->ccClient()->getCRLProfile( nNum, &sCRLProfile );

    QString strVersion;
    QString strLastUpdate;
    QString strNextUpdate;

    strVersion = QString( "V%1" ).arg( sCRLProfile.nVersion + 1);

    if( sCRLProfile.nLastUpdate == 0 )
    {
        strLastUpdate = "GenTime";
        strNextUpdate = QString( "%1 Days" ).arg( sCRLProfile.nNextUpdate );
    }
    else
    {
        strLastUpdate = getDateTime( sCRLProfile.nLastUpdate );
        strNextUpdate = getDateTime( sCRLProfile.nNextUpdate );
    }

    logClear();
    manApplet->log( "========================================================================\n" );
    manApplet->log( "== CRL Profile Information\n" );
    manApplet->log( "========================================================================\n" );
    manApplet->log( QString("Num          : %1\n").arg(sCRLProfile.nNum));
    manApplet->log( QString("Name         : %1\n").arg(sCRLProfile.pName));
    manApplet->log( QString("Version      : %1 - %2\n").arg(sCRLProfile.nVersion).arg(strVersion));
    manApplet->log( QString("LastUpdate   : %1 - %2\n").arg(sCRLProfile.nLastUpdate).arg(strLastUpdate));
    manApplet->log( QString("NextUpdate   : %1 - %2\n").arg(sCRLProfile.nNextUpdate).arg(strNextUpdate));
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
    JCC_Signer  sSigner;
    memset( &sSigner, 0x00, sizeof(sSigner));

    manApplet->ccClient()->getSigner( nNum, &sSigner );

    manApplet->mainWindow()->logClear();
    manApplet->log( "========================================================================\n" );
    manApplet->log( "== Signer Information\n" );
    manApplet->log( "========================================================================\n" );
    manApplet->log( QString("Num          : %1\n").arg( sSigner.nNum));
    manApplet->log( QString("RegTime      : %1\n").arg(getDateTime(sSigner.nRegTime)));
    manApplet->log( QString("Type         : %1 - %2\n").arg( sSigner.nType ).arg(getSignerTypeName(sSigner.nType)));
    manApplet->log( QString("DN           : %1\n").arg( sSigner.pDN));
    manApplet->log( QString("DNHash       : %1\n").arg( sSigner.pDNHash));
    manApplet->log( QString("Cert         : %1\n").arg( sSigner.pCert ));
    manApplet->log( QString("Status       : %1 - %2\n").arg( sSigner.nStatus).arg(getStatusName( sSigner.nStatus )));
    manApplet->log( QString("Desc         : %1\n").arg( sSigner.pDesc ));

    logCursorTop();
    JS_DB_resetSigner( &sSigner );
}

void MainWindow::logCert( int nNum )
{
    QString strMsg;
    QString strPart;

    JCC_Cert    sCert;
    JCC_Cert    sIssuer;

    memset( &sCert, 0x00, sizeof(sCert));
    memset( &sIssuer, 0x00, sizeof(sIssuer));

    char    sRegDate[64];
    QString strUserName;
    QString strIssuerName;

    CCClient* ccClient = manApplet->ccClient();

    manApplet->ccClient()->getCert( nNum, &sCert );
    manApplet->ccClient()->getCert( sCert.nIssuerNum, &sIssuer );

    strUserName = ccClient->getName( sCert.nUserNum, "user" );
    strIssuerName = sIssuer.pSubjectDN;

    manApplet->mainWindow()->logClear();
    manApplet->log( "========================================================================\n" );
    manApplet->log( "== Certificate Information\n" );
    manApplet->log( "========================================================================\n" );
    manApplet->log( QString("Num           : %1\n").arg(sCert.nNum));
    JS_UTIL_getDateTime( sCert.nRegTime, sRegDate );
    manApplet->log( QString("RegDate       : %1\n").arg(sRegDate));
    manApplet->log( QString("KeyNum        : %1\n").arg(sCert.nKeyNum));
    manApplet->log( QString("UserNum       : %1 - %2\n").arg(sCert.nUserNum ).arg(strUserName));
    manApplet->log( QString("SignAlgorithm : %1\n").arg(sCert.pSignAlg));
    manApplet->log( QString("Certificate   : %1\n").arg(sCert.pCert));
    manApplet->log( QString("IsCA          : %1\n").arg(sCert.bCA));
    manApplet->log( QString("IsSelf        : %1\n").arg(sCert.bSelf));
    manApplet->log( QString("SubjectDN     : %1\n").arg(sCert.pSubjectDN));
    manApplet->log( QString("IssuerNum     : %1 - %2\n").arg(sCert.nIssuerNum).arg(strIssuerName));
    manApplet->log( QString("Status        : %1 - %2\n").arg(sCert.nStatus).arg(getCertStatusName(sCert.nStatus)));
    manApplet->log( QString("Serial        : %1\n").arg(sCert.pSerial));
    manApplet->log( QString("DNHash        : %1\n").arg(sCert.pDNHash));
    manApplet->log( QString("KeyHash       : %1\n").arg(sCert.pKeyHash));
    manApplet->log( QString("CRLDP         : %1\n").arg(sCert.pCRLDP));

    logCursorTop();
    JS_DB_resetCert( &sCert );
    JS_DB_resetCert( &sIssuer );
}

void MainWindow::logCRL( int nNum )
{
    QString strMsg;
    QString strPart;

    JCC_CRL sCRL;
    JCC_Cert sIssuer;

    memset( &sCRL, 0x00, sizeof(sCRL));
    memset( &sIssuer, 0x00, sizeof(sIssuer));

    char    sRegTime[64];

    manApplet->ccClient()->getCRL( nNum, &sCRL );
    manApplet->ccClient()->getCert( sCRL.nIssuerNum, &sIssuer );

    QString strIssuerName = sIssuer.pSubjectDN;

    manApplet->mainWindow()->logClear();
    manApplet->log( "========================================================================\n" );
    manApplet->log( "== CRL Information\n" );
    manApplet->log( "========================================================================\n" );
    manApplet->log( QString("Num           : %1\n").arg(sCRL.nNum));
    JS_UTIL_getDateTime( sCRL.nRegTime, sRegTime );
    manApplet->log( QString("RegTime       : %1\n").arg(sRegTime));
    manApplet->log( QString("IssuerNum     : %1 - %2\n").arg(sCRL.nIssuerNum).arg(strIssuerName));
    manApplet->log( QString("SignAlgorithm : %1\n").arg(sCRL.pSignAlg));
    manApplet->log( QString("CRLDP         : %1\n").arg(sCRL.pCRLDP));
    manApplet->log( QString("CRL           : %1\n").arg(sCRL.pCRL));

    logCursorTop();
    JS_DB_resetCRL( &sCRL );
    JS_DB_resetCert( &sIssuer );
}

void MainWindow::logRevoked( int nSeq )
{
    QString strMsg;
    QString strPart;

    JCC_Revoked sRevoked;
    memset( &sRevoked, 0x00, sizeof(sRevoked));

    CCClient* ccClient = manApplet->ccClient();

    ccClient->getRevoked( nSeq, &sRevoked );
    QString strCertName = ccClient->getDN( sRevoked.nCertNum );
    QString strIssuerName = ccClient->getDN( sRevoked.nIssuerNum );
    QString strReason = JS_PKI_getRevokeReasonName( sRevoked.nReason );

    manApplet->mainWindow()->logClear();
    manApplet->log( "========================================================================\n" );
    manApplet->log( "== Revoke Information\n" );
    manApplet->log( "========================================================================\n" );
    manApplet->log( QString("Seq          : %1\n").arg( sRevoked.nSeq));
    manApplet->log( QString("CertNum      : %1 - %2\n").arg( sRevoked.nCertNum).arg(strCertName));
    manApplet->log( QString("IssuerNum    : %1 - %2\n").arg( sRevoked.nIssuerNum).arg(strIssuerName));
    manApplet->log( QString("Serial       : %1\n").arg( sRevoked.pSerial));
    manApplet->log( QString("RevokeDate   : %1\n").arg( getDateTime( sRevoked.nRevokedDate )));
    manApplet->log( QString("Reason       : %1 - %2\n").arg( sRevoked.nReason).arg(strReason));
    manApplet->log( QString("CRLDP        : %1\n").arg( sRevoked.pCRLDP ));

    logCursorTop();
    JS_DB_resetRevoked( &sRevoked );
}

void MainWindow::logCA( int row )
{
    QTableWidgetItem* leftItem = right_table_->item(row, 0);
    QTableWidgetItem* rightItem = right_table_->item(row, 1);


    logClear();
    log( leftItem->text() );
    log( "\n\n" );
    log( rightItem->text() );
    logCursorTop();
}

void MainWindow::logKMS( int nSeq )
{
    JCC_KMS sKMS;
    memset( &sKMS, 0x00, sizeof(sKMS));
    CCClient* ccClient = manApplet->ccClient();

    ccClient->getKMS( nSeq, &sKMS );

    QString strType = JS_KMS_getObjectTypeName( sKMS.nType );
    QString strAlg = JS_PKI_getKeyTypeName( sKMS.nAlgorithm );

    manApplet->mainWindow()->logClear();
    manApplet->log( "========================================================================\n" );
    manApplet->log( "== KMS Information\n" );
    manApplet->log( "========================================================================\n" );
    manApplet->log( QString("Seq          : %1\n").arg( sKMS.nSeq));
    manApplet->log( QString("RegTime      : %1\n").arg( getDateTime( sKMS.nRegTime )));
    manApplet->log( QString("State        : %1 - %2\n").arg( sKMS.nState ).arg( getStatusName( sKMS.nState )));
    manApplet->log( QString("Type         : %1 - %2\n").arg( sKMS.nType ).arg(strType));
    manApplet->log( QString("Algorithm    : %1 - %2\n").arg( sKMS.nAlgorithm ).arg(strAlg));
    manApplet->log( QString("ID           : %1\n").arg( sKMS.pID ));
    manApplet->log( QString("Info         : %1\n").arg( sKMS.pInfo ));

    logCursorTop();
    JS_DB_resetKMS( &sKMS );
}

void MainWindow::logTSP( int nSeq )
{
    JCC_TSP sTSP;
    memset( &sTSP, 0x00, sizeof(sTSP));

    manApplet->ccClient()->getTSP( nSeq, &sTSP );

    manApplet->mainWindow()->logClear();
    manApplet->log( "========================================================================\n" );
    manApplet->log( "== TSP Information\n" );
    manApplet->log( "========================================================================\n" );
    manApplet->log( QString("Seq          : %1\n").arg( sTSP.nSeq));
    manApplet->log( QString("RegTime      : %1\n").arg( getDateTime( sTSP.nRegTime )));
    manApplet->log( QString("Serial       : %1\n").arg( sTSP.nSerial ));
    manApplet->log( QString("SrcHash      : %1\n").arg( sTSP.pSrcHash ));
    manApplet->log( QString("Policy       : %1\n").arg( sTSP.pPolicy ));
    manApplet->log( QString("TSTInfo      : %1\n").arg( sTSP.pTSTInfo ));
    manApplet->log( QString("Data         : %1\n").arg( sTSP.pData ));

    logCursorTop();
    JS_DB_resetTSP( &sTSP );
}

void MainWindow::logAudit( int nSeq )
{
    JCC_Audit sAudit;
    memset( &sAudit, 0x00, sizeof(sAudit));

    manApplet->ccClient()->getAudit( nSeq, &sAudit );

    QString strKind = JS_GEN_getKindName( sAudit.nKind );
    QString strOper = JS_GEN_getOperationName( sAudit.nOperation );

    manApplet->mainWindow()->logClear();
    manApplet->log( "========================================================================\n" );
    manApplet->log( "== Audit Information\n" );
    manApplet->log( "========================================================================\n" );
    manApplet->log( QString("Seq          : %1\n").arg( sAudit.nSeq));
    manApplet->log( QString("RegTime      : %1\n").arg( getDateTime( sAudit.nRegTime )));
    manApplet->log( QString("Kind         : %1 - %2\n").arg( sAudit.nKind ).arg( strKind ));
    manApplet->log( QString("Operation    : %1 - %2\n").arg( sAudit.nOperation ).arg( strOper ));
    manApplet->log( QString("UserName     : %1\n").arg( sAudit.pUserName ));
    manApplet->log( QString("Info         : %1\n").arg( sAudit.pInfo ));
    manApplet->log( QString("MAC          : %1\n").arg( sAudit.pMAC ));

    logCursorTop();
    JS_DB_resetAudit( &sAudit );
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

    ManTreeItem *pKMSItem = new ManTreeItem( QString( "KMS" ));
    pKMSItem->setIcon(QIcon(":/images/kms.png"));
    pKMSItem->setType( ITEM_TYPE_KMS );
    pTopItem->appendRow( pKMSItem );

    ManTreeItem *pTSPItem = new ManTreeItem( QString( "TSP" ));
    pTSPItem->setIcon(QIcon(":/images/timestamp.png"));
    pTSPItem->setType( ITEM_TYPE_TSP );
    pTopItem->appendRow( pTSPItem );

    ManTreeItem *pStatisticsItem = new ManTreeItem( QString( "Statistics" ));
    pStatisticsItem->setIcon(QIcon(":/images/statistics.png"));
    pStatisticsItem->setType( ITEM_TYPE_STATISTICS );
    pTopItem->appendRow( pStatisticsItem );

    ManTreeItem *pAuditItem = new ManTreeItem( QString( "Audit") );
    pAuditItem->setIcon( QIcon(":/images/audit.png"));
    pAuditItem->setType( ITEM_TYPE_AUDIT );
    pTopItem->appendRow( pAuditItem );

    QModelIndex ri = left_model_->index(0,0);
    left_tree_->expand(ri);
}

void MainWindow::createRightList(int nItemType)
{
    stack_->setCurrentIndex(0);

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
    else if( nItemType == ITEM_TYPE_KMS )
        createRightKMS();
    else if( nItemType == ITEM_TYPE_TSP )
        createRightTSP();
    else if( nItemType == ITEM_TYPE_STATISTICS )
        createRightStatistics();
    else if( nItemType == ITEM_TYPE_AUDIT )
        createRightAudit();
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

    right_table_->setColumnWidth( 0, 40 );
    right_table_->setColumnWidth( 1, 60 );
    right_table_->setColumnWidth( 2, 60 );
    right_table_->setColumnWidth( 3, 160 );
    right_table_->setColumnWidth( 4, 160 );

    while( pCurList )
    {
        right_table_->insertRow(i);

        right_table_->setRowHeight(i, 10 );
        right_table_->setItem( i, 0, new QTableWidgetItem( QString("%1").arg( pCurList->sAdmin.nSeq ) ));
        right_table_->setItem( i, 1, new QTableWidgetItem( QString("%1").arg( getStatusName( pCurList->sAdmin.nStatus ) ) ));
        right_table_->setItem( i, 2, new QTableWidgetItem( QString("%1").arg( getAdminTypeName( pCurList->sAdmin.nType ) )));
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
    int nTotalCnt = 0;

    QString strTarget = right_menu_->getCondName();
    QString strWord = right_menu_->getInputWord();

    right_menu_->setLimit( nLimit );
    right_menu_->setTotalCount( nTotalCnt );


    JDB_UserList    *pDBUserList = NULL;
    JDB_UserList    *pCurList = NULL;

    removeAllRight();

    QStringList titleList = { tr("Num"), tr("RegTime"), tr("Name"), tr("SSN"), tr("Email"), tr("Status") };


    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setType( ITEM_TYPE_USER );

    right_table_->setColumnCount(titleList.size());
    right_table_->setHorizontalHeaderLabels(titleList);
    right_table_->verticalHeader()->setVisible(false);

    if( strWord.length() > 0 )
    {
        nTotalCnt = manApplet->ccClient()->searchCount( ITEM_TYPE_USER, strTarget, strWord );
        manApplet->ccClient()->searchUserList( strTarget, strWord, nOffset, nLimit, &pDBUserList );
    }
    else
    {
        nTotalCnt = manApplet->ccClient()->getCount( ITEM_TYPE_USER );
        manApplet->ccClient()->getUserList( nOffset, nLimit, &pDBUserList );
    }

    pCurList = pDBUserList;

    right_table_->setColumnWidth( 0, 40 );
    right_table_->setColumnWidth( 1, 140 );
    right_table_->setColumnWidth( 2, 180 );
    right_table_->setColumnWidth( 3, 100 );
    right_table_->setColumnWidth( 4, 180 );
    right_table_->setColumnWidth( 5, 60 );

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
        right_table_->setItem(i,5, new QTableWidgetItem(QString("%1").arg( getUserStatusName( pCurList->sUser.nStatus ) )));

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

    QStringList titleList = { tr("Num"), tr("Name"), tr("Version"), tr("NotBefore"), tr("NotAfter"), tr("Hash"), tr("DNTemplate") };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setType( ITEM_TYPE_CERT_PROFILE );

    right_table_->setColumnCount(titleList.size());
    right_table_->setHorizontalHeaderLabels( titleList );
    right_table_->verticalHeader()->setVisible(false);

    manApplet->ccClient()->getCertProfileList( &pCertProfileList );
    pCurList = pCertProfileList;

    right_table_->setColumnWidth( 0, 40 );
    right_table_->setColumnWidth( 1, 200 );
    right_table_->setColumnWidth( 2, 50 );
    right_table_->setColumnWidth( 3, 100 );
    right_table_->setColumnWidth( 4, 100 );
    right_table_->setColumnWidth( 5, 60 );

    while( pCurList )
    {
        QString strVersion;
        QString strNotBefore;
        QString strNotAfter;
        QString strDNTemplate;

        strVersion = QString( "V%1" ).arg( pCurList->sCertProfile.nVersion + 1);

        if( pCurList->sCertProfile.nNotBefore == 0 )
        {
            strNotBefore = "GenTime";
            strNotAfter = QString( "%1 Days" ).arg( pCurList->sCertProfile.nNotBefore );
        }
        else
        {
            strNotBefore = getDateTime( pCurList->sCertProfile.nNotBefore );
            strNotAfter = getDateTime( pCurList->sCertProfile.nNotAfter );
        }

        if( strcasecmp( pCurList->sCertProfile.pDNTemplate, "#CSR" ) == 0 )
            strDNTemplate = "Use CSR DN";
        else
            strDNTemplate = pCurList->sCertProfile.pDNTemplate;

        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        right_table_->setItem( i, 0, new QTableWidgetItem( QString("%1").arg( pCurList->sCertProfile.nNum ) ));
        right_table_->setItem( i, 1, new QTableWidgetItem( pCurList->sCertProfile.pName ));
        right_table_->setItem( i, 2, new QTableWidgetItem( QString("%1").arg( strVersion )));
        right_table_->setItem( i, 3, new QTableWidgetItem( QString("%1").arg( strNotBefore )));
        right_table_->setItem( i, 4, new QTableWidgetItem( QString("%1").arg( strNotAfter )));
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

    QStringList titleList = { tr("Num"), tr("Name"), tr("Version"), tr("LastUpdate"), tr("NextUpdate"), tr("Hash") };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setType(ITEM_TYPE_CRL_PROFILE);

    right_table_->setColumnCount( titleList.size() );
    right_table_->setHorizontalHeaderLabels( titleList );
    right_table_->verticalHeader()->setVisible(false);

    manApplet->ccClient()->getCRLProfileList( &pCRLProfileList );
    pCurList = pCRLProfileList;

    right_table_->setColumnWidth( 0, 40 );
    right_table_->setColumnWidth( 1, 300 );
    right_table_->setColumnWidth( 2, 50 );
    right_table_->setColumnWidth( 3, 100 );
    right_table_->setColumnWidth( 4, 100 );
    right_table_->setColumnWidth( 5, 60 );

    while( pCurList )
    {
        QString strVersion;
        QString strLastUpdate;
        QString strNextUpdate;

        strVersion = QString( "V%1" ).arg( pCurList->sCRLProfile.nVersion + 1);

        if( pCurList->sCRLProfile.nLastUpdate == 0 )
        {
            strLastUpdate = "GenTime";
            strNextUpdate = QString( "%1 Days" ).arg( pCurList->sCRLProfile.nNextUpdate );
        }
        else
        {
            strLastUpdate = getDateTime( pCurList->sCRLProfile.nLastUpdate );
            strNextUpdate = getDateTime( pCurList->sCRLProfile.nNextUpdate );
        }

        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        right_table_->setItem( i, 0, new QTableWidgetItem( QString("%1").arg( pCurList->sCRLProfile.nNum )) );
        right_table_->setItem( i, 1, new QTableWidgetItem( pCurList->sCRLProfile.pName) );
        right_table_->setItem( i, 2, new QTableWidgetItem( QString("%1").arg( strVersion )) );
        right_table_->setItem( i, 3, new QTableWidgetItem( QString("%1").arg( strLastUpdate )) );
        right_table_->setItem( i, 4, new QTableWidgetItem( QString("%1").arg( strNextUpdate )) );
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

    QStringList titleList = { tr("Num"), tr("RegTime"), tr("Type"), tr("DN"), tr("Status"), tr("DNHash") };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setType( nItemType );

    right_table_->setColumnCount(titleList.size());
    right_table_->setHorizontalHeaderLabels(titleList);
    right_table_->verticalHeader()->setVisible(false);

    manApplet->ccClient()->getSignerList( nType, &pSignerList );
    pCurList = pSignerList;

    right_table_->setColumnWidth( 0, 40 );
    right_table_->setColumnWidth( 1, 140 );
    right_table_->setColumnWidth( 2, 80 );
    right_table_->setColumnWidth( 3, 200 );
    right_table_->setColumnWidth( 4, 60 );

    while( pCurList )
    {
        char sRegTime[64];
        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        JS_UTIL_getDateTime( pCurList->sSigner.nRegTime, sRegTime );

        right_table_->setItem(i,0, new QTableWidgetItem(QString("%1").arg( pCurList->sSigner.nNum )));
        right_table_->setItem(i,1, new QTableWidgetItem(QString("%1").arg( sRegTime )));
        right_table_->setItem(i,2, new QTableWidgetItem(QString("%1").arg( getSignerTypeName( pCurList->sSigner.nType ) )));
        right_table_->setItem(i,3, new QTableWidgetItem(QString("%1").arg( pCurList->sSigner.pDN )));
        right_table_->setItem(i,4, new QTableWidgetItem(QString("%1").arg( getStatusName(pCurList->sSigner.nStatus) )));
        right_table_->setItem(i,5, new QTableWidgetItem(QString("%1").arg( pCurList->sSigner.pDNHash )));

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
    int nTotalCnt = 0;
    char    sRegTime[64];

    right_menu_->setLimit( nLimit );
    right_menu_->setTotalCount( nTotalCnt );
    QString strTarget = right_menu_->getCondName();
    QString strWord = right_menu_->getInputWord();

    QStringList titleList = { tr("Num"), tr("RegTime"), tr("Serial"), tr("SignAlg"), tr("SubjectDN") };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setColumnCount( titleList.size() );
    right_table_->setHorizontalHeaderLabels( titleList );
    right_table_->verticalHeader()->setVisible(false);

    JDB_CertList    *pCertList = NULL;
    JDB_CertList    *pCurList = NULL;

    if( strWord.length() > 0 && strTarget.length() > 0 )
    {
        nTotalCnt = manApplet->ccClient()->searchCount( ITEM_TYPE_CERT, strTarget, strWord );
        manApplet->ccClient()->searchCertList( strTarget, strWord, nOffset, nLimit, &pCertList );
    }
    else
    {
        nTotalCnt = manApplet->ccClient()->getCount( ITEM_TYPE_CERT );
        manApplet->ccClient()->getCertList( nOffset, nLimit, &pCertList );
    }

    pCurList = pCertList;

    right_table_->setColumnWidth( 0, 40 );
    right_table_->setColumnWidth( 1, 140 );
    right_table_->setColumnWidth( 2, 100 );
    right_table_->setColumnWidth( 3, 200 );

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
    int nTotalCnt = 0;
    char    sRegTime[64];

    right_menu_->setLimit( nLimit );
    right_menu_->setTotalCount( nTotalCnt );
    QString strTarget = right_menu_->getCondName();
    QString strWord = right_menu_->getInputWord();

    QStringList titleList = { tr("Num"), tr("RegTime"), tr("IssuerNum"), tr("SignAlg"), tr("CRLDP") };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setColumnCount( titleList.size() );
    right_table_->setHorizontalHeaderLabels(titleList);
    right_table_->verticalHeader()->setVisible(false);

    JDB_CRLList     *pCRLList = NULL;
    JDB_CRLList     *pCurList = NULL;

    if( strTarget.length() > 0 && strWord.length() > 0 )
    {
        nTotalCnt = manApplet->ccClient()->searchCount( ITEM_TYPE_CRL, strTarget, strWord );
        manApplet->ccClient()->searchCRLList( strTarget, strWord, nOffset, nLimit, &pCRLList );
    }
    else
    {
        nTotalCnt = manApplet->ccClient()->getCount( ITEM_TYPE_CRL );
        manApplet->ccClient()->getCRLList( nOffset, nLimit, &pCRLList );
    }

    right_table_->setColumnWidth( 0, 40 );
    right_table_->setColumnWidth( 1, 140 );
    right_table_->setColumnWidth( 2, 200 );
    right_table_->setColumnWidth( 3, 200 );

    pCurList = pCRLList;

    while( pCurList )
    {
        JCC_Cert sIssuer;
        QString strIssuerName;

        memset( &sIssuer, 0x00, sizeof(sIssuer));

        manApplet->ccClient()->getCert( pCurList->sCRL.nIssuerNum, &sIssuer );
        strIssuerName = sIssuer.pSubjectDN;

        JS_UTIL_getDateTime( pCurList->sCRL.nRegTime, sRegTime );

        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        right_table_->setItem( i, 0, new QTableWidgetItem( QString("%1").arg( pCurList->sCRL.nNum )));
        right_table_->setItem( i, 1, new QTableWidgetItem( QString("%1").arg( sRegTime )));
        right_table_->setItem( i, 2, new QTableWidgetItem(QString("%1").arg( strIssuerName )));
        right_table_->setItem( i, 3, new QTableWidgetItem( pCurList->sCRL.pSignAlg ));
        right_table_->setItem( i, 4, new QTableWidgetItem( pCurList->sCRL.pCRLDP ));

        JS_DB_resetCert( &sIssuer );
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
    int nTotalCnt = 0;
    char    sDateTime[64];
    CCClient *ccClient = manApplet->ccClient();

    right_menu_->setLimit( nLimit );
    right_menu_->setTotalCount( nTotalCnt );
    QString strTarget = right_menu_->getCondName();
    QString strWord = right_menu_->getInputWord();

    QStringList titleList = { tr("Num"), tr("CertNum"), tr("IssuerNum"), tr("Serial"), tr("RevokedDate"), tr("Reason"), tr("CRLDP") };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setColumnCount( titleList.size() );
    right_table_->setHorizontalHeaderLabels(titleList);
    right_table_->verticalHeader()->setVisible(false);

    JCC_RevokedList     *pRevokedList = NULL;
    JCC_RevokedList     *pCurList = NULL;

    if( strTarget.length() > 0 && strWord.length() > 0 )
    {
        nTotalCnt = manApplet->ccClient()->searchCount( ITEM_TYPE_REVOKE, strTarget, strWord );
        manApplet->ccClient()->searchRevokedList( strTarget, strWord, nOffset, nLimit, &pRevokedList );
    }
    else
    {
        nTotalCnt = manApplet->ccClient()->getCount( ITEM_TYPE_REVOKE );
        manApplet->ccClient()->getRevokedList( nOffset, nLimit, &pRevokedList );
    }

    right_table_->setColumnWidth( 0, 40 );
    right_table_->setColumnWidth( 1, 120 );
    right_table_->setColumnWidth( 2, 120 );
    right_table_->setColumnWidth( 3, 60 );
    right_table_->setColumnWidth( 4, 140 );
    right_table_->setColumnWidth( 5, 120 );

    pCurList = pRevokedList;

    while( pCurList )
    {
        QString strCertName = ccClient->getDN( pCurList->sRevoked.nCertNum );
        QString strIssuerName = ccClient->getDN( pCurList->sRevoked.nIssuerNum );

        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        JS_UTIL_getDateTime( pCurList->sRevoked.nRevokedDate, sDateTime );

        right_table_->setItem(i,0, new QTableWidgetItem(QString("%1").arg( pCurList->sRevoked.nSeq )));
        right_table_->setItem(i,1, new QTableWidgetItem(QString("%1").arg( strCertName )));
        right_table_->setItem(i,2, new QTableWidgetItem(QString("%1").arg( strIssuerName )));
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

    QStringList titleList = { tr("Name"), tr("Values") };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setColumnCount(titleList.size());
    right_table_->setHorizontalHeaderLabels( titleList );
    right_table_->verticalHeader()->setVisible(false);

    right_table_->setColumnWidth( 0, 140 );

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

void MainWindow::createRightStatistics()
{
    stack_->setCurrentIndex(1);
}

void MainWindow::createRightKMS()
{
    log( "KMS" );

    removeAllRight();

    int i = 0;
    int nLimit = manApplet->settingsMgr()->listCount();
    int nPage = right_menu_->curPage();
    int nOffset = nPage * nLimit;
    int nTotalCnt = 0;
    char    sDateTime[64];
    CCClient* ccClient = manApplet->ccClient();

    right_menu_->setLimit( nLimit );
    right_menu_->setTotalCount( nTotalCnt );
    QString strTarget = right_menu_->getCondName();
    QString strWord = right_menu_->getInputWord();

    QStringList titleList = { tr("Seq"), tr("RegTime"), tr("State"), tr("Type"), tr("Algorithm"), tr("ID"), tr("Info") };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setColumnCount( titleList.size() );
    right_table_->setHorizontalHeaderLabels(titleList);
    right_table_->verticalHeader()->setVisible(false);

    JCC_KMSList     *pKMSList = NULL;
    JCC_KMSList     *pCurList = NULL;

    if( strTarget.length() > 0 && strWord.length() > 0 )
    {
        nTotalCnt = manApplet->ccClient()->searchCount( ITEM_TYPE_KMS, strTarget, strWord );
        ccClient->searchKMSList( strTarget, strWord, nOffset, nLimit, &pKMSList );
    }
    else
    {
        nTotalCnt = manApplet->ccClient()->getCount( ITEM_TYPE_KMS );
        ccClient->getKMSList( nOffset, nLimit, &pKMSList );
    }

    pCurList = pKMSList;

    right_table_->setColumnWidth( 0, 40 );
    right_table_->setColumnWidth( 1, 140 );
    right_table_->setColumnWidth( 2, 100 );
    right_table_->setColumnWidth( 3, 100 );
    right_table_->setColumnWidth( 4, 100 );
    right_table_->setColumnWidth( 5, 100 );

    while( pCurList )
    {
        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        QString strType = JS_KMS_getObjectTypeName( pCurList->sKMS.nType );
        QString strAlg = JS_PKI_getKeyTypeName( pCurList->sKMS.nAlgorithm );

        JS_UTIL_getDateTime( pCurList->sKMS.nRegTime, sDateTime );

        right_table_->setItem(i,0, new QTableWidgetItem(QString("%1").arg( pCurList->sKMS.nSeq )));
        right_table_->setItem(i,1, new QTableWidgetItem(QString("%1").arg( sDateTime )));
        right_table_->setItem(i,2, new QTableWidgetItem(QString("%1").arg( getStatusName( pCurList->sKMS.nState ))));
        right_table_->setItem(i,3, new QTableWidgetItem(QString("%1").arg( strType )));
        right_table_->setItem(i,4, new QTableWidgetItem(QString("%1").arg( strAlg )));
        right_table_->setItem(i,5, new QTableWidgetItem(QString("%1").arg( pCurList->sKMS.pID )));
        right_table_->setItem(i,6, new QTableWidgetItem(QString("%1").arg( pCurList->sKMS.pInfo )));

        pCurList = pCurList->pNext;
        i++;
    }


    right_menu_->updatePageLabel();
    if( pKMSList ) JS_DB_resetKMSList( &pKMSList );
}

void MainWindow::createRightTSP()
{
    log( "TSP" );
    removeAllRight();

    int i = 0;
    int nLimit = manApplet->settingsMgr()->listCount();
    int nPage = right_menu_->curPage();
    int nOffset = nPage * nLimit;
    int nTotalCnt = 0;
    char    sDateTime[64];

    right_menu_->setLimit( nLimit );
    right_menu_->setTotalCount( nTotalCnt );
    QString strTarget = right_menu_->getCondName();
    QString strWord = right_menu_->getInputWord();

    QStringList titleList = { tr("Seq"), tr("RegTime"), tr("Serial"), tr("SrcHash"), tr("Policy") };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setColumnCount( titleList.size() );
    right_table_->setHorizontalHeaderLabels(titleList);
    right_table_->verticalHeader()->setVisible(false);

    JCC_TSPList     *pTSPList = NULL;
    JCC_TSPList     *pCurList = NULL;

    if( strTarget.length() > 0 && strWord.length() > 0 )
    {
        nTotalCnt = manApplet->ccClient()->searchCount( ITEM_TYPE_TSP, strTarget, strWord );
        manApplet->ccClient()->searchTSPList( strTarget, strWord, nOffset, nLimit, &pTSPList );
    }
    else
    {
        nTotalCnt = manApplet->ccClient()->getCount( ITEM_TYPE_TSP );
        manApplet->ccClient()->getTSPList( nOffset, nLimit, &pTSPList );
    }

    right_table_->setColumnWidth( 0, 40 );
    right_table_->setColumnWidth( 1, 140 );
    right_table_->setColumnWidth( 2, 100 );
    right_table_->setColumnWidth( 3, 200 );

    pCurList = pTSPList;

    while( pCurList )
    {
        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        JS_UTIL_getDateTime( pCurList->sTSP.nRegTime, sDateTime );

        right_table_->setItem(i,0, new QTableWidgetItem(QString("%1").arg( pCurList->sTSP.nSeq )));
        right_table_->setItem(i,1, new QTableWidgetItem(QString("%1").arg( sDateTime )));
        right_table_->setItem(i,2, new QTableWidgetItem(QString("%1").arg( pCurList->sTSP.nSerial )));
        right_table_->setItem(i,3, new QTableWidgetItem(QString("%1").arg( pCurList->sTSP.pSrcHash )));
        right_table_->setItem(i,4, new QTableWidgetItem(QString("%1").arg( pCurList->sTSP.pPolicy )));


        pCurList = pCurList->pNext;
        i++;
    }


    right_menu_->updatePageLabel();
    if( pTSPList ) JS_DB_resetTSPList( &pTSPList );
}

void MainWindow::createRightAudit()
{
    log( "Audit" );
    removeAllRight();

    int i = 0;
    int nLimit = manApplet->settingsMgr()->listCount();
    int nPage = right_menu_->curPage();
    int nOffset = nPage * nLimit;
    int nTotalCnt = 0;
    char    sDateTime[64];

    right_menu_->setLimit( nLimit );
    right_menu_->setTotalCount( nTotalCnt );
    QString strTarget = right_menu_->getCondName();
    QString strWord = right_menu_->getInputWord();

    QStringList titleList = { tr("Seq"), tr("RegTime"), tr("Kind"), tr("Operation"), tr("UserName"), tr("Info"), tr("MAC") };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setColumnCount( titleList.size() );
    right_table_->setHorizontalHeaderLabels(titleList);
    right_table_->verticalHeader()->setVisible(false);

    JCC_AuditList     *pAuditList = NULL;
    JCC_AuditList     *pCurList = NULL;

    if( strTarget.length() > 0 && strWord.length() > 0 )
    {
        nTotalCnt = manApplet->ccClient()->searchCount( ITEM_TYPE_AUDIT, strTarget, strWord );
        manApplet->ccClient()->searchAuditList( strTarget, strWord, nOffset, nLimit, &pAuditList );
    }
    else
    {
        nTotalCnt = manApplet->ccClient()->getCount( ITEM_TYPE_AUDIT );
        manApplet->ccClient()->getAuditList( nOffset, nLimit, &pAuditList );
    }

    pCurList = pAuditList;

    right_table_->setColumnWidth( 0, 40 );
    right_table_->setColumnWidth( 1, 140 );
    right_table_->setColumnWidth( 2, 100 );
    right_table_->setColumnWidth( 3, 100 );
    right_table_->setColumnWidth( 4, 100 );
    right_table_->setColumnWidth( 5, 100 );

    while( pCurList )
    {
        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        QString strKind = JS_GEN_getKindName( pCurList->sAudit.nKind );
        QString strOper = JS_GEN_getOperationName( pCurList->sAudit.nOperation );

        JS_UTIL_getDateTime( pCurList->sAudit.nRegTime, sDateTime );

        right_table_->setItem(i,0, new QTableWidgetItem(QString("%1").arg( pCurList->sAudit.nSeq )));
        right_table_->setItem(i,1, new QTableWidgetItem(QString("%1").arg( sDateTime )));
        right_table_->setItem(i,2, new QTableWidgetItem(QString("%1").arg( strKind )));
        right_table_->setItem(i,3, new QTableWidgetItem(QString("%1").arg( strOper )));
        right_table_->setItem(i,4, new QTableWidgetItem(QString("%1").arg( pCurList->sAudit.pUserName )));
        right_table_->setItem(i,5, new QTableWidgetItem(QString("%1").arg( pCurList->sAudit.pInfo )));
        right_table_->setItem(i,6, new QTableWidgetItem(QString("%1").arg( pCurList->sAudit.pMAC )));

        pCurList = pCurList->pNext;
        i++;
    }


    right_menu_->updatePageLabel();
    if( pAuditList ) JS_DB_resetAuditList( &pAuditList );
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
    UserDlg userDlg;
    userDlg.exec();
}

void MainWindow::regAdmin()
{
    AdminDlg adminDlg;
    adminDlg.exec();
}

void MainWindow::deleteUser()
{
    int row = right_table_->currentRow();
    if( row < 0 ) return;

    QTableWidgetItem* item = right_table_->item( row, 0 );
    int nSeq = item->text().toInt();
    QString strMsg = tr( "Are you sure to delete user?" );

    bool bVal = manApplet->yesOrNoBox( strMsg, NULL, false );
    if( bVal == false ) return;

    manApplet->ccClient()->delUser( nSeq );

    createRightUserList();
}

void MainWindow::modifyUser()
{
    int row = right_table_->currentRow();
    if( row < 0 ) return;

    QTableWidgetItem* item = right_table_->item( row, 0 );
    int nSeq = item->text().toInt();

    UserDlg userDlg;
    userDlg.setEditMode( nSeq );
    userDlg.exec();
}

void MainWindow::modifyAdmin()
{
    int row = right_table_->currentRow();
    if( row < 0 ) return;

    QTableWidgetItem* item = right_table_->item( row, 0 );
    int nSeq = item->text().toInt();

    AdminDlg adminDlg;
    adminDlg.setEditMode( nSeq );
    adminDlg.exec();
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
    QString strMsg = tr("Are you sure to delete the signer?" );
    bool bVal = manApplet->yesOrNoBox( strMsg, this, false );
    if( bVal == false ) return;

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

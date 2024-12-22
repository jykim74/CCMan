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
//#include "search_menu.h"
#include "search_form.h"
#include "user_dlg.h"
#include "cc_client.h"
#include "settings_dlg.h"
#include "about_dlg.h"
#include "settings_mgr.h"
#include "cert_info_dlg.h"
#include "crl_info_dlg.h"
#include "admin_dlg.h"
#include "user_dlg.h"
#include "tst_info_dlg.h"
#include "config_dlg.h"
#include "make_lcn_dlg.h"
#include "lcn_info_dlg.h"

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
#include "js_pki.h"
#include "js_tsp.h"
#include "js_pkcs7.h"
#include "js_license.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initialize();

    createActions();
    createStatusBar();
    createTreeMenu();
    createTableMenu();

    setUnifiedTitleAndToolBarOnMac(true);
}

MainWindow::~MainWindow()
{
#ifdef _ENABLE_CHARTS
    delete stat_;
    delete stack_;
#endif
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
//    right_menu_ = new SearchMenu;
    right_menu_ = new SearchForm;
    log_text_ = new CodeEditor;

    right_table_ = new ManRightWidget;

#ifdef Q_OS_MAC
    right_menu_->setMaximumHeight( 30 );
#else
    right_menu_->setMaximumHeight(20);
#endif

    left_tree_->setModel(left_model_);
    log_text_->setReadOnly(true);

    right_table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    right_table_->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QWidget *rightWidget = new QWidget;

    hsplitter_->addWidget(left_tree_);

#ifdef _ENABLE_CHARTS
    stack_ = new QStackedLayout;
    stat_ = new StatForm;

    stack_->addWidget( vsplitter_ );
    stack_->addWidget( stat_ );
    rightWidget->setLayout( stack_ );

    hsplitter_->addWidget(left_tree_);
    hsplitter_->addWidget( rightWidget );
#else
    hsplitter_->addWidget( vsplitter_ );
#endif

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

void MainWindow::logLine()
{
    log( "================================================================================\n" );
}

void MainWindow::logLine2()
{
    log( "--------------------------------------------------------------------------------\n" );
}

void MainWindow::createActions()
{
    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));
    QToolBar *toolsToolBar = addToolBar(tr("Tools"));

    const QIcon configIcon = QIcon::fromTheme( "make-config", QIcon(":/images/config.png"));
    QAction *configAct = new QAction( configIcon, tr( "&MakeConfig"), this );
    configAct->setStatusTip( tr( "Make Config" ));
    connect( configAct, &QAction::triggered, this, &MainWindow::makeConfig );
    toolsMenu->addAction( configAct );
    toolsToolBar->addAction( configAct );

    const QIcon regIcon = QIcon::fromTheme( "reg-user", QIcon(":/images/user_add.png"));
    QAction *regUserAct = new QAction( regIcon, tr("&RegisterUser"), this );
    regUserAct->setStatusTip( tr("Register a user"));
    connect( regUserAct, &QAction::triggered, this, &MainWindow::regUser);
    toolsMenu->addAction(regUserAct);
    toolsToolBar->addAction(regUserAct);

    const QIcon certIcon = QIcon::fromTheme( "document-cert", QIcon(":/images/cert.png"));
    QAction *issueCertAct = new QAction( certIcon, tr("&IssueCert"), this );
    issueCertAct->setStatusTip(tr("Issue certificate") );
    connect( issueCertAct, &QAction::triggered, this, &MainWindow::issueCert );
    toolsMenu->addAction(issueCertAct);
    toolsToolBar->addAction(issueCertAct);

    const QIcon signerRegIcon = QIcon::fromTheme("signer-register", QIcon(":/images/signer_reg.png"));
    QAction *regSignerAct = new QAction( signerRegIcon, tr("Register&Signer"), this );
    regSignerAct->setStatusTip(tr( "Register Signer"));
    connect( regSignerAct, &QAction::triggered, this, &MainWindow::regSigner );
    toolsMenu->addAction( regSignerAct );
    toolsToolBar->addAction( regSignerAct );

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QToolBar *helpToolBar = addToolBar(tr("Help"));

    const QIcon settingIcon = QIcon::fromTheme("setting", QIcon(":/images/setting.png"));
    QAction *settingsAct = new QAction( settingIcon, tr("&Settings"), this);
    connect( settingsAct, &QAction::triggered, this, &MainWindow::settings);
    settingsAct->setStatusTip(tr("Settings CCMan"));
    helpMenu->addAction( settingsAct );
    helpToolBar->addAction( settingsAct );

    const QIcon caManIcon = QIcon::fromTheme("certman", QIcon(":/images/caman.png"));
    QAction *aboutAct = new QAction( caManIcon, tr("&About CCMan"), this);
    connect( aboutAct, &QAction::triggered, this, &MainWindow::about);
    helpMenu->addAction( aboutAct );
    helpToolBar->addAction( aboutAct );
    aboutAct->setStatusTip(tr("About CertMan"));

    const QIcon certProfileIcon = QIcon::fromTheme("cert-profile", QIcon(":/images/cert_profile.png"));
    QAction *makeCertProfileAct = new QAction( certProfileIcon, tr("MakeCert&Profile"), this );
    makeCertProfileAct->setStatusTip(tr( "Make certificate profile"));
    connect( makeCertProfileAct, &QAction::triggered, this, &MainWindow::makeCertProfile );
    toolsMenu->addAction( makeCertProfileAct );
    toolsToolBar->addAction( makeCertProfileAct );

    const QIcon crlProfileIcon = QIcon::fromTheme("crl-profile", QIcon(":/images/crl_profile.png"));
    QAction *makeCRLProfileAct = new QAction( crlProfileIcon, tr("MakeC&RLProfile"), this );
    connect( makeCRLProfileAct, &QAction::triggered, this, &MainWindow::makeCRLProfile);
    toolsMenu->addAction( makeCRLProfileAct );
    toolsToolBar->addAction( makeCRLProfileAct );
    makeCRLProfileAct->setStatusTip(tr( "Make CRL Profile"));

    const QIcon lcnIcon = QIcon::fromTheme("make-lcn", QIcon(":/images/license.png"));
    QAction *makeLCNAct = new QAction( lcnIcon, tr("MakeL&icense"), this );
    connect( makeLCNAct, &QAction::triggered, this, &MainWindow::makeLicense);
    toolsMenu->addAction( makeLCNAct );
    toolsToolBar->addAction( makeLCNAct );
    makeCRLProfileAct->setStatusTip(tr( "Make License"));
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createTableMenu()
{
    QString style = "QHeaderView::section {background-color:#404040;color:#FFFFFF;}";
    right_table_->horizontalHeader()->setStyleSheet( style );
}

void MainWindow::showRightMenu(QPoint point)
{
//    QTableWidgetItem* item = right_table_->itemAt(point);

    QMenu menu(this);

    if( right_table_->type() == ITEM_TYPE_ADMIN )
    {
        menu.addAction( tr("ModifyAdmin"), this, &MainWindow::modifyAdmin );
    }
    else if( right_table_->type() == ITEM_TYPE_CONFIG )
    {
        menu.addAction( tr("ModifyConfig"), this, &MainWindow::editConfig );
        menu.addAction( tr("DeleteConfig"), this, &MainWindow::deleteConfig );
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
        menu.addAction( tr( "CopyCertProfile" ), this, &MainWindow::copyCertProfile );
    }
    else if( rightType() == ITEM_TYPE_CRL_PROFILE )
    {
        menu.addAction( tr("ModifyCRLProfile"), this, &MainWindow::modifyCRLProfile );
        menu.addAction( tr("DeleteCRLProfile"), this, &MainWindow::deleteCRLProfile );
        menu.addAction( tr("CopyCRLProfile"), this, &MainWindow::copyCRLProfile );
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
    else if( rightType() == ITEM_TYPE_AUDIT )
    {
        menu.addAction( tr("VerifyAudit"), this, &MainWindow::verifyAudit );
    }
    else if( rightType() == ITEM_TYPE_TSP )
    {
        menu.addAction( tr("View TSTInfo"), this, &MainWindow::viewTSTInfo );
        menu.addAction( tr("VerifyTSMessage"), this, &MainWindow::verifyTSMessage );
    }
    else if( rightType() == ITEM_TYPE_LICENSE )
    {
        menu.addAction( tr("DeleteLicense"), this, &MainWindow::deleteLicense );
        menu.addAction( tr("ViewLicense"), this, &MainWindow::viewLicense );
        menu.addAction( tr( "SaveLicense"), this, &MainWindow::saveLicense );
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

    log_text_->setPlainText( strVal );

    if( nType == ITEM_TYPE_ADMIN )
        logAdmin( nSeq );
    else if( nType == ITEM_TYPE_CONFIG )
        logConfig( nSeq );
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
    else if( nType == ITEM_TYPE_LICENSE )
        logLCN( nSeq );
}

void MainWindow::logAdmin( int nSeq )
{
    JCC_Admin sAdmin;
    memset( &sAdmin, 0x00, sizeof(sAdmin));

    manApplet->ccClient()->getAdmin( nSeq, &sAdmin );

    logClear();

    logLine();
    log( "== Admin Information\n" );
    logLine();
    log( QString("Seq          : %1\n").arg(sAdmin.nSeq));
    log( QString("Status       : %1 - %2\n").arg(getStatusName(sAdmin.nStatus)).arg(sAdmin.nStatus));
    log( QString("Type         : %1 - %2\n").arg(getAdminTypeName(sAdmin.nType)).arg(sAdmin.nType));
    log( QString("Name         : %1\n").arg(sAdmin.pName));
    log( QString("Password     : %1\n").arg(sAdmin.pPassword));
    log( QString("Email        : %1\n").arg(sAdmin.pEmail));
    logLine();

    logCursorTop();
    JS_DB_resetAdmin( &sAdmin );
}

void MainWindow::logConfig( int nNum )
{
    JCC_Config sConfig;
    memset( &sConfig, 0x00, sizeof(sConfig));

    manApplet->ccClient()->getConfig( nNum, &sConfig );

    logClear();

    logLine();
    manApplet->log( "== Config Information\n" );
    logLine();
    manApplet->log( QString("Num          : %1\n").arg(sConfig.nNum));
    manApplet->log( QString("Kind         : %1\n").arg(sConfig.nKind));
    manApplet->log( QString("Name         : %1\n").arg(sConfig.pName));
    manApplet->log( QString("Value        : %1\n").arg(sConfig.pValue));
    logLine();

    logCursorTop();
    JS_DB_resetConfig( &sConfig );
}

void MainWindow::logUser( int nSeq )
{
    JCC_User sUser;
    memset( &sUser, 0x00, sizeof(sUser));

    manApplet->ccClient()->getUser( nSeq, &sUser );

    logClear();

    logLine();
    manApplet->log( "== User Information\n" );
    logLine();
    manApplet->log( QString("Num           : %1\n").arg(sUser.nNum));
    manApplet->log( QString("RegTime       : %1\n").arg(getDateTime(sUser.nRegTime)));
    manApplet->log( QString("Name          : %1\n").arg(sUser.pName));
    manApplet->log( QString("SSN           : %1\n").arg(sUser.pSSN));
    manApplet->log( QString("Email         : %1\n").arg(sUser.pEmail));
    manApplet->log( QString("Status        : %1 - %2\n").arg( getUserStatusName(sUser.nStatus)).arg(sUser.nStatus));
    manApplet->log( QString("RefNum        : %1\n").arg(sUser.pRefNum));
    manApplet->log( QString("AuthCode      : %1\n").arg(sUser.pAuthCode));
    logLine();

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
/*
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
*/

    getPeriodString( sCertProfile.nNotBefore, sCertProfile.nNotAfter, strNotBefore, strNotAfter );

    if( strcasecmp( sCertProfile.pDNTemplate, "#CSR_DN" ) == 0 )
        strDNTemplate = "Use CSR DN";
    else
        strDNTemplate = sCertProfile.pDNTemplate;

    logClear();
    logLine();
    manApplet->log( "== Certificate Profile Information\n" );
    logLine();
    manApplet->log( QString("Num         : %1\n").arg(sCertProfile.nNum));
    manApplet->log( QString("Name        : %1\n").arg(sCertProfile.pName));
    manApplet->log( QString("Version     : %1 - %2\n").arg(strVersion).arg(sCertProfile.nVersion));
    manApplet->log( QString("NotBefore   : %1 - %2\n").arg(strNotBefore).arg(sCertProfile.nNotBefore));
    manApplet->log( QString("NotAfter    : %1 - %2\n").arg(strNotAfter).arg(sCertProfile.nNotAfter));
    manApplet->log( QString("Hash        : %1\n").arg(sCertProfile.pHash));
    manApplet->log( QString("DNTemplate  : %1 - %2\n").arg(strDNTemplate).arg(sCertProfile.pDNTemplate));
    manApplet->log( "======================= Extension Information ==========================\n" );

    manApplet->ccClient()->getCertProfileExtList( nNum, &pProfileExtList );
    pCurList = pProfileExtList;

    while( pCurList )
    {
        int nSeq = pCurList->sProfileExt.nSeq;
        QString strSN = pCurList->sProfileExt.pSN;
        QString strValue = pCurList->sProfileExt.pValue;
        QString strShowValue = getProfileExtInfoValue( strSN, strValue );
        int bCrit = pCurList->sProfileExt.bCritical;

        logLine();
        manApplet->log( QString( "| %1 | %2 | Seq: %3 |\n")
                            .arg( strSN, -37 )
                            .arg( bCrit ? "Critical" : "Normal", -10 )
                            .arg( nSeq, 10 ));

        if( strShowValue.length() > 0 )
        {
            logLine2();
            manApplet->log( QString( "%1" ).arg( strShowValue ) );
        }

        logLine();

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
    QString strThisUpdate;
    QString strNextUpdate;

    strVersion = QString( "V%1" ).arg( sCRLProfile.nVersion + 1);

    if( sCRLProfile.nThisUpdate == 0 )
    {
        strThisUpdate = "GenTime";
        strNextUpdate = QString( "%1 Days" ).arg( sCRLProfile.nNextUpdate );
    }
    else
    {
        strThisUpdate = getDateTime( sCRLProfile.nThisUpdate );
        strNextUpdate = getDateTime( sCRLProfile.nNextUpdate );
    }

    logClear();
    logLine();
    manApplet->log( "== CRL Profile Information\n" );
    logLine();
    manApplet->log( QString("Num          : %1\n").arg(sCRLProfile.nNum));
    manApplet->log( QString("Name         : %1\n").arg(sCRLProfile.pName));
    manApplet->log( QString("Version      : %1 - %2\n").arg(strVersion).arg(sCRLProfile.nVersion));
    manApplet->log( QString("ThisUpdate   : %1 - %2\n").arg(strThisUpdate).arg(sCRLProfile.nThisUpdate));
    manApplet->log( QString("NextUpdate   : %1 - %2\n").arg(strNextUpdate).arg(sCRLProfile.nNextUpdate));
    manApplet->log( QString("Hash         : %1\n").arg(sCRLProfile.pHash));
    manApplet->log( "======================= Extension Information ==========================\n" );

    manApplet->ccClient()->getCRLProfileExtList( nNum, &pProfileExtList );
    pCurList = pProfileExtList;

    while( pCurList )
    {
        int nSeq = pCurList->sProfileExt.nSeq;
        QString strSN = pCurList->sProfileExt.pSN;
        QString strValue = pCurList->sProfileExt.pValue;
        QString strShowValue = getProfileExtInfoValue( strSN, strValue );
        int bCrit = pCurList->sProfileExt.bCritical;

        logLine();
        manApplet->log( QString( "| %1 | %2 | Seq: %3 |\n")
                           .arg( strSN, -37 )
                           .arg( bCrit ? "Critical" : "Normal", -10 )
                           .arg( nSeq, 10 ));

        if( strShowValue.length() > 0 )
        {
            logLine2();
            manApplet->log( QString( "%1" ).arg( strShowValue ) );
        }

        logLine();

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
    logLine();
    manApplet->log( "== Signer Information\n" );
    logLine();
    manApplet->log( QString("Num          : %1\n").arg( sSigner.nNum));
    manApplet->log( QString("RegTime      : %1\n").arg(getDateTime(sSigner.nRegTime)));
    manApplet->log( QString("Type         : %1 - %2\n").arg(getSignerTypeName(sSigner.nType)).arg( sSigner.nType ));
    manApplet->log( QString("DN           : %1\n").arg( sSigner.pDN));
    manApplet->log( QString("DNHash       : %1\n").arg( sSigner.pDNHash));
    manApplet->log( QString("Cert         : %1\n").arg( sSigner.pCert ));
    manApplet->log( QString("Status       : %1 - %2\n").arg(getStatusName( sSigner.nStatus )).arg( sSigner.nStatus));
    manApplet->log( QString("Info         : %1\n").arg( sSigner.pInfo ));
    logLine();

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
    logLine();
    manApplet->log( "== Certificate Information\n" );
    logLine();
    manApplet->log( QString("Num           : %1\n").arg(sCert.nNum));
    JS_UTIL_getDateTime( sCert.nRegTime, sRegDate );
    manApplet->log( QString("RegDate       : %1\n").arg(sRegDate));
    manApplet->log( QString("KeyNum        : %1\n").arg(sCert.nKeyNum));
    manApplet->log( QString("UserNum       : %1 - %2\n").arg(strUserName).arg(sCert.nUserNum ));
    manApplet->log( QString("SignAlgorithm : %1\n").arg(sCert.pSignAlg));
    manApplet->log( QString("Certificate   : %1\n").arg(sCert.pCert));
    manApplet->log( QString("IsCA          : %1\n").arg(sCert.bCA));
    manApplet->log( QString("IsSelf        : %1\n").arg(sCert.bSelf));
    manApplet->log( QString("SubjectDN     : %1\n").arg(sCert.pSubjectDN));
    manApplet->log( QString("IssuerNum     : %1 - %2\n").arg(strIssuerName).arg(sCert.nIssuerNum));
    manApplet->log( QString("Status        : %1 - %2\n").arg(getCertStatusName(sCert.nStatus)).arg(sCert.nStatus));
    manApplet->log( QString("Serial        : %1\n").arg(sCert.pSerial));
    manApplet->log( QString("DNHash        : %1\n").arg(sCert.pDNHash));
    manApplet->log( QString("KeyHash       : %1\n").arg(sCert.pKeyHash));
    manApplet->log( QString("CRLDP         : %1\n").arg(sCert.pCRLDP));
    logLine();

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
    logLine();
    manApplet->log( "== CRL Information\n" );
    logLine();
    manApplet->log( QString("Num           : %1\n").arg(sCRL.nNum));
    JS_UTIL_getDateTime( sCRL.nRegTime, sRegTime );
    manApplet->log( QString("RegTime       : %1\n").arg(sRegTime));
    manApplet->log( QString("IssuerNum     : %1 - %2\n").arg(strIssuerName).arg(sCRL.nIssuerNum));
    manApplet->log( QString("SignAlgorithm : %1\n").arg(sCRL.pSignAlg));
    manApplet->log( QString("CRLDP         : %1\n").arg(sCRL.pCRLDP));
    manApplet->log( QString("CRL           : %1\n").arg(sCRL.pCRL));
    logLine();

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
    logLine();
    manApplet->log( "== Revoke Information\n" );
    logLine();
    manApplet->log( QString("Seq          : %1\n").arg( sRevoked.nSeq));
    manApplet->log( QString("CertNum      : %1 - %2\n").arg(strCertName).arg( sRevoked.nCertNum));
    manApplet->log( QString("IssuerNum    : %1 - %2\n").arg(strIssuerName).arg( sRevoked.nIssuerNum));
    manApplet->log( QString("Serial       : %1\n").arg( sRevoked.pSerial));
    manApplet->log( QString("RevokeDate   : %1\n").arg( getDateTime( sRevoked.nRevokedDate )));
    manApplet->log( QString("Reason       : %1 - %2\n").arg(strReason).arg( sRevoked.nReason));
    manApplet->log( QString("CRLDP        : %1\n").arg( sRevoked.pCRLDP ));
    logLine();

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
    logLine();
    manApplet->log( "== KMS Information\n" );
    logLine();
    manApplet->log( QString("Seq          : %1\n").arg( sKMS.nSeq));
    manApplet->log( QString("RegTime      : %1\n").arg( getDateTime( sKMS.nRegTime )));
    manApplet->log( QString("State        : %1 - %2\n").arg( getStatusName( sKMS.nState )).arg( sKMS.nState ));
    manApplet->log( QString("Type         : %1 - %2\n").arg(strType).arg( sKMS.nType ));
    manApplet->log( QString("Algorithm    : %1 - %2\n").arg(strAlg).arg( sKMS.nAlgorithm ));
    manApplet->log( QString("ID           : %1\n").arg( sKMS.pID ));
    manApplet->log( QString("Info         : %1\n").arg( sKMS.pInfo ));
    logLine();

    logCursorTop();
    JS_DB_resetKMS( &sKMS );
}

void MainWindow::logTSP( int nSeq )
{
    JCC_TSP sTSP;
    memset( &sTSP, 0x00, sizeof(sTSP));

    manApplet->ccClient()->getTSP( nSeq, &sTSP );

    manApplet->mainWindow()->logClear();
    logLine();
    manApplet->log( "== TSP Information\n" );
    logLine();
    manApplet->log( QString("Seq          : %1\n").arg( sTSP.nSeq));
    manApplet->log( QString("RegTime      : %1\n").arg( getDateTime( sTSP.nRegTime )));
    manApplet->log( QString("Serial       : %1\n").arg( sTSP.nSerial ));
    manApplet->log( QString("SrcHash      : %1\n").arg( sTSP.pSrcHash ));
    manApplet->log( QString("Policy       : %1\n").arg( sTSP.pPolicy ));
    manApplet->log( QString("TSTInfo      : %1\n").arg( sTSP.pTSTInfo ));
    manApplet->log( QString("Data         : %1\n").arg( sTSP.pData ));
    logLine();

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
    logLine();
    manApplet->log( "== Audit Information\n" );
    logLine();
    manApplet->log( QString("Seq          : %1\n").arg( sAudit.nSeq));
    manApplet->log( QString("RegTime      : %1\n").arg( getDateTime( sAudit.nRegTime )));
    manApplet->log( QString("Kind         : %1 - %2\n").arg( strKind ).arg( sAudit.nKind ));
    manApplet->log( QString("Operation    : %1 - %2\n").arg( strOper ).arg( sAudit.nOperation ));
    manApplet->log( QString("UserName     : %1\n").arg( sAudit.pUserName ));
    manApplet->log( QString("Info         : %1\n").arg( sAudit.pInfo ));
    manApplet->log( QString("MAC          : %1\n").arg( sAudit.pMAC ));
    logLine();

    logCursorTop();
    JS_DB_resetAudit( &sAudit );
}

void MainWindow::logLCN( int nSeq )
{
    JCC_LCN sLCN;
    memset( &sLCN, 0x00, sizeof(sLCN));

    manApplet->ccClient()->getLCN( nSeq, &sLCN );

    manApplet->mainWindow()->logClear();
    logLine();
    manApplet->log( "== License Information\n" );
    logLine();
    manApplet->log( QString("Seq          : %1\n").arg( sLCN.nSeq));
    manApplet->log( QString("RegTime      : %1\n").arg( getDateTime( sLCN.nRegTime )));
    manApplet->log( QString("Type         : %1\n").arg( sLCN.nType));
    manApplet->log( QString("QTY          : %1\n").arg( sLCN.nQTY));
    manApplet->log( QString("SID          : %1\n").arg( sLCN.pSID ));
    manApplet->log( QString("User         : %1\n").arg( sLCN.pUser ));
    manApplet->log( QString("Issued       : %1\n").arg( sLCN.pIssued ));
    manApplet->log( QString("Expire       : %1\n").arg( sLCN.pExpire ));
    manApplet->log( QString("Product      : %1\n").arg( sLCN.pProduct));
    manApplet->log( QString("Ref          : %1\n").arg( sLCN.pRef));
    manApplet->log( QString("Ext          : %1\n").arg( sLCN.pExt ));
    manApplet->log( QString("AuthKey      : %1\n").arg( sLCN.pAuthKey ));
    manApplet->log( QString("License      : %1\n").arg( sLCN.pLicense ));
    logLine();


    logCursorTop();
    JS_DB_resetLCN( &sLCN );
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

    ManTreeItem *pConfigItem = new ManTreeItem( QString("Config"));
    pConfigItem->setIcon(QIcon(":/images/config.png"));
    pConfigItem->setType( ITEM_TYPE_CONFIG );
    pTopItem->appendRow( pConfigItem );



    ManTreeItem *pProfileItem = new ManTreeItem( QString("Profile"));
    pProfileItem->setIcon( QIcon(":/images/policy.png"));
    pProfileItem->setType( ITEM_TYPE_CERT_PROFILE );
    pTopItem->appendRow( pProfileItem );

    ManTreeItem *pCertProfileItem = new ManTreeItem( QString("CertProfile"));
    pCertProfileItem->setIcon( QIcon(":/images/cert_profile.png"));
    pCertProfileItem->setType( ITEM_TYPE_CERT_PROFILE );
    pProfileItem->appendRow( pCertProfileItem );

    ManTreeItem *pCRLProfileItem = new ManTreeItem( QString("CRLProfile"));
    pCRLProfileItem->setIcon( QIcon(":/images/crl_profile.png"));
    pCRLProfileItem->setType( ITEM_TYPE_CRL_PROFILE );
    pProfileItem->appendRow( pCRLProfileItem );

    ManTreeItem *pSignerItem = new ManTreeItem( QString("Signer"));
    pSignerItem->setIcon( QIcon(":/images/signer.png"));
    pSignerItem->setType( ITEM_TYPE_REG_SIGNER );
    pTopItem->appendRow( pSignerItem );

    ManTreeItem *pRegSignerItem = new ManTreeItem( QString("RegSigner"));
    pRegSignerItem->setIcon( QIcon(":/images/reg_signer.png"));
    pRegSignerItem->setType( ITEM_TYPE_REG_SIGNER );
    pSignerItem->appendRow( pRegSignerItem );

    ManTreeItem *pOCSPSignerItem = new ManTreeItem( QString("OCSPSigner"));
    pOCSPSignerItem->setIcon( QIcon(":/images/ocsp_signer.png"));
    pOCSPSignerItem->setType( ITEM_TYPE_OCSP_SIGNER );
    pSignerItem->appendRow( pOCSPSignerItem );

    ManTreeItem *pCAItem = new ManTreeItem( QString( "CA" ) );
    pCAItem->setType( ITEM_TYPE_CA );
    pCAItem->setIcon( QIcon(":/images/ca.png"));
    pTopItem->appendRow( pCAItem );

    ManTreeItem *pUserItem = new ManTreeItem( QString("User") );
    pUserItem->setIcon(QIcon(":/images/user_reg.png"));
    pUserItem->setType( ITEM_TYPE_USER );
    pCAItem->appendRow( pUserItem );

    ManTreeItem *pCertItem = new ManTreeItem( QString("Certificate"));
    pCertItem->setType( ITEM_TYPE_CERT );
    pCertItem->setIcon(QIcon(":/images/cert.png"));
 //   pTopItem->appendRow( pCertItem );
    pCAItem->appendRow( pCertItem );

    ManTreeItem *pCRLItem = new ManTreeItem( QString("CRL") );
    pCRLItem->setType( ITEM_TYPE_CRL );
    pCRLItem->setIcon(QIcon(":/images/crl.png"));
//    pTopItem->appendRow( pCRLItem );
    pCAItem->appendRow( pCRLItem );

    ManTreeItem *pRevokeItem = new ManTreeItem( QString("Revoke"));
    pRevokeItem->setType( ITEM_TYPE_REVOKE );
    pRevokeItem->setIcon(QIcon(":/images/revoke.png"));
//    pTopItem->appendRow( pRevokeItem );
    pCAItem->appendRow( pRevokeItem );


    ManTreeItem *pAuditItem = new ManTreeItem( QString( "Audit") );
    pAuditItem->setIcon( QIcon(":/images/audit.png"));
    pAuditItem->setType( ITEM_TYPE_AUDIT );
    pTopItem->appendRow( pAuditItem );

#ifdef _ENABLE_CHARTS
    ManTreeItem *pStatisticsItem = new ManTreeItem( QString( "Statistics" ));
    pStatisticsItem->setIcon(QIcon(":/images/statistics.png"));
    pStatisticsItem->setType( ITEM_TYPE_STATISTICS );
    pAuditItem->appendRow( pStatisticsItem );
#endif

    ManTreeItem *pKMSItem = new ManTreeItem( QString( "KMS" ));
    pKMSItem->setIcon(QIcon(":/images/kms.png"));
    pKMSItem->setType( ITEM_TYPE_KMS );
    pTopItem->appendRow( pKMSItem );

    ManTreeItem *pTSPItem = new ManTreeItem( QString( "TSP" ));
    pTSPItem->setIcon(QIcon(":/images/timestamp.png"));
    pTSPItem->setType( ITEM_TYPE_TSP );
    pTopItem->appendRow( pTSPItem );

    ManTreeItem *pLCNItem = new ManTreeItem( QString( "License") );
    pLCNItem->setIcon( QIcon(":/images/license.png"));
    pLCNItem->setType( ITEM_TYPE_LICENSE );
    pTopItem->appendRow( pLCNItem );

    QModelIndex ri = left_model_->index(0,0);
//    left_tree_->expand(ri);
    left_tree_->expandAll();
}

void MainWindow::createRightList(int nItemType)
{
#ifdef _ENABLE_CHARTS
    stack_->setCurrentIndex(0);
#endif

    if( nItemType == ITEM_TYPE_CERT_PROFILE ||
            nItemType == ITEM_TYPE_CRL_PROFILE ||
            nItemType == ITEM_TYPE_REG_SIGNER ||
            nItemType == ITEM_TYPE_OCSP_SIGNER ||
            nItemType == ITEM_TYPE_CA )
    {
        right_menu_->hide();
    }
    else
    {
        right_menu_->show();
    }

    if( nItemType == ITEM_TYPE_ADMIN )
        createRightAdminList();
    else if( nItemType == ITEM_TYPE_CONFIG )
        createRightConfigList();
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
#ifdef _ENABLE_CHARTS
    else if( nItemType == ITEM_TYPE_STATISTICS )
        createRightStatistics();
#endif
    else if( nItemType == ITEM_TYPE_AUDIT )
        createRightAudit();
    else if( nItemType == ITEM_TYPE_LICENSE )
        createRightLCNList();
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

    right_table_->setColumnWidth( 0, 60 );
    right_table_->setColumnWidth( 1, 60 );
    right_table_->setColumnWidth( 2, 60 );
    right_table_->setColumnWidth( 3, 160 );
    right_table_->setColumnWidth( 4, 160 );

    while( pCurList )
    {
        right_table_->insertRow(i);

        QTableWidgetItem *item = new QTableWidgetItem( pCurList->sAdmin.pName );
        QTableWidgetItem *seq = new QTableWidgetItem( QString("%1").arg( pCurList->sAdmin.nSeq ));
        seq->setIcon(QIcon(":/images/admin.png"));

        right_table_->setRowHeight(i, 10 );
        right_table_->setItem( i, 0, seq );
        right_table_->setItem( i, 1, new QTableWidgetItem( QString("%1").arg( getStatusName( pCurList->sAdmin.nStatus ) ) ));
        right_table_->setItem( i, 2, new QTableWidgetItem( QString("%1").arg( getAdminTypeName( pCurList->sAdmin.nType ) )));
        right_table_->setItem( i, 3, item );
        right_table_->setItem( i, 4, new QTableWidgetItem( QString("%1").arg( pCurList->sAdmin.pPassword )));
        right_table_->setItem( i, 5, new QTableWidgetItem( pCurList->sAdmin.pEmail ));

        pCurList = pCurList->pNext;
        i++;
    }

    if( pAdminList ) JS_DB_resetAdminList( &pAdminList );
}

void MainWindow::createRightConfigList()
{
    int i = 0;
    removeAllRight();
    JDB_ConfigList  *pConfigList = NULL;
    JDB_ConfigList  *pCurList = NULL;

    QStringList titleList = { tr("Num"), tr("Kind"), tr("Name"), tr("Value") };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setType( ITEM_TYPE_CONFIG );

    right_table_->setColumnCount(titleList.size());
    right_table_->setHorizontalHeaderLabels( titleList );
    right_table_->verticalHeader()->setVisible(false);

    manApplet->ccClient()->getConfigList( &pConfigList );
    pCurList = pConfigList;

    right_table_->setColumnWidth( 0, 60 );
    right_table_->setColumnWidth( 1, 60 );
    right_table_->setColumnWidth( 2, 60 );


    while( pCurList )
    {
        right_table_->insertRow(i);

        QTableWidgetItem *item = new QTableWidgetItem( pCurList->sConfig.pName );
        QTableWidgetItem *seq = new QTableWidgetItem( QString("%1").arg( pCurList->sConfig.nNum ));
        seq->setIcon(QIcon(":/images/config.png"));

        right_table_->setRowHeight(i, 10 );
        right_table_->setItem( i, 0, seq );
        right_table_->setItem( i, 1, new QTableWidgetItem( QString("%1").arg( pCurList->sConfig.nKind ) ));
        right_table_->setItem( i, 2, item );
        right_table_->setItem( i, 3, new QTableWidgetItem( QString("%1").arg( pCurList->sConfig.pValue )));

        pCurList = pCurList->pNext;
        i++;
    }

    if( pConfigList ) JS_DB_resetConfigList( &pConfigList );
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

    JDB_UserList    *pDBUserList = NULL;
    JDB_UserList    *pCurList = NULL;

    removeAllRight();

    QStringList titleList = { tr("Num"), tr("RegTime"), tr("Status"), tr("Name"), tr("Email")  };


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

    right_table_->setColumnWidth( 0, 60 );
    right_table_->setColumnWidth( 1, 130 );
    right_table_->setColumnWidth( 2, 60 );
    right_table_->setColumnWidth( 3, 180 );


    while( pCurList )
    {
        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        QTableWidgetItem *item = new QTableWidgetItem( pCurList->sUser.pName );
        item->setIcon(QIcon(":/images/user_reg.png"));

        right_table_->setItem(i,0, new QTableWidgetItem(QString("%1").arg( pCurList->sUser.nNum )));
        right_table_->setItem(i,1, new QTableWidgetItem(QString("%1").arg( dateString( pCurList->sUser.nRegTime )) ));
        right_table_->setItem(i,2, new QTableWidgetItem(QString("%1").arg( getUserStatusName( pCurList->sUser.nStatus ) )));
        right_table_->setItem(i,3, item );
        right_table_->setItem(i,4, new QTableWidgetItem(QString("%1").arg( pCurList->sUser.pEmail )));


        pCurList = pCurList->pNext;
        i++;
    }

    right_menu_->setTotalCount( nTotalCnt );
    right_menu_->updatePageLabel();
    if( pDBUserList ) JS_DB_resetUserList( &pDBUserList );
}

void MainWindow::createRightCertProfileList()
{
    int i = 0;
    removeAllRight();
    JDB_CertProfileList  *pCertProfileList = NULL;
    JDB_CertProfileList  *pCurList = NULL;

    QStringList titleList = { tr("Num"), tr("NotBefore"), tr("NotAfter"), tr("Hash"), tr("Name") };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setType( ITEM_TYPE_CERT_PROFILE );

    right_table_->setColumnCount(titleList.size());
    right_table_->setHorizontalHeaderLabels( titleList );
    right_table_->verticalHeader()->setVisible(false);

    manApplet->ccClient()->getCertProfileList( 0, &pCertProfileList );
    pCurList = pCertProfileList;

    right_table_->setColumnWidth( 0, 60 );
    right_table_->setColumnWidth( 1, 100 );
    right_table_->setColumnWidth( 2, 100 );
    right_table_->setColumnWidth( 3, 80 );


    while( pCurList )
    {
        QString strVersion;
        QString strNotBefore;
        QString strNotAfter;
        QString strDNTemplate;

        QTableWidgetItem *item = new QTableWidgetItem( pCurList->sCertProfile.pName );
        QTableWidgetItem *seq = new QTableWidgetItem( QString("%1").arg( pCurList->sCertProfile.nNum ));
        seq->setIcon(QIcon(":/images/cert_profile.png"));

        strVersion = QString( "V%1" ).arg( pCurList->sCertProfile.nVersion + 1);

        /*
        if( pCurList->sCertProfile.nNotBefore == 0 )
        {
            strNotBefore = "GenTime";
            strNotAfter = QString( "%1 Days" ).arg( pCurList->sCertProfile.nNotAfter );
        }
        else
        {
            strNotBefore = getDateTime( pCurList->sCertProfile.nNotBefore );
            strNotAfter = getDateTime( pCurList->sCertProfile.nNotAfter );
        }
        */

        getPeriodString( pCurList->sCertProfile.nNotBefore,
                        pCurList->sCertProfile.nNotAfter,
                        strNotBefore,
                        strNotAfter );

        if( strcasecmp( pCurList->sCertProfile.pDNTemplate, "#CSR_DN" ) == 0 )
            strDNTemplate = "Use CSR DN";
        else
            strDNTemplate = pCurList->sCertProfile.pDNTemplate;

        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        right_table_->setItem( i, 0, seq );
        right_table_->setItem( i, 1, new QTableWidgetItem( QString("%1").arg( strNotBefore )));
        right_table_->setItem( i, 2, new QTableWidgetItem( QString("%1").arg( strNotAfter )));
        right_table_->setItem( i, 3, new QTableWidgetItem( pCurList->sCertProfile.pHash ));
        right_table_->setItem( i, 4, item );

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

    QStringList titleList = { tr("Num"), tr("ThisUpdate"), tr("NextUpdate"), tr("Hash"), tr("Name") };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setType(ITEM_TYPE_CRL_PROFILE);

    right_table_->setColumnCount( titleList.size() );
    right_table_->setHorizontalHeaderLabels( titleList );
    right_table_->verticalHeader()->setVisible(false);

    manApplet->ccClient()->getCRLProfileList( &pCRLProfileList );
    pCurList = pCRLProfileList;

    right_table_->setColumnWidth( 0, 60 );
    right_table_->setColumnWidth( 1, 100 );
    right_table_->setColumnWidth( 2, 100 );
    right_table_->setColumnWidth( 3, 80 );


    while( pCurList )
    {
        QString strVersion;
        QString strThisUpdate;
        QString strNextUpdate;

        QTableWidgetItem *item = new QTableWidgetItem( pCurList->sCRLProfile.pName );
        QTableWidgetItem *seq = new QTableWidgetItem( QString("%1").arg( pCurList->sCRLProfile.nNum ));
        seq->setIcon(QIcon(":/images/crl_profile.png"));

        strVersion = QString( "V%1" ).arg( pCurList->sCRLProfile.nVersion + 1);

        if( pCurList->sCRLProfile.nThisUpdate == 0 )
        {
            strThisUpdate = "GenTime";
            strNextUpdate = QString( "%1 Days" ).arg( pCurList->sCRLProfile.nNextUpdate );
        }
        else
        {
            strThisUpdate = getDateTime( pCurList->sCRLProfile.nThisUpdate );
            strNextUpdate = getDateTime( pCurList->sCRLProfile.nNextUpdate );
        }

        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        right_table_->setItem( i, 0, seq );
        right_table_->setItem( i, 1, new QTableWidgetItem( QString("%1").arg( strThisUpdate )) );
        right_table_->setItem( i, 2, new QTableWidgetItem( QString("%1").arg( strNextUpdate )) );
        right_table_->setItem( i, 3, new QTableWidgetItem( pCurList->sCRLProfile.pHash) );
        right_table_->setItem( i, 4, item );

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

    QStringList titleList = { tr("Num"), tr("RegTime"), tr("Type"), tr("Status"), tr("DN") };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setType( nItemType );

    right_table_->setColumnCount(titleList.size());
    right_table_->setHorizontalHeaderLabels(titleList);
    right_table_->verticalHeader()->setVisible(false);

    manApplet->ccClient()->getSignerList( nType, &pSignerList );
    pCurList = pSignerList;

    right_table_->setColumnWidth( 0, 60 );
    right_table_->setColumnWidth( 1, 130 );
    right_table_->setColumnWidth( 2, 80 );
    right_table_->setColumnWidth( 3, 80 );

    while( pCurList )
    {
        QString strDate;
        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        strDate = dateString( pCurList->sSigner.nRegTime );

        QTableWidgetItem *item = new QTableWidgetItem( pCurList->sSigner.pDN );

        if( nType == 0 )
            item->setIcon(QIcon(":/images/reg_signer.png"));
        else if( nType == 1 )
            item->setIcon(QIcon(":/images/ocsp_signer.png"));

        right_table_->setItem(i,0, new QTableWidgetItem(QString("%1").arg( pCurList->sSigner.nNum )));
        right_table_->setItem(i,1, new QTableWidgetItem(QString("%1").arg( strDate )));
        right_table_->setItem(i,2, new QTableWidgetItem(QString("%1").arg( getSignerTypeName( pCurList->sSigner.nType ) )));
        right_table_->setItem(i,3, new QTableWidgetItem(QString("%1").arg( getStatusName(pCurList->sSigner.nStatus) )));
        right_table_->setItem(i,4, item );

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


    QString strTarget = right_menu_->getCondName();
    QString strWord = right_menu_->getInputWord();

    QStringList titleList = { tr("Num"), tr("RegTime"), tr("Serial"), tr("SubjectDN") };

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

    right_table_->setColumnWidth( 0, 60 );
    right_table_->setColumnWidth( 1, 130 );
    right_table_->setColumnWidth( 2, 100 );

    while( pCurList )
    {
        QString strDNInfo;
        if( pCurList->sCert.bSelf ) strDNInfo += "[Self]";

        strDNInfo += QString( "[%1] " ).arg( pCurList->sCert.nStatus );
        strDNInfo += pCurList->sCert.pSubjectDN;

        QTableWidgetItem *item = new QTableWidgetItem( strDNInfo );
        if( pCurList->sCert.bCA )
            item->setIcon(QIcon(":/images/ca.png"));
        else
            item->setIcon(QIcon(":/images/cert.png"));

        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        right_table_->setItem( i, 0, new QTableWidgetItem( QString("%1").arg( pCurList->sCert.nNum) ));
        right_table_->setItem( i, 1, new QTableWidgetItem( QString("%1").arg( dateString( pCurList->sCert.nRegTime ) ) ));
        right_table_->setItem( i, 2, new QTableWidgetItem( QString("%1").arg(pCurList->sCert.pSerial)));
        right_table_->setItem( i, 3, item );

        pCurList = pCurList->pNext;
        i++;
    }

    right_menu_->setTotalCount( nTotalCnt );
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


    QString strTarget = right_menu_->getCondName();
    QString strWord = right_menu_->getInputWord();

    QStringList titleList = { tr("Num"), tr("RegTime"), tr("IssuerNum"), tr("CRLDP") };

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

    right_table_->setColumnWidth( 0, 60 );
    right_table_->setColumnWidth( 1, 130 );
    right_table_->setColumnWidth( 2, 200 );

    pCurList = pCRLList;

    while( pCurList )
    {
        JCC_Cert sIssuer;
        QString strIssuerName;

        memset( &sIssuer, 0x00, sizeof(sIssuer));

        manApplet->ccClient()->getCert( pCurList->sCRL.nIssuerNum, &sIssuer );
        strIssuerName = sIssuer.pSubjectDN;

        QTableWidgetItem *item = new QTableWidgetItem( strIssuerName );
        item->setIcon(QIcon(":/images/crl.png"));

        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        right_table_->setItem( i, 0, new QTableWidgetItem( QString("%1").arg( pCurList->sCRL.nNum )));
        right_table_->setItem( i, 1, new QTableWidgetItem( QString("%1").arg( dateString( pCurList->sCRL.nRegTime ) )));
        right_table_->setItem( i, 2, item );
        right_table_->setItem( i, 3, new QTableWidgetItem( pCurList->sCRL.pCRLDP ));

        JS_DB_resetCert( &sIssuer );
        pCurList = pCurList->pNext;
        i++;
    }

    right_menu_->setTotalCount( nTotalCnt );
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

    CCClient *ccClient = manApplet->ccClient();


    QString strTarget = right_menu_->getCondName();
    QString strWord = right_menu_->getInputWord();

    QStringList titleList = { tr("Num"), tr("Serial"), tr("RevokedDate"), tr("Reason"), tr("CertNum") };

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

    right_table_->setColumnWidth( 0, 60 );
    right_table_->setColumnWidth( 1, 120 );
    right_table_->setColumnWidth( 2, 130 );
    right_table_->setColumnWidth( 3, 100 );

    pCurList = pRevokedList;

    while( pCurList )
    {
        QString strCertName = ccClient->getDN( pCurList->sRevoked.nCertNum );
        QString strIssuerName = ccClient->getDN( pCurList->sRevoked.nIssuerNum );

        QTableWidgetItem *item = new QTableWidgetItem( strCertName );
        item->setIcon(QIcon(":/images/revoke.png"));

        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        right_table_->setItem(i,0, new QTableWidgetItem(QString("%1").arg( pCurList->sRevoked.nSeq )));
        right_table_->setItem(i,1, new QTableWidgetItem(QString("%1").arg( pCurList->sRevoked.pSerial )));
        right_table_->setItem(i,2, new QTableWidgetItem(QString("%1").arg( dateString( pCurList->sRevoked.nRevokedDate ) )));
        right_table_->setItem(i,3, new QTableWidgetItem(QString("%1").arg( JS_PKI_getRevokeReasonName( pCurList->sRevoked.nReason ))));
        right_table_->setItem(i,4, item );

        pCurList = pCurList->pNext;
        i++;
    }


    right_menu_->setTotalCount( nTotalCnt );
    right_menu_->updatePageLabel();
    if( pRevokedList ) JS_DB_resetRevokedList( &pRevokedList );
}

void MainWindow::createRightCA()
{
    int ret = 0;
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

    ret = JS_PKI_getCertInfo( &binCert, &sCertInfo, &pExtInfoList );
    if( ret != 0 )
    {
        manApplet->warningBox( tr( "fail to decode certificate" ), this );
        goto end;
    }

    right_table_->insertRow(i);
    right_table_->setRowHeight(i, 10 );
    right_table_->setItem( i, 0, new QTableWidgetItem( "Version" ));
    right_table_->setItem( i, 1, new QTableWidgetItem(QString("%1").arg(sCertInfo.nVersion + 1)));

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

end :
    JS_BIN_reset( &binCert );
    JS_CC_resetNameVal( &sNameVal );
    JS_PKI_resetCertInfo( &sCertInfo );
    if( pExtInfoList ) JS_PKI_resetExtensionInfoList( &pExtInfoList );
}

#ifdef _ENABLE_CHARTS
void MainWindow::createRightStatistics()
{
    stack_->setCurrentIndex(1);
}
#endif

void MainWindow::createRightKMS()
{
    log( "KMS" );

    removeAllRight();

    int i = 0;
    int nLimit = manApplet->settingsMgr()->listCount();
    int nPage = right_menu_->curPage();
    int nOffset = nPage * nLimit;
    int nTotalCnt = 0;

    CCClient* ccClient = manApplet->ccClient();

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

    right_table_->setColumnWidth( 0, 60 );
    right_table_->setColumnWidth( 1, 130 );
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

        right_table_->setItem(i,0, new QTableWidgetItem(QString("%1").arg( pCurList->sKMS.nSeq )));
        right_table_->setItem(i,1, new QTableWidgetItem(QString("%1").arg( dateString( pCurList->sKMS.nRegTime ) )));
        right_table_->setItem(i,2, new QTableWidgetItem(QString("%1").arg( getStatusName( pCurList->sKMS.nState ))));
        right_table_->setItem(i,3, new QTableWidgetItem(QString("%1").arg( strType )));
        right_table_->setItem(i,4, new QTableWidgetItem(QString("%1").arg( strAlg )));
        right_table_->setItem(i,5, new QTableWidgetItem(QString("%1").arg( pCurList->sKMS.pID )));
        right_table_->setItem(i,6, new QTableWidgetItem(QString("%1").arg( pCurList->sKMS.pInfo )));

        pCurList = pCurList->pNext;
        i++;
    }


    right_menu_->setTotalCount( nTotalCnt );
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

    right_table_->setColumnWidth( 0, 60 );
    right_table_->setColumnWidth( 1, 130 );
    right_table_->setColumnWidth( 2, 100 );
    right_table_->setColumnWidth( 3, 200 );

    pCurList = pTSPList;

    while( pCurList )
    {
        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        right_table_->setItem(i,0, new QTableWidgetItem(QString("%1").arg( pCurList->sTSP.nSeq )));
        right_table_->setItem(i,1, new QTableWidgetItem(QString("%1").arg( dateString(pCurList->sTSP.nRegTime) )));
        right_table_->setItem(i,2, new QTableWidgetItem(QString("%1").arg( pCurList->sTSP.nSerial )));
        right_table_->setItem(i,3, new QTableWidgetItem(QString("%1").arg( pCurList->sTSP.pSrcHash )));
        right_table_->setItem(i,4, new QTableWidgetItem(QString("%1").arg( pCurList->sTSP.pPolicy )));


        pCurList = pCurList->pNext;
        i++;
    }


    right_menu_->setTotalCount( nTotalCnt );
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

    QString strTarget = right_menu_->getCondName();
    QString strWord = right_menu_->getInputWord();

    QStringList titleList = { tr("Seq"), tr("RegTime"), tr("Kind"), tr("Operation"), tr("UserName"), tr("MAC") };

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

    right_table_->setColumnWidth( 0, 60 );
    right_table_->setColumnWidth( 1, 130 );
    right_table_->setColumnWidth( 2, 100 );
    right_table_->setColumnWidth( 3, 100 );
    right_table_->setColumnWidth( 4, 100 );

    while( pCurList )
    {
        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        QString strKind = JS_GEN_getKindName( pCurList->sAudit.nKind );
        QString strOper = JS_GEN_getOperationName( pCurList->sAudit.nOperation );


        QTableWidgetItem *item = new QTableWidgetItem( strOper );
        item->setIcon(QIcon(":/images/audit.png"));

        right_table_->setItem(i,0, new QTableWidgetItem(QString("%1").arg( pCurList->sAudit.nSeq )));
        right_table_->setItem(i,1, new QTableWidgetItem(QString("%1").arg( dateString(pCurList->sAudit.nRegTime) )));
        right_table_->setItem(i,2, new QTableWidgetItem(QString("%1").arg( strKind )));
        right_table_->setItem(i,3, item );
        right_table_->setItem(i,4, new QTableWidgetItem(QString("%1").arg( pCurList->sAudit.pUserName )));
        right_table_->setItem(i,5, new QTableWidgetItem(QString("%1").arg( pCurList->sAudit.pMAC )));

        pCurList = pCurList->pNext;
        i++;
    }


    right_menu_->setTotalCount( nTotalCnt );
    right_menu_->updatePageLabel();
    if( pAuditList ) JS_DB_resetAuditList( &pAuditList );
}

void MainWindow::createRightLCNList()
{
    removeAllRight();

    int i = 0;
    int nLimit = manApplet->settingsMgr()->listCount();
    int nPage = right_menu_->curPage();
    int nOffset = nPage * nLimit;
    int nTotalCnt = 0;
    char    sDateTime[64];
    CCClient *ccClient = manApplet->ccClient();


    QString strTarget = right_menu_->getCondName();
    QString strWord = right_menu_->getInputWord();

    QStringList titleList = { tr("Num"), tr("RegDate"), tr("SID"), tr("User"), tr("Product"), tr("Status") };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setColumnCount( titleList.size() );
    right_table_->setHorizontalHeaderLabels(titleList);
    right_table_->verticalHeader()->setVisible(false);

    JCC_LCNList     *pLCNList = NULL;
    JCC_LCNList     *pCurList = NULL;

    if( strTarget.length() > 0 && strWord.length() > 0 )
    {
        nTotalCnt = manApplet->ccClient()->searchCount( ITEM_TYPE_LICENSE, strTarget, strWord );
        manApplet->ccClient()->searchLCNList( strTarget, strWord, nOffset, nLimit, &pLCNList );
    }
    else
    {
        nTotalCnt = manApplet->ccClient()->getCount( ITEM_TYPE_LICENSE );
        manApplet->ccClient()->getLCNList( nOffset, nLimit, &pLCNList );
    }

    right_table_->setColumnWidth( 0, 40 );
    right_table_->setColumnWidth( 1, 120 );
    right_table_->setColumnWidth( 2, 180 );
    right_table_->setColumnWidth( 3, 80 );
    right_table_->setColumnWidth( 4, 180 );


    pCurList = pLCNList;

    while( pCurList )
    {
        QTableWidgetItem *item = new QTableWidgetItem( pCurList->sLCN.pSID );
        item->setIcon(QIcon(":/images/license.png"));

        JS_UTIL_getDateTime( pCurList->sLCN.nRegTime, sDateTime );

        right_table_->insertRow(i);
        right_table_->setRowHeight(i, 10 );

        right_table_->setItem(i,0, new QTableWidgetItem(QString("%1").arg( pCurList->sLCN.nSeq )));
        right_table_->setItem(i,1, new QTableWidgetItem(QString("%1").arg( sDateTime )));
        right_table_->setItem(i,2, item );
        right_table_->setItem(i,3, new QTableWidgetItem(QString("%1").arg( pCurList->sLCN.pUser )));
        right_table_->setItem(i,4, new QTableWidgetItem(QString("%1").arg( pCurList->sLCN.pProduct )));
        right_table_->setItem(i,5, new QTableWidgetItem(QString("%1").arg( pCurList->sLCN.nStatus )));

        pCurList = pCurList->pNext;
        i++;
    }


    right_menu_->setTotalCount( nTotalCnt );
    right_menu_->updatePageLabel();
    if( pLCNList ) JS_DB_resetLCNList( &pLCNList );
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
    log_text_->setPlainText("");

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
    if( item == NULL ) return;

    int num = item->text().toInt();

    MakeCertProfileDlg makeCertProfileDlg;
    makeCertProfileDlg.setEdit(num);
    makeCertProfileDlg.exec();
}

void MainWindow::modifyCRLProfile()
{
    int row = right_table_->currentRow();
    QTableWidgetItem* item = right_table_->item( row, 0 );
    if( item == NULL ) return;

    int num = item->text().toInt();

    MakeCRLProfileDlg makeCRLProfileDlg;
    makeCRLProfileDlg.setEdit(num);
    makeCRLProfileDlg.exec();
}

void MainWindow::deleteCertProfile()
{
    int row = right_table_->currentRow();
    QTableWidgetItem* item = right_table_->item( row, 0 );
    if( item == NULL ) return;

    int num = item->text().toInt();

    manApplet->ccClient()->delCertProfile( num );
    createRightCertProfileList();
}

void MainWindow::deleteCRLProfile()
{
    int row = right_table_->currentRow();
    QTableWidgetItem* item = right_table_->item( row, 0 );
    if( item == NULL ) return;

    int num = item->text().toInt();

    manApplet->ccClient()->delCRLProfile( num );
    createRightCRLProfileList();
}

void MainWindow::copyCertProfile()
{
    int row = right_table_->currentRow();
    QTableWidgetItem* item = right_table_->item( row, 0 );
    if( item == NULL ) return;

    int num = item->text().toInt();

    MakeCertProfileDlg makeCertProfileDlg;
    makeCertProfileDlg.loadProfile( num, true );
    makeCertProfileDlg.exec();
}

void MainWindow::copyCRLProfile()
{
    int row = right_table_->currentRow();
    QTableWidgetItem* item = right_table_->item( row, 0 );
    if( item == NULL ) return;

    int num = item->text().toInt();

    MakeCRLProfileDlg makeCRLProfileDlg;
    makeCRLProfileDlg.loadProfile( num, true );
    makeCRLProfileDlg.exec();
}

void MainWindow::deleteSigner()
{
    int row = right_table_->currentRow();
    QTableWidgetItem* item = right_table_->item( row, 0 );
    if( item == NULL ) return;

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
    if( item == NULL ) return;

    int num = item->text().toInt();

    RevokeCertDlg revokeCertDlg;
    revokeCertDlg.setCertNum(num);
    revokeCertDlg.exec();
}

void MainWindow::deleteRevoke()
{
    int row = right_table_->currentRow();
    QTableWidgetItem* item = right_table_->item( row, 0 );
    if( item == NULL ) return;

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

void MainWindow::certCAInfo()
{
    BIN binCA = {0,0};
    JCC_NameVal sNameVal;
    memset( &sNameVal, 0x00, sizeof(sNameVal));

    manApplet->ccClient()->getCA( &sNameVal );
    JS_BIN_decodeHex( sNameVal.pValue, &binCA );

    CertInfoDlg certInfoDlg;
    certInfoDlg.setCertBin( &binCA );
    certInfoDlg.exec();

    JS_BIN_reset( &binCA );
    JS_CC_resetNameVal( &sNameVal );
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

int verifyAuditData( const JCC_Audit *pAudit )
{
    int ret = 0;
    char sData[1024];
    BIN binData = {0,0};
    BIN binKey = {0,0};
    BIN binHMAC = {0,0};
    BIN binSrcMAC = {0,0};

    if( pAudit == NULL ) return -1;

    sprintf( sData, "%d_%d_%d_%s_%d_%s",
             pAudit->nSeq,
             pAudit->nKind,
             pAudit->nOperation,
             pAudit->pInfo,
             pAudit->nRegTime,
             pAudit->pUserName );

    JS_BIN_set( &binData, (unsigned char *)sData, strlen(sData));
    JS_BIN_set( &binKey, (unsigned char *)JS_GEN_HMAC_KEY, strlen(JS_GEN_HMAC_KEY));
    JS_BIN_decodeHex( pAudit->pMAC, &binSrcMAC );

    ret = JS_PKI_genHMAC( "SHA256", &binData, &binKey, &binHMAC );
    if( ret != 0 ) goto end;

    ret = JS_BIN_cmp( &binHMAC, &binSrcMAC );

end :
    JS_BIN_reset( &binData );
    JS_BIN_reset( &binKey );
    JS_BIN_reset( &binHMAC );
    JS_BIN_reset( &binSrcMAC );

    return ret;
}

void MainWindow::makeConfig()
{
    ConfigDlg configDlg;
    configDlg.exec();
}

void MainWindow::editConfig()
{
    int row = right_table_->currentRow();
    QTableWidgetItem* item = right_table_->item( row, 0 );

    int num = item->text().toInt();

    ConfigDlg configDlg;
    configDlg.setCurNum( num );
    configDlg.exec();
}



void MainWindow::deleteConfig()
{
    bool bVal = manApplet->yesOrCancelBox( tr( "Are you sure to delete this config?" ), this, false );
    if( bVal == false ) return;

    int row = right_table_->currentRow();
    QTableWidgetItem* item = right_table_->item( row, 0 );
    if( item == NULL ) return;

    int num = item->text().toInt();

    manApplet->ccClient()->delConfig( num );
    createRightConfigList();
}

void MainWindow::makeLicense()
{
    MakeLCNDlg makeLCNDlg;
    makeLCNDlg.exec();
}

void MainWindow::deleteLicense()
{
    bool bVal = manApplet->yesOrCancelBox( tr( "Are you sure to delete this license?" ), this, false );
    if( bVal == false ) return;

    int row = right_table_->currentRow();
    QTableWidgetItem* item = right_table_->item( row, 0 );
    if( item == NULL ) return;

    int num = item->text().toInt();

    manApplet->ccClient()->delLCN( num );
    createRightLCNList();
}

void MainWindow::viewLicense()
{
    int row = right_table_->currentRow();
    QTableWidgetItem* item = right_table_->item( row, 0 );

    int num = item->text().toInt();

    LCNInfoDlg lcnInfoDlg;
    lcnInfoDlg.setSeq( num );
    lcnInfoDlg.exec();
}

void MainWindow::saveLicense()
{
    bool bPEM = false;
    QString selectedFilter;
    QString strFilter;
    QString fileName;
    QString strPath = "js_license.lcn";

    QFileDialog::Options options;
    options |= QFileDialog::DontUseNativeDialog;


    int row = right_table_->currentRow();
    QTableWidgetItem* item = right_table_->item( row, 0 );

    JCC_LCN sLCN;
    BIN binLCN = {0,0};
    memset( &sLCN, 0x00, sizeof(sLCN));

    int num = item->text().toInt();
    manApplet->ccClient()->getLCN( num, &sLCN );

    JS_BIN_decodeHex( sLCN.pLicense, &binLCN );
    if( binLCN.nLen < 1 )
    {
        manApplet->warningBox( tr( "License info is empty" ), this );
        goto end;
    }

    bPEM = manApplet->yesOrNoBox( "Are you save the license as PEM", this );


    fileName = QFileDialog::getSaveFileName( this,
                                                     tr("Save Files"),
                                                     strPath,
                                                     strFilter,
                                                     &selectedFilter,
                                                     options );

    if( fileName.length() > 0 )
    {
        if( bPEM == true )
            JS_LCN_fileWrite( &binLCN, fileName.toLocal8Bit().toStdString().c_str() );
        else
            JS_BIN_fileWrite( &binLCN, fileName.toLocal8Bit().toStdString().c_str() );

        manApplet->messageBox( tr( "licensed saved" ), this );
    }

end :
    JS_DB_resetLCN( &sLCN );
    JS_BIN_reset( &binLCN );
}

void MainWindow::verifyAudit()
{
    int ret = 0;
    JCC_Audit sAudit;

    int row = right_table_->currentRow();
    if( row < 0 ) return;

    QTableWidgetItem* item = right_table_->item( row, 0 );
    int num = item->text().toInt();

    memset( &sAudit, 0x00, sizeof(sAudit));
    ret = manApplet->ccClient()->getAudit( num, &sAudit );

    if( ret != 0 ) return;

    ret = verifyAuditData( &sAudit );
    if( ret == 0 )
        manApplet->messageBox( tr( "MAC Verify OK" ), this );
    else
        manApplet->warningBox( tr( "MAC is not valid" ), this );

    JS_DB_resetAudit( &sAudit );
}

void MainWindow::viewTSTInfo()
{
    int ret = 0;
    int row = right_table_->currentRow();
    QTableWidgetItem* item = right_table_->item( row, 0 );

    int num = item->text().toInt();

    TSTInfoDlg tstInfoDlg;
    tstInfoDlg.setSeq( num );
    tstInfoDlg.exec();
}

void MainWindow::verifyTSMessage()
{
    int ret = 0;
    BIN binTS = {0,0};
    BIN binCert = {0,0};
    BIN binData = {0,0};
    JDB_TSP sTSP;

    int row = right_table_->currentRow();
    QTableWidgetItem* item = right_table_->item( row, 0 );
    SettingsMgr *smgr = manApplet->settingsMgr();
    QString strVerify;

    int num = item->text().toInt();

    memset( &sTSP, 0x00, sizeof(sTSP));

    ret = manApplet->ccClient()->getTSP( num, &sTSP );
    if( ret != 0 )
    {
        manApplet->warningBox( "fail to get tsp message", this );
        goto end;
    }

    JS_BIN_decodeHex( sTSP.pData, &binTS );

    if( smgr )
    {
        JS_BIN_fileReadBER( smgr->getTSPSrvCertPath().toStdString().c_str(), &binCert );
    }

    ret = JS_PKCS7_verifySignedData( &binTS, &binCert, &binData );
    strVerify = QString( "Verify val:%1" ).arg( ret );

    manApplet->messageBox( strVerify );
end :
    JS_BIN_reset( &binTS );
    JS_BIN_reset( &binCert );
    JS_BIN_reset( &binData );
    JS_DB_resetTSP( &sTSP );
}

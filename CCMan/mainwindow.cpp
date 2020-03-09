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

#include "js_db.h"
#include "js_db_data.h"

const int kListCount = 5;

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
    right_text_ = new QTextEdit;
//    right_table_ = new QTableWidget;
    right_table_ = new ManRightWidget;

    left_tree_->setModel(left_model_);

    hsplitter_->addWidget(left_tree_);
    hsplitter_->addWidget(vsplitter_);

    vsplitter_->addWidget(right_table_);
    vsplitter_->addWidget(right_menu_);
    vsplitter_->addWidget(right_text_);

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

void MainWindow::createActions()
{
    QMenu *userMenu = menuBar()->addMenu(tr("&User"));
    QToolBar *userToolBar = addToolBar(tr("User"));

    const QIcon regIcon = QIcon::fromTheme( "reg-user", QIcon(":/images/user_add.png"));
    QAction *regUserAct = new QAction( regIcon, tr("&RegisterUser"), this );
    regUserAct->setStatusTip( tr("Register a user"));
    connect( regUserAct, &QAction::triggered, this, &MainWindow::regUser);
    userMenu->addAction(regUserAct);
    userToolBar->addAction(regUserAct);
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

    menu.exec(QCursor::pos());
}

void MainWindow::treeMenuClick(QModelIndex index )
{
    int nType = -1;

    ManTreeItem *pItem = (ManTreeItem *)left_model_->itemFromIndex(index);

    if( pItem == NULL ) return;

    right_menu_->setCurPage(0);

    nType = pItem->type();
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

    right_text_->setText( strVal );

    if( right_table_->type() == ITEM_TYPE_USER )
    {
        showRightBottomUser( nSeq );
    }
    else if( right_table_->type() == ITEM_TYPE_CERT_POLICY )
    {
        showRightBottomCertPolicy( nSeq );
    }
    else if( right_table_->type() == ITEM_TYPE_CRL_POLICY )
    {
        showRightBottomCRLPolicy( nSeq );
    }
    else if( right_table_->type() == ITEM_TYPE_REG_SIGNER || right_table_->type() == ITEM_TYPE_OCSP_SIGNER )
    {
        showRightBottomSigner( nSeq );
    }
}

void MainWindow::showRightBottomUser( int nSeq )
{
    QString strMsg;
    QString strPart;

    JCC_User sUser;
    memset( &sUser, 0x00, sizeof(sUser));

    manApplet->ccClient()->getUser( nSeq, &sUser );

    strMsg = "[ User information ]\n";

    strPart = QString( "Num: %1\n").arg( sUser.nNum);
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
    right_text_->setText( strMsg );
}

void MainWindow::showRightBottomCertPolicy( int nNum )
{
    QString strMsg;
    QString strPart;

    JCC_CertPolicy  sCertPolicy;

    memset( &sCertPolicy, 0x00, sizeof(sCertPolicy));
    manApplet->ccClient()->getCertPolicy( nNum, &sCertPolicy );

    strMsg = "[ Certificate policy information ]\n";

    strPart = QString( "Num: %1\n").arg( sCertPolicy.nNum );
    strMsg += strPart;

    strPart = QString( "Name: %1\n").arg( sCertPolicy.pName);
    strMsg += strPart;

    strPart = QString( "Version: %1\n").arg( sCertPolicy.nVersion );
    strMsg += strPart;

    strPart = QString( "NotBefore: %1\n").arg( sCertPolicy.nNotBefore );
    strMsg += strPart;

    strPart = QString( "NotAfter: %1\n").arg( sCertPolicy.nNotAfter );
    strMsg += strPart;

    strPart = QString( "Hash: %1\n").arg( sCertPolicy.pHash );
    strMsg += strPart;

    strPart = QString( "DNTemplate: %1\n").arg( sCertPolicy.pDNTemplate );
    strMsg += strPart;

    strMsg += "========= Extensions information ==========\n";


    JCC_PolicyExtList *pPolicyExtList = NULL;
    JCC_PolicyExtList *pCurList = NULL;

    manApplet->ccClient()->getCertPolicyExtList( nNum, &pPolicyExtList );

    pCurList = pPolicyExtList;

    while( pCurList )
    {
        strPart = QString( "%1 || %2 || %3 || %4\n")
                .arg(pCurList->sPolicyExt.nSeq)
                .arg(pCurList->sPolicyExt.bCritical)
                .arg(pCurList->sPolicyExt.pSN)
                .arg(pCurList->sPolicyExt.pValue);

        strMsg += strPart;
        pCurList = pCurList->pNext;
    }

    right_text_->setText( strMsg );
    JS_DB_resetCertPolicy( &sCertPolicy );
    if( pPolicyExtList ) JS_DB_resetPolicyExtList( &pPolicyExtList );
}

void MainWindow::showRightBottomCRLPolicy( int nNum )
{
    QString strMsg;
    QString strPart;

    JCC_CRLPolicy   sCRLPolicy;

    memset( &sCRLPolicy, 0x00, sizeof(sCRLPolicy));
    manApplet->ccClient()->getCRLPolicy( nNum, &sCRLPolicy );

    strMsg = "[ CRL information ]\n";

    strPart = QString( "Num: %1\n").arg(sCRLPolicy.nNum);
    strMsg += strPart;

    strPart = QString( "Name: %1\n").arg(sCRLPolicy.pName);
    strMsg += strPart;

    strPart = QString( "Version: %1\n").arg(sCRLPolicy.nVersion);
    strMsg += strPart;

    strPart = QString( "LastUpdate : %1\n").arg(sCRLPolicy.nLastUpdate);
    strMsg += strPart;

    strPart = QString("NextUpdate: %1\n").arg(sCRLPolicy.nNextUpdate);
    strMsg += strPart;

    strPart = QString("Hash: %1\n").arg(sCRLPolicy.pHash);
    strMsg += strPart;

    strMsg += "========= Extensions information ==========\n";

    JCC_PolicyExtList *pPolicyExtList = NULL;
    JCC_PolicyExtList *pCurList = NULL;

    manApplet->ccClient()->getCRLPolicyExtList( nNum, &pPolicyExtList );

    pCurList = pPolicyExtList;

    while( pCurList )
    {
        strPart = QString( "%1 || %2 || %3 || %4\n")
                .arg(pCurList->sPolicyExt.nSeq)
                .arg(pCurList->sPolicyExt.bCritical)
                .arg(pCurList->sPolicyExt.pSN)
                .arg(pCurList->sPolicyExt.pValue);

        strMsg += strPart;
        pCurList = pCurList->pNext;
    }

    right_text_->setText( strMsg );
    JS_DB_resetCRLPolicy( &sCRLPolicy );
    if( pPolicyExtList ) JS_DB_resetPolicyExtList( &pPolicyExtList );
}

void MainWindow::showRightBottomSigner(int nNum)
{
    QString strMsg;
    QString strPart;

    JCC_Signer  sSigner;
    memset( &sSigner, 0x00, sizeof(sSigner));

    manApplet->ccClient()->getSigner( nNum, &sSigner );

    strMsg = "[ Signer information ]\n";

    strPart = QString( "Num: %1\n").arg( sSigner.nNum);
    strMsg += strPart;

    strPart = QString( "Type: %1\n").arg( sSigner.nType );
    strMsg += strPart;

    strPart = QString( "DN: %1\n").arg( sSigner.pDN );
    strMsg += strPart;

    strPart = QString( "DNHash: %1\n").arg( sSigner.pDNHash );
    strMsg += strPart;

    strPart = QString( "Cert: %1\n").arg( sSigner.pCert );
    strMsg += strPart;

    strPart = QString( "Status: %1\n").arg( sSigner.nStatus );
    strMsg += strPart;

    strPart = QString( "Desc: %1\n").arg( sSigner.pDesc );
    strMsg += strPart;

    right_text_->setText( strMsg );

    JS_DB_resetSigner( &sSigner );
}

void MainWindow::createTreeMenu()
{
    left_model_->clear();
    left_tree_->header()->setVisible(false);

    ManTreeItem *pRootItem = (ManTreeItem *)left_model_->invisibleRootItem();


    ManTreeItem *pTopItem = new ManTreeItem( QString( "CertificateAuthority" ) );
    pTopItem->setIcon(QIcon(":/images/man.png"));
    pRootItem->insertRow( 0, pTopItem );

    ManTreeItem *pUserItem = new ManTreeItem( QString("User") );
    pUserItem->setIcon(QIcon(":/images/user.jpg"));
    pUserItem->setType( ITEM_TYPE_USER );
    pTopItem->appendRow( pUserItem );

    ManTreeItem *pCertPolicyItem = new ManTreeItem( QString("CertPolicy"));
    pCertPolicyItem->setIcon( QIcon(":/images/policy.png"));
    pCertPolicyItem->setType( ITEM_TYPE_CERT_POLICY );
    pTopItem->appendRow( pCertPolicyItem );

    ManTreeItem *pCRLPolicyItem = new ManTreeItem( QString("CRLPolicy"));
    pCRLPolicyItem->setIcon( QIcon(":/images/policy.png"));
    pCRLPolicyItem->setType( ITEM_TYPE_CRL_POLICY );
    pTopItem->appendRow( pCRLPolicyItem );

    ManTreeItem *pRegSignerItem = new ManTreeItem( QString("RegSigner"));
    pRegSignerItem->setIcon( QIcon(":/images/reg_signer.png"));
    pRegSignerItem->setType( ITEM_TYPE_REG_SIGNER );
    pTopItem->appendRow( pRegSignerItem );

    ManTreeItem *pOCSPSignerItem = new ManTreeItem( QString("OCSPSigner"));
    pOCSPSignerItem->setIcon( QIcon(":/images/ocsp_signer.png"));
    pOCSPSignerItem->setType( ITEM_TYPE_OCSP_SIGNER );
    pTopItem->appendRow( pOCSPSignerItem );

    QModelIndex ri = left_model_->index(0,0);
    left_tree_->expand(ri);
}

void MainWindow::createRightList(int nItemType)
{
    if( nItemType == ITEM_TYPE_USER )
        createRightUserList();
    else if( nItemType == ITEM_TYPE_CERT_POLICY )
        createRightCertPolicyList();
    else if( nItemType == ITEM_TYPE_CRL_POLICY )
        createRightCRLPolicyList();
    else if( nItemType == ITEM_TYPE_REG_SIGNER || nItemType == ITEM_TYPE_OCSP_SIGNER )
        createRightSignerList( nItemType );
}

void MainWindow::createRightUserList()
{
    int i = 0;
    int nLimit = kListCount;
    int nPage = right_menu_->curPage();
    int nOffset = nPage * nLimit;
    int nTotalCnt = manApplet->ccClient()->getCount( ITEM_TYPE_USER );

    right_menu_->setLimit( nLimit );
    right_menu_->setTotalCount( nTotalCnt );


    JDB_UserList    *pDBUserList = NULL;
    JDB_UserList    *pCurList = NULL;

    removeAllRight();

    QStringList titleList = { "Num", "Name", "SSN", "Email", "Status", "RefNum", "SecretCode" };


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
        right_table_->insertRow(i);

        right_table_->setItem(i,0, new QTableWidgetItem(QString("%1").arg( pCurList->sUser.nNum )));
        right_table_->setItem(i,1, new QTableWidgetItem(QString("%1").arg( pCurList->sUser.pName )));
        right_table_->setItem(i,2, new QTableWidgetItem(QString("%1").arg( pCurList->sUser.pSSN )));
        right_table_->setItem(i,3, new QTableWidgetItem(QString("%1").arg( pCurList->sUser.pEmail )));
        right_table_->setItem(i,4, new QTableWidgetItem(QString("%1").arg( pCurList->sUser.nStatus )));
        right_table_->setItem(i,5, new QTableWidgetItem(QString("%1").arg( pCurList->sUser.pRefNum )));
        right_table_->setItem(i,6, new QTableWidgetItem(QString("%1").arg( pCurList->sUser.pAuthCode )));

        pCurList = pCurList->pNext;
        i++;
    }

    right_menu_->updatePageLabel();
    if( pDBUserList ) JS_DB_resetUserList( &pDBUserList );
}

void MainWindow::createRightCertPolicyList()
{
    int i = 0;
    removeAllRight();
    JDB_CertPolicyList  *pCertPolicyList = NULL;
    JDB_CertPolicyList  *pCurList = NULL;

    QStringList titleList = { "Num", "Name", "Version", "NotBefore", "NotAfter", "Hash", "DNTemplate" };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setType( ITEM_TYPE_CERT_POLICY );

    right_table_->setColumnCount(titleList.size());
    right_table_->setHorizontalHeaderLabels( titleList );
    right_table_->verticalHeader()->setVisible(false);

    manApplet->ccClient()->getCertPolicyList( &pCertPolicyList );
    pCurList = pCertPolicyList;

    while( pCurList )
    {
        right_table_->insertRow(i);

        right_table_->setItem( i, 0, new QTableWidgetItem( QString("%1").arg( pCurList->sCertPolicy.nNum ) ));
        right_table_->setItem( i, 1, new QTableWidgetItem( pCurList->sCertPolicy.pName ));
        right_table_->setItem( i, 2, new QTableWidgetItem( QString("%1").arg( pCurList->sCertPolicy.nVersion )));
        right_table_->setItem( i, 3, new QTableWidgetItem( QString("%1").arg( pCurList->sCertPolicy.nNotBefore )));
        right_table_->setItem( i, 4, new QTableWidgetItem( QString("%1").arg( pCurList->sCertPolicy.nNotAfter )));
        right_table_->setItem( i, 5, new QTableWidgetItem( pCurList->sCertPolicy.pHash ));
        right_table_->setItem( i, 6, new QTableWidgetItem( pCurList->sCertPolicy.pDNTemplate ));

        pCurList = pCurList->pNext;
        i++;
    }

    if( pCertPolicyList ) JS_DB_resetCertPolicyList( &pCertPolicyList );
}

void MainWindow::createRightCRLPolicyList()
{
    int i = 0;
    removeAllRight();

    JDB_CRLPolicyList   *pCRLPolicyList = NULL;
    JDB_CRLPolicyList   *pCurList = NULL;

    QStringList titleList = { "Num", "Name", "Version", "LastUpdate", "NextUpdate", "Hash" };

    right_table_->clear();
    right_table_->horizontalHeader()->setStretchLastSection(true);
    right_table_->setType(ITEM_TYPE_CRL_POLICY);

    right_table_->setColumnCount( titleList.size() );
    right_table_->setHorizontalHeaderLabels( titleList );
    right_table_->verticalHeader()->setVisible(false);

    manApplet->ccClient()->getCRLPolicyList( &pCRLPolicyList );
    pCurList = pCRLPolicyList;

    while( pCurList )
    {
        right_table_->insertRow(i);
        right_table_->setItem( i, 0, new QTableWidgetItem( QString("%1").arg( pCurList->sCRLPolicy.nNum )) );
        right_table_->setItem( i, 1, new QTableWidgetItem( pCurList->sCRLPolicy.pName) );
        right_table_->setItem( i, 2, new QTableWidgetItem( QString("%1").arg( pCurList->sCRLPolicy.nVersion )) );
        right_table_->setItem( i, 3, new QTableWidgetItem( QString("%1").arg( pCurList->sCRLPolicy.nLastUpdate )) );
        right_table_->setItem( i, 4, new QTableWidgetItem( QString("%1").arg( pCurList->sCRLPolicy.nNextUpdate )) );
        right_table_->setItem( i, 5, new QTableWidgetItem( pCurList->sCRLPolicy.pHash) );

        pCurList = pCurList->pNext;
        i++;
    }

    if( pCRLPolicyList ) JS_DB_resetCRLPolicyList( &pCRLPolicyList );
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

    QStringList titleList = { "Num", "Type", "DN", "Status", "Cert" };

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
        right_table_->insertRow(i);

        right_table_->setItem(i,0, new QTableWidgetItem(QString("%1").arg( pCurList->sSigner.nNum )));
        right_table_->setItem(i,1, new QTableWidgetItem(QString("%1").arg( pCurList->sSigner.nType )));
        right_table_->setItem(i,2, new QTableWidgetItem(QString("%1").arg( pCurList->sSigner.pDN )));
        right_table_->setItem(i,3, new QTableWidgetItem(QString("%1").arg( pCurList->sSigner.nStatus )));
        right_table_->setItem(i,4, new QTableWidgetItem(QString("%1").arg( pCurList->sSigner.pCert )));

        pCurList = pCurList->pNext;
        i++;
    }

    if( pSignerList ) JS_DB_resetSignerList( &pSignerList );
}

void MainWindow::removeAllRight()
{
    right_text_->setText("");

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

int MainWindow::rightType()
{
    return right_table_->type();
}

int MainWindow::rightCount()
{
    return right_table_->rowCount();
}

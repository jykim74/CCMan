#include "issue_crl_dlg.h"
#include "mainwindow.h"
#include "man_applet.h"
#include "cc_client.h"
#include "js_cc.h"

IssueCRLDlg::IssueCRLDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    initialize();
}

IssueCRLDlg::~IssueCRLDlg()
{

}

void IssueCRLDlg::accept()
{
    int     ret = 0;
    int     nPolicyNum = -1;
    int     bDownload = 0;

    JCC_IssueCRLReq     sIssueReq;
    JCC_IssueCRLRsp     sIssueRsp;

    memset( &sIssueReq, 0x00, sizeof(sIssueReq));
    memset( &sIssueRsp, 0x00, sizeof(sIssueRsp));

    bDownload = mCRLDownloadCheck->isChecked();

    QVariant data = mCRLPolicyCombo->currentData();
    nPolicyNum = data.toInt();

    JS_CC_setIssueCRLReq( &sIssueReq, nPolicyNum, bDownload );

    ret = manApplet->ccClient()->issueCRL( &sIssueReq, &sIssueRsp );

    JS_CC_resetIssueCRLReq( &sIssueReq );
    JS_CC_resetIssueCRLRsp( &sIssueRsp );

    if( ret == 0 )
    {
        manApplet->mainWindow()->createRightCRLList();
        QDialog::accept();
    }
}

void IssueCRLDlg::clickDownload()
{

}

void IssueCRLDlg::initialize()
{
    connect( mCRLDownloadCheck, SIGNAL(clicked()), this, SLOT(clickDownload()));

    JCC_CRLPolicyList   *pPolicyList = NULL;
    JCC_CRLPolicyList   *pCurList = NULL;

    manApplet->ccClient()->getCRLPolicyList( &pPolicyList );

    pCurList = pPolicyList;

    while( pCurList )
    {
        QVariant data = pCurList->sCRLPolicy.nNum;
        mCRLPolicyCombo->addItem( pCurList->sCRLPolicy.pName, data );

        pCurList = pCurList->pNext;
    }

    if( pPolicyList ) JS_DB_resetCRLPolicyList( &pPolicyList );
}

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
    int     nProfileNum = -1;
    int     bDownload = 0;

    JCC_IssueCRLReq     sIssueReq;
    JCC_IssueCRLRsp     sIssueRsp;

    memset( &sIssueReq, 0x00, sizeof(sIssueReq));
    memset( &sIssueRsp, 0x00, sizeof(sIssueRsp));

    bDownload = mCRLDownloadCheck->isChecked();

    QVariant data = mCRLProfileCombo->currentData();
    nProfileNum = data.toInt();

    QString strCRLPD = mCRLDPCombo->currentText();

    JS_CC_setIssueCRLReq( &sIssueReq, nProfileNum, bDownload, strCRLPD.toStdString().c_str() );

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

    JCC_CRLProfileList   *pProfileList = NULL;
    JCC_CRLProfileList   *pCurList = NULL;
    JCC_NameValList     *pCRLDPList = NULL;
    JCC_NameValList     *pList = NULL;

    manApplet->ccClient()->getCRLProfileList( &pProfileList );

    pCurList = pProfileList;

    while( pCurList )
    {
        QVariant data = pCurList->sCRLProfile.nNum;
        mCRLProfileCombo->addItem( pCurList->sCRLProfile.pName, data );

        pCurList = pCurList->pNext;
    }

    manApplet->ccClient()->getCRLDPList( &pCRLDPList );

    pList = pCRLDPList;

    while( pList )
    {
        mCRLDPCombo->addItem( pList->sNameVal.pValue );
        pList = pList->pNext;
    }

    if( pProfileList ) JS_DB_resetCRLProfileList( &pProfileList );
    if( pCRLDPList ) JS_UTIL_resetNameValList( &pCRLDPList );
}

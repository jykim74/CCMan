#include "mainwindow.h"
#include "man_applet.h"
#include "js_cc.h"
#include "js_http.h"

#include "reg_user_dlg.h"
#include "account_info.h"
#include "cc_client.h"

RegUserDlg::RegUserDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
}

RegUserDlg::~RegUserDlg()
{

}

void RegUserDlg::accept()
{
    int     ret = 0;
    char    *pReq = NULL;
    char    *pRsp = NULL;
    const char *pContentType = "application/json";
    int     nStatusCode = -1;

    JCC_RegUserReq  sRegUserReq;
    JCC_RegUserRsp  sRegUserRsp;

    memset( &sRegUserReq, 0x00, sizeof(sRegUserReq));
    memset( &sRegUserRsp, 0x00, sizeof(sRegUserRsp));

    QString strName = mNameText->text();
    QString strSSN = mSSNText->text();
    QString strEmail = mEmailText->text();

    QString strURL = manApplet->ccClient()->baseUrl();
    strURL += JS_CC_PATH_USER;

    QString strToken = manApplet->accountInfo()->token();

    JS_CC_setRegUserReq( &sRegUserReq,
                         strName.toStdString().c_str(),
                         strSSN.toStdString().c_str(),
                         strEmail.toStdString().c_str() );

    JS_CC_encodeRegUserReq( &sRegUserReq, &pReq );

    ret = JS_HTTP_requestTokenPost(
                strURL.toStdString().c_str(),
                pContentType,
                strToken.toStdString().c_str(),
                pReq,
                &nStatusCode,
                &pRsp );

    JS_CC_decodeRegUserRsp( pRsp, &sRegUserRsp );

    JS_CC_resetRegUserReq( &sRegUserReq );
    if( pReq ) JS_free( pReq );
    if( pRsp ) JS_free( pRsp );

    if( nStatusCode == JS_HTTP_STATUS_OK )
    {
        QString strMsg = QString( "The success to register user(%1:%2)")
                .arg( sRegUserRsp.pRefNum)
                .arg( sRegUserRsp.pAuthCode );
        manApplet->messageBox( strMsg, this );
        JS_CC_resetRegUserRsp( &sRegUserRsp );
        QDialog::accept();
    }
    else
    {
        JS_CC_resetRegUserRsp( &sRegUserRsp );
        manApplet->warningBox(tr("fail to register user"), this );
        QDialog::reject();
    }

    manApplet->mainWindow()->createRightUserList();
}

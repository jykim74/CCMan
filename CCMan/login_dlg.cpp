#include "login_dlg.h"
#include "js_cc.h"
#include "js_ssl.h"
#include "js_net.h"
#include "js_util.h"
#include "js_http.h"

#include "mainwindow.h"
#include "man_applet.h"
#include "account_info.h"
#include "cc_client.h"


LoginDlg::LoginDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    QString strURL = "http://127.0.0.1:9050";
    QString strUserName = "admin";
    QString strPasswd = "admin";

    mServerURLCombo->addItem( strURL );
    mUserNameText->setText( strUserName );
    mPasswordText->setText( strPasswd );
}

LoginDlg::~LoginDlg()
{

}

void LoginDlg::accept()
{
    int ret = 0;


    char    *pReq = NULL;
    char    *pRsp = NULL;
    const char    *pContentType = "application/json";
    int     nStatusCode = -1;

    JCC_AuthReq sAuthReq;
    JCC_AuthRsp sAuthRsp;

    memset( &sAuthReq, 0x00, sizeof(sAuthReq));
    memset( &sAuthRsp, 0x00, sizeof(sAuthRsp));

    QString strUserName = mUserNameText->text();
    QString strPasswd = mPasswordText->text();
    QString strURL = mServerURLCombo->currentText();

    strURL += JS_CC_PATH_AUTH;

    JS_CC_setAuthReq( &sAuthReq, strUserName.toStdString().c_str(), strPasswd.toStdString().c_str() );
    JS_CC_encodeAuthReq( &sAuthReq, &pReq );

    ret = JS_HTTP_requestPost( strURL.toStdString().c_str(), pReq, pContentType, &nStatusCode, &pRsp );

    JS_CC_decodeAuthRsp( pRsp, &sAuthRsp );

    JS_CC_resetAuthReq( &sAuthReq );
    if( pReq ) JS_free( pReq );
    if( pRsp ) JS_free( pRsp );

    if( nStatusCode == JS_HTTP_STATUS_OK )
    {
        manApplet->accountInfo()->setToken( sAuthRsp.pToken );
        manApplet->accountInfo()->setPerm( sAuthRsp.pPerm );
        manApplet->ccClient()->setBaseUrl( mServerURLCombo->currentText() );

        JS_CC_resetAuthRsp( &sAuthRsp );
        QDialog::accept();
    }
    else
    {
        JS_CC_resetAuthRsp( &sAuthRsp );
        manApplet->warningBox( tr("Fail to login"), this );
        QDialog::reject();
    }
}

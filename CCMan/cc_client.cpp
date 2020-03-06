#include "cc_client.h"
#include "js_http.h"
#include "js_cc.h"
#include "js_cc_data.h"

#include "mainwindow.h"
#include "man_applet.h"
#include "account_info.h"


CCClient::CCClient()
{

}

void CCClient::setBaseUrl(const QString strUrl)
{
    base_url_ = strUrl;
}

int CCClient::getCount(int nType)
{
    int ret = 0;
    QString strURL;
    JNameValList    *pParamList = NULL;
    char    *pRsp = NULL;
    JCC_CountRsp sCountRsp;

    memset( &sCountRsp, 0x00, sizeof(sCountRsp));

    strURL = base_url_;
    strURL += JS_CC_PATH_COUNT;

    JS_UTIL_createNameValList2( "OP", "COUNT", &pParamList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_GET,
                pParamList,
                NULL,
                NULL,
                &pRsp );

    JS_CC_decodeCountRsp( pRsp, &sCountRsp );

    int nResCode = atoi( sCountRsp.pResCode );
    if( nResCode != 0 ) return -1;

    return atoi( sCountRsp.pCount );
}

int CCClient::getUserList( JCC_UserList **ppUserList )
{
    int ret = 0;
    QString strURL;
    JNameValList    *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char    *pRsp = NULL;

    strURL = base_url_;
    strURL += JS_CC_PATH_USER;

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &pRsp );

    JS_CC_decodeUserList( pRsp, ppUserList );

    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getUser( int nNum, JCC_User *pUser )
{
    int ret = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char *pRsp = NULL;


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_USER ).arg( nNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &pRsp );

    JS_CC_decodeUser( pRsp, pUser );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::delUser(int nNum)
{
    int ret = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_USER ).arg( nNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_DELETE,
                NULL,
                pHeaderList,
                NULL,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

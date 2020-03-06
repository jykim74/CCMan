#include "cc_client.h"
#include "js_http.h"
#include "js_cc.h"
#include "js_cc_data.h"

#include "mainwindow.h"
#include "man_applet.h"
#include "account_info.h"
#include "man_tree_item.h"


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
    JNameValList    *pHeaderList = NULL;
    char    *pRsp = NULL;
    JCC_NameVal     sNameVal;

    memset( &sNameVal, 0x00, sizeof(sNameVal));

    QString strToken = manApplet->accountInfo()->token();
    QString strType;

    if( nType == ITEM_TYPE_USER )
        strType = "users";


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_COUNT ).arg( strType );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &pRsp );

    JS_CC_decodeNameVal( pRsp, &sNameVal );
    int nCount = atoi( sNameVal.pValue );

    JS_CC_resetNameVal( &sNameVal );

    return nCount;
}

int CCClient::getUserList( int nOffset, int nLimit, JCC_UserList **ppUserList )
{
    int ret = 0;
    QString strURL;
    JNameValList    *pParamList = NULL;
    JNameValList    *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char    *pRsp = NULL;

    strURL = base_url_;
    strURL += JS_CC_PATH_USER;

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    if( nOffset >= 0 && nLimit >= 0 )
    {
        QString strOffset = QString( "%1").arg(nOffset);
        QString strLimit = QString( "%1").arg( nLimit );

        JS_UTIL_createNameValList2( "offset", strOffset.toStdString().c_str(), &pParamList );
        JS_UTIL_appendNameValList2( pParamList, "limit", strLimit.toStdString().c_str() );
    }

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &pRsp );

    JS_CC_decodeUserList( pRsp, ppUserList );

    if( pRsp ) JS_free( pRsp );

    if( pParamList ) JS_UTIL_resetNameValList( &pParamList );
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

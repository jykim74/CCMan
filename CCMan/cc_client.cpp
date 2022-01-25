#include "cc_client.h"
#include "js_http.h"
#include "js_cc.h"
#include "js_cc_client.h"

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
    int status = 0;
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
    else if( nType == ITEM_TYPE_CERT )
        strType = "certs";
    else if( nType == ITEM_TYPE_CRL )
        strType = "crls";
    else if( nType == ITEM_TYPE_REVOKE )
        strType = "revokeds";
    else if( nType == ITEM_TYPE_KMS )
        strType = "kms";
    else if( nType == ITEM_TYPE_TSP )
        strType = "tsp";
    else if( nType == ITEM_TYPE_AUDIT )
        strType = "audit";


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_COUNT ).arg( strType );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeNameVal( pRsp, &sNameVal );
    int nCount = atoi( sNameVal.pValue );

    JS_CC_resetNameVal( &sNameVal );

    return nCount;
}

int CCClient::searchCount(int nType, const QString strTarget, const QString strWord)
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList    *pParamList = NULL;
    JNameValList    *pHeaderList = NULL;
    char    *pRsp = NULL;
    JCC_NameVal     sNameVal;

    memset( &sNameVal, 0x00, sizeof(sNameVal));

    QString strToken = manApplet->accountInfo()->token();
    QString strType;

    if( nType == ITEM_TYPE_USER )
        strType = "user";
    else if( nType == ITEM_TYPE_CERT )
        strType = "cert";
    else if( nType == ITEM_TYPE_CRL )
        strType = "crl";
    else if( nType == ITEM_TYPE_REVOKE )
        strType = "revoked";
    else if( nType == ITEM_TYPE_KMS )
        strType = "kms";
    else if( nType == ITEM_TYPE_TSP )
        strType = "tsp";
    else if( nType == ITEM_TYPE_AUDIT )
        strType = "audit";


    strURL = base_url_;
    strURL += QString( "%1/%2?target=%3&word=%4" )
            .arg( JS_CC_PATH_COUNT )
            .arg( strType )
            .arg( strTarget )
            .arg( strWord );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeNameVal( pRsp, &sNameVal );
    int nCount = atoi( sNameVal.pValue );

    JS_CC_resetNameVal( &sNameVal );

    return nCount;
}

int CCClient::getNum(int nType)
{
    int ret = 0;
    int status = 0;
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
    else if( nType == ITEM_TYPE_CERT )
        strType = "certs";
    else if( nType == ITEM_TYPE_CRL )
        strType = "crls";
    else if( nType == ITEM_TYPE_REVOKE )
        strType = "revokeds";
    else if( nType == ITEM_TYPE_CERT_PROFILE )
        strType = "cert_policies";
    else if( nType == ITEM_TYPE_CRL_PROFILE )
        strType = "crl_policies";
    else
        strType = "none";


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_NUM ).arg( strType );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeNameVal( pRsp, &sNameVal );
    int nNum = atoi( sNameVal.pValue );

    JS_CC_resetNameVal( &sNameVal );

    return nNum;
}

QString CCClient::getName( int nNum, const char *pTable )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList    *pParamList = NULL;
    JNameValList    *pHeaderList = NULL;
    char    *pRsp = NULL;
    JCC_NameVal     sNameVal;
    QString strName;

    memset( &sNameVal, 0x00, sizeof(sNameVal));

    QString strToken = manApplet->accountInfo()->token();


    strURL = base_url_;
    strURL += QString( "%1/%2/%3" )
            .arg( JS_CC_PATH_NAME )
            .arg( pTable )
            .arg( nNum );


    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeNameVal( pRsp, &sNameVal );
    if( sNameVal.pValue ) strName = sNameVal.pValue;

    JS_CC_resetNameVal( &sNameVal );

    return strName;
}

QString CCClient::getDN( int nCertNum )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList    *pParamList = NULL;
    JNameValList    *pHeaderList = NULL;
    char    *pRsp = NULL;
    JCC_NameVal     sNameVal;
    QString strName;

    memset( &sNameVal, 0x00, sizeof(sNameVal));

    QString strToken = manApplet->accountInfo()->token();


    strURL = base_url_;
    strURL += QString( "%1/%2" )
            .arg( JS_CC_PATH_DN )
            .arg( nCertNum );


    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeNameVal( pRsp, &sNameVal );
    if( sNameVal.pValue ) strName = sNameVal.pValue;

    JS_CC_resetNameVal( &sNameVal );

    return strName;
}

int CCClient::getUserList( int nOffset, int nLimit, JCC_UserList **ppUserList )
{
    int ret = 0;
    int status = 0;
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
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeUserList( pRsp, ppUserList );

    if( pRsp ) JS_free( pRsp );

    if( pParamList ) JS_UTIL_resetNameValList( &pParamList );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::searchUserList( const QString strTarget, const QString strWord, int nOffset, int nLimit, JCC_UserList **ppUserList )
{
    int ret = 0;
    int status = 0;
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
        JS_UTIL_appendNameValList2( pParamList, "target", strTarget.toStdString().c_str() );
        JS_UTIL_appendNameValList2( pParamList, "word", strWord.toStdString().c_str() );
    }

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &status,
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
    int status = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char *pRsp = NULL;


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_USER ).arg( nNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeUser( pRsp, pUser );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::delUser(int nNum)
{
    int ret = 0;
    int status = 0;
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
                NULL,
                NULL,
                JS_HTTP_METHOD_DELETE,
                NULL,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::modUser( int nNum, JCC_User *pUser )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;
    char *pReq = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += QString( "%1/%2").arg(JS_CC_PATH_USER).arg(nNum);

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    JS_CC_encodeUser( pUser, &pReq );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_PUT,
                NULL,
                pHeaderList,
                pReq,
                &status,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pReq ) JS_free( pReq );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getAdmin( int nSeq, JCC_Admin *pAdmin )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char *pRsp = NULL;


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_ADMIN ).arg( nSeq );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeAdmin( pRsp, pAdmin );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::addAdmin( JCC_Admin *pAdmin )
{
    int ret = 0;
    int status = 0;

    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;
    char *pReq = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += JS_CC_PATH_ADMIN;

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    JS_CC_encodeAdmin( pAdmin, &pReq );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_POST,
                NULL,
                pHeaderList,
                pReq,
                &status,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pReq ) JS_free( pReq );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::modAdmin( int nSeq, JCC_Admin *pAdmin )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;
    char *pReq = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += QString( "%1/%2").arg(JS_CC_PATH_ADMIN).arg(nSeq);

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    JS_CC_encodeAdmin( pAdmin, &pReq );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_PUT,
                NULL,
                pHeaderList,
                pReq,
                &status,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pReq ) JS_free( pReq );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::delAdmin(int nSeq)
{
    int ret = 0;
    int status = 0;

    QString strURL;
    JNameValList *pHeaderList = NULL;

    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_ADMIN ).arg( nSeq );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );


    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_DELETE,
                NULL,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getAdminList( JCC_AdminList **ppAdminList )
{
    int ret = 0;
    int status = 0;
    QString strURL;

    JNameValList    *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char    *pRsp = NULL;

    strURL = base_url_;
    strURL += JS_CC_PATH_ADMIN;

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );


    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeAdminList( pRsp, ppAdminList );

    if( pRsp ) JS_free( pRsp );

    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );
    return 0;
}

int CCClient::getCertProfileList( JCC_CertProfileList **ppCertProfileList )
{
    int ret = 0;
    int status = 0;
    QString strURL;

    JNameValList    *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char    *pRsp = NULL;

    strURL = base_url_;
    strURL += JS_CC_PATH_CERT_PROFILE;

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );


    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeCertProfileList( pRsp, ppCertProfileList );

    if( pRsp ) JS_free( pRsp );

    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );
    return 0;
}

int CCClient::getCertProfile(int nNum, JCC_CertProfile *pCertProfile)
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char *pRsp = NULL;


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_CERT_PROFILE ).arg( nNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeCertProfile( pRsp, pCertProfile );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getCertProfileExtList( int nProfileNum, JCC_ProfileExtList **ppProfileExtList )
{
    int ret = 0;
    int status = 0;

    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char *pRsp = NULL;


    strURL = base_url_;
    strURL += QString( "%1/%2/extensions" ).arg( JS_CC_PATH_CERT_PROFILE ).arg( nProfileNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeProfileExtList( pRsp, ppProfileExtList );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}


int CCClient::getCRLProfileList( JCC_CRLProfileList **ppCRLProfileList )
{
    int ret = 0;
    int status = 0;
    QString strURL;

    JNameValList    *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char    *pRsp = NULL;

    strURL = base_url_;
    strURL += JS_CC_PATH_CRL_PROFILE;

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );


    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeCRLProfileList( pRsp, ppCRLProfileList );

    if( pRsp ) JS_free( pRsp );

    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );
    return 0;
}

int CCClient::getCRLProfile(int nNum, JCC_CRLProfile *pCRLProfile)
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char *pRsp = NULL;


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_CRL_PROFILE ).arg( nNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeCRLProfile( pRsp, pCRLProfile );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getCRLProfileExtList( int nProfileNum, JCC_ProfileExtList **ppProfileExtList )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char *pRsp = NULL;


    strURL = base_url_;
    strURL += QString( "%1/%2/extensions" ).arg( JS_CC_PATH_CRL_PROFILE ).arg( nProfileNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeProfileExtList( pRsp, ppProfileExtList );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getSigner( int nNum, JCC_Signer *pSigner )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char *pRsp = NULL;


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_SIGNER ).arg( nNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeSigner( pRsp, pSigner );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getSignerList( int nType, JCC_SignerList **ppSignerList )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList    *pParamList = NULL;
    JNameValList    *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char    *pRsp = NULL;

    strURL = base_url_;
    strURL += JS_CC_PATH_SIGNER;

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    if( nType >= 0 )
    {
        QString strType = QString( "%1").arg(nType);

        JS_UTIL_createNameValList2( "type", strType.toStdString().c_str(), &pParamList );
    }

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeSignerList( pRsp, ppSignerList );

    if( pRsp ) JS_free( pRsp );

    if( pParamList ) JS_UTIL_resetNameValList( &pParamList );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getCert( int nNum, JCC_Cert *pCert )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char *pRsp = NULL;


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_CERT ).arg( nNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeCert( pRsp, pCert );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getCertList( int nOffset, int nLimit, JCC_CertList **ppCertList )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList    *pParamList = NULL;
    JNameValList    *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char    *pRsp = NULL;

    strURL = base_url_;
    strURL += JS_CC_PATH_CERT;

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
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    fprintf( stderr, "Rsp : %s\n", pRsp );

    JS_CC_decodeCertList( pRsp, ppCertList );

    if( pRsp ) JS_free( pRsp );

    if( pParamList ) JS_UTIL_resetNameValList( &pParamList );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::searchCertList( const QString strTarget, const QString strWord, int nOffset, int nLimit, JCC_CertList **ppCertList )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList    *pParamList = NULL;
    JNameValList    *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char    *pRsp = NULL;

    strURL = base_url_;
    strURL += JS_CC_PATH_CERT;

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    if( nOffset >= 0 && nLimit >= 0 )
    {
        QString strOffset = QString( "%1").arg(nOffset);
        QString strLimit = QString( "%1").arg( nLimit );

        JS_UTIL_createNameValList2( "offset", strOffset.toStdString().c_str(), &pParamList );
        JS_UTIL_appendNameValList2( pParamList, "limit", strLimit.toStdString().c_str() );
        JS_UTIL_appendNameValList2( pParamList, "target", strTarget.toStdString().c_str() );
        JS_UTIL_appendNameValList2( pParamList, "word", strWord.toStdString().c_str() );
    }

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    fprintf( stderr, "Rsp : %s\n", pRsp );

    JS_CC_decodeCertList( pRsp, ppCertList );

    if( pRsp ) JS_free( pRsp );

    if( pParamList ) JS_UTIL_resetNameValList( &pParamList );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getCRL( int nNum, JCC_CRL *pCRL )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char *pRsp = NULL;


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_CRL ).arg( nNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeCRL( pRsp, pCRL );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );
    return 0;
}

int CCClient::getCRLList( int nOffset, int nLimit, JCC_CRLList **ppCRLList )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList    *pParamList = NULL;
    JNameValList    *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char    *pRsp = NULL;

    strURL = base_url_;
    strURL += JS_CC_PATH_CRL;

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
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeCRLList( pRsp, ppCRLList );

    if( pRsp ) JS_free( pRsp );

    if( pParamList ) JS_UTIL_resetNameValList( &pParamList );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::searchCRLList( const QString strTarget, const QString strWord, int nOffset, int nLimit, JCC_CRLList **ppCRLList )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList    *pParamList = NULL;
    JNameValList    *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char    *pRsp = NULL;

    strURL = base_url_;
    strURL += JS_CC_PATH_CRL;

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    if( nOffset >= 0 && nLimit >= 0 )
    {
        QString strOffset = QString( "%1").arg(nOffset);
        QString strLimit = QString( "%1").arg( nLimit );

        JS_UTIL_createNameValList2( "offset", strOffset.toStdString().c_str(), &pParamList );
        JS_UTIL_appendNameValList2( pParamList, "limit", strLimit.toStdString().c_str() );
        JS_UTIL_appendNameValList2( pParamList, "target", strTarget.toStdString().c_str() );
        JS_UTIL_appendNameValList2( pParamList, "word", strWord.toStdString().c_str() );
    }

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeCRLList( pRsp, ppCRLList );

    if( pRsp ) JS_free( pRsp );

    if( pParamList ) JS_UTIL_resetNameValList( &pParamList );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getRevoked( int nSeq, JCC_Revoked *pRevoked )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char *pRsp = NULL;


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_REVOKED ).arg( nSeq );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeRevoked( pRsp, pRevoked );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getRevokedList( int nOffset, int nLimit, JCC_RevokedList **ppRevokedList )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList    *pParamList = NULL;
    JNameValList    *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char    *pRsp = NULL;

    strURL = base_url_;
    strURL += JS_CC_PATH_REVOKED;

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
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeRevokedList( pRsp, ppRevokedList );

    if( pRsp ) JS_free( pRsp );

    if( pParamList ) JS_UTIL_resetNameValList( &pParamList );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::searchRevokedList( const QString strTarget, const QString strWord, int nOffset, int nLimit, JCC_RevokedList **ppRevokedList )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList    *pParamList = NULL;
    JNameValList    *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char    *pRsp = NULL;

    strURL = base_url_;
    strURL += JS_CC_PATH_REVOKED;

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    if( nOffset >= 0 && nLimit >= 0 )
    {
        QString strOffset = QString( "%1").arg(nOffset);
        QString strLimit = QString( "%1").arg( nLimit );

        JS_UTIL_createNameValList2( "offset", strOffset.toStdString().c_str(), &pParamList );
        JS_UTIL_appendNameValList2( pParamList, "limit", strLimit.toStdString().c_str() );
        JS_UTIL_appendNameValList2( pParamList, "target", strTarget.toStdString().c_str() );
        JS_UTIL_appendNameValList2( pParamList, "word", strWord.toStdString().c_str() );
    }

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeRevokedList( pRsp, ppRevokedList );

    if( pRsp ) JS_free( pRsp );

    if( pParamList ) JS_UTIL_resetNameValList( &pParamList );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getKMS( int nNum, JCC_KMS *pKMS )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char *pRsp = NULL;


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_KMS ).arg( nNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeKMS( pRsp, pKMS );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getKMSList( int nOffset, int nLimit, JCC_KMSList **ppKMSList )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList    *pParamList = NULL;
    JNameValList    *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char    *pRsp = NULL;

    strURL = base_url_;
    strURL += JS_CC_PATH_KMS;

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
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeKMSList( pRsp, ppKMSList );

    if( pRsp ) JS_free( pRsp );

    if( pParamList ) JS_UTIL_resetNameValList( &pParamList );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::searchKMSList( const QString strTarget, const QString strWord, int nOffset, int nLimit, JCC_KMSList **ppKMSList )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList    *pParamList = NULL;
    JNameValList    *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char    *pRsp = NULL;

    strURL = base_url_;
    strURL += JS_CC_PATH_KMS;

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    if( nOffset >= 0 && nLimit >= 0 )
    {
        QString strOffset = QString( "%1").arg(nOffset);
        QString strLimit = QString( "%1").arg( nLimit );

        JS_UTIL_createNameValList2( "offset", strOffset.toStdString().c_str(), &pParamList );
        JS_UTIL_appendNameValList2( pParamList, "limit", strLimit.toStdString().c_str() );
        JS_UTIL_appendNameValList2( pParamList, "target", strTarget.toStdString().c_str() );
        JS_UTIL_appendNameValList2( pParamList, "word", strWord.toStdString().c_str() );
    }

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeKMSList( pRsp, ppKMSList );

    if( pRsp ) JS_free( pRsp );

    if( pParamList ) JS_UTIL_resetNameValList( &pParamList );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getTSP( int nNum, JCC_TSP *pTSP )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char *pRsp = NULL;


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_TSP ).arg( nNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeTSP( pRsp, pTSP );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getTSPList( int nOffset, int nLimit, JCC_TSPList **ppTSPList )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList    *pParamList = NULL;
    JNameValList    *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char    *pRsp = NULL;

    strURL = base_url_;
    strURL += JS_CC_PATH_TSP;

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
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeTSPList( pRsp, ppTSPList );

    if( pRsp ) JS_free( pRsp );

    if( pParamList ) JS_UTIL_resetNameValList( &pParamList );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::searchTSPList( const QString strTarget, const QString strWord, int nOffset, int nLimit, JCC_TSPList **ppTSPList )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList    *pParamList = NULL;
    JNameValList    *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char    *pRsp = NULL;

    strURL = base_url_;
    strURL += JS_CC_PATH_TSP;

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    if( nOffset >= 0 && nLimit >= 0 )
    {
        QString strOffset = QString( "%1").arg(nOffset);
        QString strLimit = QString( "%1").arg( nLimit );

        JS_UTIL_createNameValList2( "offset", strOffset.toStdString().c_str(), &pParamList );
        JS_UTIL_appendNameValList2( pParamList, "limit", strLimit.toStdString().c_str() );
        JS_UTIL_appendNameValList2( pParamList, "target", strTarget.toStdString().c_str() );
        JS_UTIL_appendNameValList2( pParamList, "word", strWord.toStdString().c_str() );
    }

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeTSPList( pRsp, ppTSPList );

    if( pRsp ) JS_free( pRsp );

    if( pParamList ) JS_UTIL_resetNameValList( &pParamList );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getAudit( int nNum, JCC_Audit *pAudit )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char *pRsp = NULL;


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_AUDIT ).arg( nNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeAudit( pRsp, pAudit );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getAuditList( int nOffset, int nLimit, JCC_AuditList **ppAuditList )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList    *pParamList = NULL;
    JNameValList    *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char    *pRsp = NULL;

    strURL = base_url_;
    strURL += JS_CC_PATH_AUDIT;

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
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeAuditList( pRsp, ppAuditList );

    if( pRsp ) JS_free( pRsp );

    if( pParamList ) JS_UTIL_resetNameValList( &pParamList );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::searchAuditList( const QString strTarget, const QString strWord, int nOffset, int nLimit, JCC_AuditList **ppAuditList )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList    *pParamList = NULL;
    JNameValList    *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char    *pRsp = NULL;

    strURL = base_url_;
    strURL += JS_CC_PATH_AUDIT;

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    if( nOffset >= 0 && nLimit >= 0 )
    {
        QString strOffset = QString( "%1").arg(nOffset);
        QString strLimit = QString( "%1").arg( nLimit );

        JS_UTIL_createNameValList2( "offset", strOffset.toStdString().c_str(), &pParamList );
        JS_UTIL_appendNameValList2( pParamList, "limit", strLimit.toStdString().c_str() );
        JS_UTIL_appendNameValList2( pParamList, "target", strTarget.toStdString().c_str() );
        JS_UTIL_appendNameValList2( pParamList, "word", strWord.toStdString().c_str() );
    }

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeAuditList( pRsp, ppAuditList );

    if( pRsp ) JS_free( pRsp );

    if( pParamList ) JS_UTIL_resetNameValList( &pParamList );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getStatisticsCount( int nStart, int nEnd, const char *pTable )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList    *pParamList = NULL;
    JNameValList    *pHeaderList = NULL;
    char    *pRsp = NULL;
    JCC_NameVal     sNameVal;

    memset( &sNameVal, 0x00, sizeof(sNameVal));

    QString strToken = manApplet->accountInfo()->token();


    if( nStart >= 0 || nEnd >= 0 )
    {
        QString strStart = QString( "%1").arg(nStart);
        QString strEnd = QString( "%1").arg( nEnd );

        JS_UTIL_createNameValList2( "start", strStart.toStdString().c_str(), &pParamList );
        JS_UTIL_appendNameValList2( pParamList, "end", strEnd.toStdString().c_str() );
    }


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_STATISTICS ).arg( pTable );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeNameVal( pRsp, &sNameVal );
    int nCount = atoi( sNameVal.pValue );

    JS_CC_resetNameVal( &sNameVal );

    return nCount;
}

int CCClient::addCRLProfile( JCC_CRLProfile *pCRLProfile )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;
    char *pReq = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += JS_CC_PATH_CRL_PROFILE;

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    JS_CC_encodeCRLProfile( pCRLProfile, &pReq );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_POST,
                NULL,
                pHeaderList,
                pReq,
                &status,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pReq ) JS_free( pReq );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );
    return 0;
}

int CCClient::modCRLProfile( int nNum, JCC_CRLProfile *pCRLProfile )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;
    char *pReq = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += QString( "%1/%2").arg(JS_CC_PATH_CRL_PROFILE).arg(nNum);

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    JS_CC_encodeCRLProfile( pCRLProfile, &pReq );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_PUT,
                NULL,
                pHeaderList,
                pReq,
                &status,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pReq ) JS_free( pReq );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::delCRLProfileExts( int nProfileNum )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    JNameValList *pParamList = NULL;
    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_CRL_PROFILE ).arg( nProfileNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );
    JS_UTIL_createNameValList2( "mode", "extonly", &pParamList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_DELETE,
                pParamList,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );
    if( pParamList ) JS_UTIL_resetNameValList( &pParamList );

    return 0;
}

int CCClient::delCRLProfile( int nNum )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;

    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_CRL_PROFILE ).arg( nNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );


    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_DELETE,
                NULL,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::addCRLProfileExt( int nProfileNum, JCC_ProfileExt *pProfileExt )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;
    char *pReq = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg(JS_CC_PATH_CRL_PROFILE).arg( nProfileNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    JS_CC_encodeProfileExt( pProfileExt, &pReq );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_POST,
                NULL,
                pHeaderList,
                pReq,
                &status,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pReq ) JS_free( pReq );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::addCertProfile( JCC_CertProfile *pCertProfile )
{
    int ret = 0;
    int status = 0;

    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;
    char *pReq = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += JS_CC_PATH_CERT_PROFILE;

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    JS_CC_encodeCertProfile( pCertProfile, &pReq );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_POST,
                NULL,
                pHeaderList,
                pReq,
                &status,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pReq ) JS_free( pReq );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );
    return 0;
}

int CCClient::modCertProfile( int nNum, JCC_CertProfile *pCertProfile )
{
    int ret = 0;
    int status = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;
    char *pReq = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg(JS_CC_PATH_CERT_PROFILE).arg( nNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    JS_CC_encodeCertProfile( pCertProfile, &pReq );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_PUT,
                NULL,
                pHeaderList,
                pReq,
                &status,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pReq ) JS_free( pReq );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::delCertProfile( int nNum )
{
    int ret = 0;
    int status = 0;

    QString strURL;
    JNameValList *pHeaderList = NULL;

    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_CERT_PROFILE ).arg( nNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );


    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_DELETE,
                NULL,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::delCertProfileExts( int nProfileNum )
{
    int ret = 0;
    int status = 0;

    QString strURL;
    JNameValList *pHeaderList = NULL;
    JNameValList *pParamList = NULL;
    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_CERT_PROFILE ).arg( nProfileNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );
    JS_UTIL_createNameValList2( "mode", "extonly", &pParamList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_DELETE,
                pParamList,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );
    if( pParamList ) JS_UTIL_resetNameValList( &pParamList );

    return 0;
}

int CCClient::addCertProfileExt( int nProfileNum, JCC_ProfileExt *pProfileExt )
{
    int ret = 0;
    int status = 0;

    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;
    char *pReq = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg(JS_CC_PATH_CERT_PROFILE).arg( nProfileNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    JS_CC_encodeProfileExt( pProfileExt, &pReq );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_POST,
                NULL,
                pHeaderList,
                pReq,
                &status,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pReq ) JS_free( pReq );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::addSigner(JCC_Signer *pSigner)
{
    int ret = 0;
    int status = 0;

    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;
    char *pReq = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += JS_CC_PATH_SIGNER;

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    JS_CC_encodeSigner( pSigner, &pReq );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_POST,
                NULL,
                pHeaderList,
                pReq,
                &status,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pReq ) JS_free( pReq );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::delSigner(int nNum)
{
    int ret = 0;
    int status = 0;

    QString strURL;
    JNameValList *pHeaderList = NULL;

    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_SIGNER ).arg( nNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );


    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_DELETE,
                NULL,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::addRevoked(JCC_Revoked *pRevoked)
{
    int ret = 0;
    int status = 0;

    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;
    char *pReq = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += JS_CC_PATH_REVOKED;

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    JS_CC_encodeRevoked( pRevoked, &pReq );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_POST,
                NULL,
                pHeaderList,
                pReq,
                &status,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pReq ) JS_free( pReq );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::delRevoked(int nSeq)
{
    int ret = 0;
    int status = 0;

    QString strURL;
    JNameValList *pHeaderList = NULL;

    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_REVOKED ).arg( nSeq );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );


    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                NULL,
                NULL,
                JS_HTTP_METHOD_DELETE,
                NULL,
                pHeaderList,
                NULL,
                &status,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::issueCert( const JCC_IssueCertReq *pReq, JCC_IssueCertRsp *pRsp )
{
    QString strToken = manApplet->accountInfo()->token();

    return JS_CC_clientIssueCert(
                base_url_.toStdString().c_str(),
                strToken.toStdString().c_str(),
                pReq,
                pRsp );
}

int CCClient::issueCRL( const JCC_IssueCRLReq *pReq, JCC_IssueCRLRsp *pRsp )
{
    QString strToken = manApplet->accountInfo()->token();

    return JS_CC_clientIssueCRL(
                base_url_.toStdString().c_str(),
                strToken.toStdString().c_str(),
                pReq,
                pRsp );
}

int CCClient::getCA(JCC_NameVal *pRsp)
{
    QString strToken = manApplet->accountInfo()->token();

    return JS_CC_clientGetCA(
                base_url_.toStdString().c_str(),
                strToken.toStdString().c_str(),
                pRsp );
}

int CCClient::publishClient( int nItemType, int nNum, JCC_CodeMsg *pCodeMsg )
{
    QString strType;
    QString strToken = manApplet->accountInfo()->token();

    if( nItemType == ITEM_TYPE_CA )
        strType = "cacert";
    else if( nItemType == ITEM_TYPE_CRL )
        strType = "crl";
    else if( nItemType == ITEM_TYPE_CERT )
        strType = "cert";

    return JS_CC_clientPublishLDAP(
                base_url_.toStdString().c_str(),
                strToken.toStdString().c_str(),
                strType.toStdString().c_str(),
                nNum,
                pCodeMsg );
}

int CCClient::getCRLDPList( JCC_NameValList **ppCRLDPList )
{
    QString strToken = manApplet->accountInfo()->token();

    return JS_CC_clientGetCRLDPList(
                base_url_.toStdString().c_str(),
                strToken.toStdString().c_str(),
                ppCRLDPList );
}

int CCClient::getCertStatus( const char *pSerial, JCC_CertStatus *pCertStatus )
{
    QString strToken = manApplet->accountInfo()->token();

    return JS_CC_clientCertStatus(
                base_url_.toStdString().c_str(),
                strToken.toStdString().c_str(),
                pSerial,
                pCertStatus );
}

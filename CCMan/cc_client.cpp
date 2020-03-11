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
    else if( nType == ITEM_TYPE_CERT )
        strType = "certs";
    else if( nType == ITEM_TYPE_CRL )
        strType = "crls";
    else if( nType == ITEM_TYPE_REVOKE )
        strType = "revokeds";


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

int CCClient::getNum(int nType)
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
    else if( nType == ITEM_TYPE_CERT )
        strType = "certs";
    else if( nType == ITEM_TYPE_CRL )
        strType = "crls";
    else if( nType == ITEM_TYPE_REVOKE )
        strType = "revokeds";
    else if( nType == ITEM_TYPE_CERT_POLICY )
        strType = "cert_policies";
    else if( nType == ITEM_TYPE_CRL_POLICY )
        strType = "crl_policies";
    else
        strType = "none";


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_NUM ).arg( strType );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &pRsp );

    JS_CC_decodeNameVal( pRsp, &sNameVal );
    int nNum = atoi( sNameVal.pValue );

    JS_CC_resetNameVal( &sNameVal );

    return nNum;
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

int CCClient::getCertPolicyList( JCC_CertPolicyList **ppCertPolicyList )
{
    int ret = 0;
    QString strURL;

    JNameValList    *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char    *pRsp = NULL;

    strURL = base_url_;
    strURL += JS_CC_PATH_CERT_POLICY;

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );


    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &pRsp );

    JS_CC_decodeCertPolicyList( pRsp, ppCertPolicyList );

    if( pRsp ) JS_free( pRsp );

    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );
    return 0;
}

int CCClient::getCertPolicy(int nNum, JCC_CertPolicy *pCertPolicy)
{
    int ret = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char *pRsp = NULL;


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_CERT_POLICY ).arg( nNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &pRsp );

    JS_CC_decodeCertPolicy( pRsp, pCertPolicy );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getCertPolicyExtList( int nPolicyNum, JCC_PolicyExtList **ppPolicyExtList )
{
    int ret = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char *pRsp = NULL;


    strURL = base_url_;
    strURL += QString( "%1/%2/extensions" ).arg( JS_CC_PATH_CERT_POLICY ).arg( nPolicyNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &pRsp );

    JS_CC_decodePolicyExtList( pRsp, ppPolicyExtList );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}


int CCClient::getCRLPolicyList( JCC_CRLPolicyList **ppCRLPolicyList )
{
    int ret = 0;
    QString strURL;

    JNameValList    *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char    *pRsp = NULL;

    strURL = base_url_;
    strURL += JS_CC_PATH_CRL_POLICY;

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );


    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &pRsp );

    JS_CC_decodeCRLPolicyList( pRsp, ppCRLPolicyList );

    if( pRsp ) JS_free( pRsp );

    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );
    return 0;
}

int CCClient::getCRLPolicy(int nNum, JCC_CRLPolicy *pCRLPolicy)
{
    int ret = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char *pRsp = NULL;


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_CRL_POLICY ).arg( nNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &pRsp );

    JS_CC_decodeCRLPolicy( pRsp, pCRLPolicy );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getCRLPolicyExtList( int nPolicyNum, JCC_PolicyExtList **ppPolicyExtList )
{
    int ret = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char *pRsp = NULL;


    strURL = base_url_;
    strURL += QString( "%1/%2/extensions" ).arg( JS_CC_PATH_CRL_POLICY ).arg( nPolicyNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &pRsp );

    JS_CC_decodePolicyExtList( pRsp, ppPolicyExtList );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getSigner( int nNum, JCC_Signer *pSigner )
{
    int ret = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char *pRsp = NULL;


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_SIGNER ).arg( nNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &pRsp );

    JS_CC_decodeSigner( pRsp, pSigner );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getSignerList( int nType, JCC_SignerList **ppSignerList )
{
    int ret = 0;
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
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
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
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char *pRsp = NULL;


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_CERT ).arg( nNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &pRsp );

    JS_CC_decodeCert( pRsp, pCert );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getCertList( int nOffset, int nLimit, JCC_CertList **ppCertList )
{
    int ret = 0;
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
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
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
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char *pRsp = NULL;


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_CRL ).arg( nNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &pRsp );

    JS_CC_decodeCRL( pRsp, pCRL );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );
    return 0;
}

int CCClient::getCRLList( int nOffset, int nLimit, JCC_CRLList **ppCRLList )
{
    int ret = 0;
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
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
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
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();

    char *pRsp = NULL;


    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_REVOKED ).arg( nSeq );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_GET,
                NULL,
                pHeaderList,
                NULL,
                &pRsp );

    JS_CC_decodeRevoked( pRsp, pRevoked );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::getRevokedList( int nOffset, int nLimit, JCC_RevokedList **ppRevokedList )
{
    int ret = 0;
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
                JS_HTTP_METHOD_GET,
                pParamList,
                pHeaderList,
                NULL,
                &pRsp );

    JS_CC_decodeRevokedList( pRsp, ppRevokedList );

    if( pRsp ) JS_free( pRsp );

    if( pParamList ) JS_UTIL_resetNameValList( &pParamList );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::addCRLPolicy( JCC_CRLPolicy *pCRLPolicy )
{
    int ret = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;
    char *pReq = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += JS_CC_PATH_CRL_POLICY;

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    JS_CC_encodeCRLPolicy( pCRLPolicy, &pReq );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_POST,
                NULL,
                pHeaderList,
                pReq,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pReq ) JS_free( pReq );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );
    return 0;
}

int CCClient::modCRLPolicy( int nNum, JCC_CRLPolicy *pCRLPolicy )
{
    int ret = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;
    char *pReq = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += QString( "%1/%2").arg(JS_CC_PATH_CRL_POLICY).arg(nNum);

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    JS_CC_encodeCRLPolicy( pCRLPolicy, &pReq );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_PUT,
                NULL,
                pHeaderList,
                pReq,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pReq ) JS_free( pReq );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::delCRLPolicyExts( int nPolicyNum )
{
    int ret = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    JNameValList *pParamList = NULL;
    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_CRL_POLICY ).arg( nPolicyNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );
    JS_UTIL_createNameValList2( "mode", "extonly", &pParamList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_DELETE,
                pParamList,
                pHeaderList,
                NULL,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );
    if( pParamList ) JS_UTIL_resetNameValList( &pParamList );

    return 0;
}

int CCClient::delCRLPolicy( int nNum )
{
    int ret = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;

    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_CRL_POLICY ).arg( nNum );

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

int CCClient::addCRLPolicyExt( int nPolicyNum, JCC_PolicyExt *pPolicyExt )
{
    int ret = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;
    char *pReq = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg(JS_CC_PATH_CRL_POLICY).arg( nPolicyNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    JS_CC_encodePolicyExt( pPolicyExt, &pReq );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_POST,
                NULL,
                pHeaderList,
                pReq,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pReq ) JS_free( pReq );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::addCertPolicy( JCC_CertPolicy *pCertPolicy )
{
    int ret = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;
    char *pReq = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += JS_CC_PATH_CERT_POLICY;

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    JS_CC_encodeCertPolicy( pCertPolicy, &pReq );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_POST,
                NULL,
                pHeaderList,
                pReq,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pReq ) JS_free( pReq );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );
    return 0;
}

int CCClient::modCertPolicy( int nNum, JCC_CertPolicy *pCertPolicy )
{
    int ret = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;
    char *pReq = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg(JS_CC_PATH_CERT_POLICY).arg( nNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    JS_CC_encodeCertPolicy( pCertPolicy, &pReq );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_PUT,
                NULL,
                pHeaderList,
                pReq,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pReq ) JS_free( pReq );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );

    return 0;
}

int CCClient::delCertPolicy( int nNum )
{
    int ret = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;

    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_CERT_POLICY ).arg( nNum );

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

int CCClient::delCertPolicyExts( int nPolicyNum )
{
    int ret = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    JNameValList *pParamList = NULL;
    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg( JS_CC_PATH_CERT_POLICY ).arg( nPolicyNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );
    JS_UTIL_createNameValList2( "mode", "extonly", &pParamList );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_DELETE,
                pParamList,
                pHeaderList,
                NULL,
                &pRsp );

    JS_CC_decodeCodeMsg( pRsp, &sCodeMsg );

    JS_CC_resetCodeMsg( &sCodeMsg );
    if( pRsp ) JS_free( pRsp );
    if( pHeaderList ) JS_UTIL_resetNameValList( &pHeaderList );
    if( pParamList ) JS_UTIL_resetNameValList( &pParamList );

    return 0;
}

int CCClient::addCertPolicyExt( int nPolicyNum, JCC_PolicyExt *pPolicyExt )
{
    int ret = 0;
    QString strURL;
    JNameValList *pHeaderList = NULL;
    QString strToken = manApplet->accountInfo()->token();
    JCC_CodeMsg sCodeMsg;

    char *pRsp = NULL;
    char *pReq = NULL;

    memset( &sCodeMsg, 0x00, sizeof(sCodeMsg));

    strURL = base_url_;
    strURL += QString( "%1/%2" ).arg(JS_CC_PATH_CERT_POLICY).arg( nPolicyNum );

    JS_UTIL_createNameValList2( "Token", strToken.toStdString().c_str(), &pHeaderList );

    JS_CC_encodePolicyExt( pPolicyExt, &pReq );

    ret = JS_HTTP_requestResponse(
                strURL.toStdString().c_str(),
                JS_HTTP_METHOD_POST,
                NULL,
                pHeaderList,
                pReq,
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
                JS_HTTP_METHOD_POST,
                NULL,
                pHeaderList,
                pReq,
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

int CCClient::addRevoked(JCC_Revoked *pRevoked)
{
    int ret = 0;
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
                JS_HTTP_METHOD_POST,
                NULL,
                pHeaderList,
                pReq,
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

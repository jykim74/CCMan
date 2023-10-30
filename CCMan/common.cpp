#include <QDateTime>
#include <QRegularExpression>

#include "common.h"
#include "js_pki_tools.h"
#include "js_pki_ext.h"

QString getDateTime( time_t tTime )
{
    QDateTime dateTime;
    dateTime.setTime_t( tTime );

    return dateTime.toString( "yyyy-MM-dd HH:mm:ss");
}


QString getRecStatusName( int nStatus )
{
    if( nStatus == JS_REC_STATUS_NOT_USED )
        return "NotUsed";
    else if( nStatus == JS_REC_STATUS_USED )
        return "Used";

    return "Unknown";
}

QString getAdminTypeName( int nType )
{
    if( nType == JS_ADMIN_TYPE_INVALID )
        return "Invalid";
    else if( nType == JS_ADMIN_TYPE_MASTER )
        return "Master";
    else if( nType == JS_ADMIN_TYPE_ADMIN )
        return "Admin";
    else if( nType == JS_ADMIN_TYPE_AUDIT )
        return "Audit";

    return "Unknown";
}

QString getStatusName( int nStatus )
{
    if( nStatus == JS_STATUS_INVALID )
        return "Invalid";
    else if( nStatus == JS_STATUS_STOP )
        return "Stop";
    else if( nStatus == JS_STATUS_VALID )
        return "Valid";

    return "Unknown";
}

QString getUserStatusName( int nStatus )
{
    if( nStatus == JS_USER_STATUS_INVALID )
        return "Invalid";
    else if( nStatus == JS_USER_STATUS_REGISTER )
        return "Register";
    else if( nStatus == JS_USER_STATUS_ISSUED )
        return "Issued";
    else if( nStatus == JS_USER_STATUS_STOP )
        return "Stop";

    return "Unknown";
}

QString getSignerTypeName( int nType )
{
    if( nType == JS_SIGNER_TYPE_REG )
        return "RegSigner";
    else if( nType == JS_SIGNER_TYPE_OCSP )
        return "OCSPSigner";

    return "Unknown";
}

QString getCertStatusName( int nStatus )
{
    if( nStatus == JS_CERT_STATUS_INVALID )
        return "Invalid";
    else if( nStatus == JS_CERT_STATUS_GOOD )
        return "Good";
    else if( nStatus == JS_CERT_STATUS_REVOKE )
        return "Revoke";
    else if( nStatus == JS_CERT_STATUS_HOLD )
        return "Hold";

    return "Unknown";
}

QString getCertStatusSName( int nStatus )
{
    if( nStatus == JS_CERT_STATUS_INVALID )
        return "I";
    else if( nStatus == JS_CERT_STATUS_GOOD )
        return "G";
    else if( nStatus == JS_CERT_STATUS_REVOKE )
        return "R";
    else if( nStatus == JS_CERT_STATUS_HOLD )
        return "H";

    return "Unknown";
}

QString getRevokeReasonName( int nReason )
{
    return kRevokeReasonList.at( nReason );
}

QString getHexString( unsigned char *pData, int nDataLen )
{
    BIN binData = {0,0};
    char *pHex = NULL;
    JS_BIN_set( &binData, pData, nDataLen );
    JS_BIN_encodeHex( &binData, &pHex );

    QString strHex = pHex;

    JS_BIN_reset( &binData );
    if(pHex) JS_free( pHex );

    return strHex;
}

int getDataLen( int nType, const QString strData )
{
    int nLen = 0;
    if( strData.isEmpty() ) return 0;

    QString strMsg = strData;


    if( nType != DATA_STRING )
    {
        strMsg.remove( QRegularExpression("[\t\r\n\\s]") );
    }

    if( nType == DATA_HEX )
    {
        nLen = strMsg.length() / 2;

        if( strMsg.length() % 2 ) nLen++;

        return nLen;
    }
    else if( nType == DATA_BASE64 )
    {
        BIN bin = {0,0};
        JS_BIN_decodeBase64( strMsg.toStdString().c_str(), &bin );
        nLen = bin.nLen;
        JS_BIN_reset( &bin );
        return nLen;
    }

    return strData.length();
}

int getDataLen( const QString strType, const QString strData )
{
    int nType = DATA_STRING;

    QString strLower = strType.toLower();

    if( strLower == "hex" )
        nType = DATA_HEX;
    else if( strLower == "base64" )
        nType = DATA_BASE64;

    return getDataLen( nType, strData );
}


void getBINFromString( BIN *pBin, const QString& strType, const QString& strString )
{
    int nType = 0;

    if( strType.toUpper() == "HEX" )
        nType = DATA_HEX;
    else if( strType.toUpper() == "BASE64" )
        nType = DATA_BASE64;
    else if( strType.toUpper() == "URL" )
        nType = DATA_URL;
    else
        nType = DATA_STRING;

    getBINFromString( pBin, nType, strString );
}

void getBINFromString( BIN *pBin, int nType, const QString& strString )
{
    QString srcString = strString;

    if( pBin == NULL ) return;

    if( nType == DATA_HEX )
    {
        srcString.remove( QRegularExpression("[\t\r\n\\s]") );
        JS_BIN_decodeHex( srcString.toStdString().c_str(), pBin );
    }
    else if( nType == DATA_BASE64 )
    {
        srcString.remove( QRegularExpression("[\t\r\n\\s]") );
        JS_BIN_decodeBase64( srcString.toStdString().c_str(), pBin );
    }
    else if( nType == DATA_URL )
    {
        char *pStr = NULL;

        JS_UTIL_decodeURL( srcString.toStdString().c_str(), &pStr );

        if( pStr )
        {
            JS_BIN_set( pBin, (unsigned char *)pStr, strlen(pStr));
            JS_free( pStr );
        }
    }
    else
    {
        JS_BIN_set( pBin, (unsigned char *)srcString.toStdString().c_str(), srcString.length() );
    }
}

QString getStringFromBIN( const BIN *pBin, const QString& strType, bool bSeenOnly )
{
    int nType = 0;

    if( strType.toUpper() == "HEX" )
        nType = DATA_HEX;
    else if( strType.toUpper() == "BASE64" )
        nType = DATA_BASE64;
    else if( strType.toUpper() == "URL" )
        nType = DATA_URL;
    else
        nType = DATA_STRING;

    return getStringFromBIN( pBin, nType, bSeenOnly );
}

static char _getch( unsigned char c )
{
    if( isprint(c) )
        return c;
    else {
        return '.';
    }
}

QString getStringFromBIN( const BIN *pBin, int nType, bool bSeenOnly )
{
    QString strOut;
    char *pOut = NULL;

    if( pBin == NULL || pBin->nLen <= 0 ) return "";

    if( nType == DATA_HEX )
    {
        JS_BIN_encodeHex( pBin, &pOut );
        strOut = pOut;
    }
    else if( nType == DATA_BASE64 )
    {
        JS_BIN_encodeBase64( pBin, &pOut );
        strOut = pOut;
    }
    else if( nType == DATA_URL )
    {
        char *pStr = NULL;
        JS_BIN_string( pBin, &pStr );
        JS_UTIL_encodeURL( pStr, &pOut );
        strOut = pOut;
        if( pStr ) JS_free( pStr );
    }
    else
    {
        int i = 0;

        if( bSeenOnly )
        {
            if( pBin->nLen > 0 )
            {
                pOut = (char *)JS_malloc(pBin->nLen + 1);

                for( i=0; i < pBin->nLen; i++ )
                    pOut[i] = _getch( pBin->pVal[i] );

                pOut[i] = 0x00;
            }
        }
        else
        {
            JS_BIN_string( pBin, &pOut );
        }

        strOut = pOut;
    }

    if( pOut ) JS_free( pOut );
    return strOut;
}


static int _getKeyUsage( const BIN *pBinExt, bool bShow, QString& strVal )
{
    int     ret = 0;
    int     nKeyUsage = 0;

    ret = JS_PKI_getKeyUsageValue( pBinExt, &nKeyUsage );

    if( nKeyUsage & JS_PKI_KEYUSAGE_DIGITAL_SIGNATURE )
    {
        if( strVal.length() > 0 ) strVal += ",";
        strVal += "DigitalSignature";
    }

    if( nKeyUsage & JS_PKI_KEYUSAGE_NON_REPUDIATION )
    {
        if( strVal.length() > 0 ) strVal += ",";
        strVal += "NonRepudiation";
    }

    if( nKeyUsage & JS_PKI_KEYUSAGE_KEY_ENCIPHERMENT )
    {
        if( strVal.length() > 0 ) strVal += ",";
        strVal += "KeyEncipherment";
    }

    if( nKeyUsage & JS_PKI_KEYUSAGE_DATA_ENCIPHERMENT )
    {
        if( strVal.length() > 0 ) strVal += ",";
        strVal += "DataEncipherment";
    }

    if( nKeyUsage & JS_PKI_KEYUSAGE_KEY_AGREEMENT )
    {
        if( strVal.length() > 0 ) strVal += ",";
        strVal += "KeyAgreement";
    }

    if( nKeyUsage & JS_PKI_KEYUSAGE_CERT_SIGN )
    {
        if( strVal.length() > 0 ) strVal += ",";
        strVal += "keyCertSign";
    }

    if( nKeyUsage & JS_PKI_KEYUSAGE_CRL_SIGN )
    {
        if( strVal.length() > 0 ) strVal += ",";
        strVal += "cRLSign";
    }

    if( nKeyUsage & JS_PKI_KEYUSAGE_ENCIPHER_ONLY )
    {
        if( strVal.length() > 0 ) strVal += ",";
        strVal += "EncipherOnly";
    }

    if( nKeyUsage & JS_PKI_KEYUSAGE_DECIPHER_ONLY )
    {
        if( strVal.length() > 0 ) strVal += ",";
        strVal += "DecipherOnly";
    }

    return 0;
}

static int _getCRLNum( const BIN *pBinExt, bool bShow, QString& strVal )
{
    int ret = 0;
    char    *pCRLNum = NULL;

    ret = JS_PKI_getCRLNumberValue( pBinExt, &pCRLNum );

    if( pCRLNum ) {
        if(bShow)
            strVal = QString( "CRL Number=%1" ).arg( pCRLNum );
        else
            strVal = pCRLNum;

        JS_free( pCRLNum );
    }

    return 0;
}

static int _getCertPolicy( const BIN *pBinExt, bool bShow, QString& strVal )
{
    int ret = 0;
    int i = 0;
    JExtPolicyList *pPolicyList = NULL;
    JExtPolicyList *pCurList = NULL;

    ret = JS_PKI_getCertificatePoliciesValue( pBinExt, &pPolicyList );

    pCurList = pPolicyList;

    while( pCurList )
    {
        if( bShow )
        {
            strVal += QString( "[%1]Certificate Policy:\n" ).arg(i+1);
            strVal += QString( " Policy Identifier=%1\n" ).arg( pCurList->sPolicy.pOID );
            if( pCurList->sPolicy.pCPS )
            {
                strVal += QString( " [%1,1] CPS = %2\n" ).arg( i+1 ).arg( pCurList->sPolicy.pCPS );
            }

            if( pCurList->sPolicy.pUserNotice )
            {
                strVal += QString( " [%1,2] UserNotice = %2\n" ).arg( i+1 ).arg( pCurList->sPolicy.pUserNotice );
            }
        }
        else
        {
            if( strVal.length() > 0 ) strVal += "%%";

            strVal += QString("#OID$%1#CPS$%2#UserNotice$%3")
                .arg( pCurList->sPolicy.pOID )
                .arg( pCurList->sPolicy.pCPS )
                .arg( pCurList->sPolicy.pUserNotice );
        }

        pCurList = pCurList->pNext;
        i++;
    }

    if( pPolicyList ) JS_PKI_resetExtPolicyList( &pPolicyList );
    return 0;
}


static int _getSKI( const BIN *pBinExt, bool bShow, QString& strVal )
{
    int ret = 0;
    char        *pSKI = NULL;

    ret = JS_PKI_getSubjectKeyIdentifierValue( pBinExt, &pSKI );

    if( pSKI )
    {
        strVal = pSKI;
        JS_free( pSKI );
    }

    return 0;
}


static int _getAKI( const BIN *pBinExt, bool bShow, QString& strVal )
{
    int ret = 0;
    char    *pAKI = NULL;
    char    *pIssuer = NULL;
    char    *pSerial = NULL;

    ret = JS_PKI_getAuthorityKeyIdentifierValue( pBinExt, &pAKI, &pIssuer, &pSerial );

    if( bShow == true )
    {
        strVal = QString( "KeyID=%1\n").arg( pAKI );
        if( pIssuer ) strVal += QString( "CertificateIssuer=\n    %1\n").arg( pIssuer );
        if( pSerial ) strVal += QString( "CertificateSerialNumber=%1").arg( pSerial );
    }
    else
    {
        strVal = QString( "KEYID$%1#ISSUER$%2#SERIAL$%3").arg( pAKI ).arg( pIssuer ).arg( pSerial );
    }

    if( pAKI ) JS_free( pAKI );
    if( pIssuer ) JS_free( pIssuer );
    if( pSerial ) JS_free( pSerial );

    return 0;
}

static int _getEKU( const BIN *pBinExt, bool bShow, QString& strVal )
{
    int     ret = 0;
    JStrList   *pEKUList = NULL;
    JStrList   *pCurList = NULL;

    ret = JS_PKI_getExtendedKeyUsageValue( pBinExt, &pEKUList );

    pCurList = pEKUList;

    while( pCurList )
    {
        if( strVal.length() > 0 ) strVal += ",";

        strVal += QString( pCurList->pStr );

        pCurList = pCurList->pNext;
    }

    if( pEKUList ) JS_UTIL_resetStrList( &pEKUList );
    return 0;
}

static int _getCRLDP( const BIN *pBinExt, bool bShow, QString& strVal )
{
    int     ret = 0;
    int i = 1;
    JNameValList   *pCRLDPList = NULL;
    JNameValList    *pCurList = NULL;

    ret = JS_PKI_getCRLDPValue( pBinExt, &pCRLDPList );

    pCurList = pCRLDPList;

    while( pCurList )
    {
        if( bShow )
        {
            strVal += QString( "[%1] CRL Distribution Point\n" ).arg(i);
            strVal += QString( " %1=%2\n" ).arg( pCurList->sNameVal.pName ).arg( pCurList->sNameVal.pValue );
        }
        else
        {
            if( strVal.length() > 0 ) strVal += "#";

            strVal += QString( "%1$%2")
                .arg( pCurList->sNameVal.pName )
                .arg( pCurList->sNameVal.pValue );
        }

        pCurList = pCurList->pNext;
        i++;
    }

    if( pCRLDPList ) JS_UTIL_resetNameValList( &pCRLDPList );
    return 0;
}

static int _getBC( const BIN *pBinExt, bool bShow, QString& strVal )
{
    int ret = 0;
    int nType = -1;
    int nPathLen = -1;

    QString strType;
    QString strPathLen;

    ret = JS_PKI_getBCValue( pBinExt, &nType, &nPathLen );

    if( nType == JS_PKI_BC_TYPE_CA )
        strType = "CA";
    else if( nType == JS_PKI_BC_TYPE_USER )
        strType = "EE";


    if( nPathLen >= 0 )
        strPathLen = QString("$PathLen:%1").arg( nPathLen );

    if( bShow )
    {
        strVal = QString( "SubjectType=%1\n").arg(strType);
        if( nPathLen >= 0 )
            strVal += QString( "PathLengthConstraint=%1" ).arg(nPathLen);
        else
            strVal += QString( "PathLengthConstraint=None" );
    }
    else
    {
        strVal += strType;
        strVal += strPathLen;
    }

    return 0;
}


static int _getPC( const BIN *pBinExt, bool bShow, QString& strVal )
{
    int ret = 0;
    int nREP = -1;
    int nIPM = -1;

    ret = JS_PKI_getPolicyConstValue( pBinExt, &nREP, &nIPM );

    if( bShow )
    {
        if( nREP >= 0 ) strVal += QString("RequiredExplicitPolicySkipCerts=%1\n").arg( nREP );
        if( nIPM >= 0 ) strVal += QString("InhibitPolicyMappingSkipCerts=%1\n").arg( nIPM );
    }
    else
    {
        if( nREP >= 0 ) strVal += QString("#REP$%1").arg( nREP );
        if( nIPM >= 0 ) strVal += QString("#IPM$%1").arg( nIPM );
    }

    return 0;
}

static int _getAIA( const BIN *pBinExt, bool bShow, QString& strVal )
{
    int ret = 0;
    int i = 1;
    JExtAuthorityInfoAccessList    *pAIAList = NULL;
    JExtAuthorityInfoAccessList    *pCurList = NULL;

    ret = JS_PKI_getAuthorityInfoAccessValue( pBinExt, &pAIAList );

    pCurList = pAIAList;

    while( pCurList )
    {
        QString strType;

        if( pCurList->sAuthorityInfoAccess.nType == JS_PKI_NAME_TYPE_DNS )
            strType = "DNS";
        else if( pCurList->sAuthorityInfoAccess.nType == JS_PKI_NAME_TYPE_URI )
            strType = "URI";
        else if( pCurList->sAuthorityInfoAccess.nType == JS_PKI_NAME_TYPE_EMAIL )
            strType = "Email";

        if( bShow )
        {
            strVal += QString( "[%1]Authority Info Access\n" ).arg(i);
            strVal += QString( " Access Method=%1\n").arg(pCurList->sAuthorityInfoAccess.pMethod);
            strVal += QString( " Alternative Name:\n" );
            strVal += QString( " %1=%2\n" ).arg(strType).arg(pCurList->sAuthorityInfoAccess.pName );
        }
        else
        {
            if( strVal.length() > 0 ) strVal += "%%";

            strVal += QString( "Method$%1#Type$%2#Name$%3")
                .arg( pCurList->sAuthorityInfoAccess.pMethod )
                .arg( strType )
                .arg( pCurList->sAuthorityInfoAccess.pName );
        }

        pCurList = pCurList->pNext;
        i++;
    }

    if( pAIAList ) JS_PKI_resetExtAuthorityInfoAccessList( &pAIAList );
    return 0;
}

static int _getIDP( const BIN *pBinExt, bool bShow, QString& strVal )
{
    int ret = 0;
    int i = 1;

    JNumValList    *pIDPList = NULL;
    JNumValList    *pCurList = NULL;

    ret = JS_PKI_getIssuingDistPointValue( pBinExt, &pIDPList );

    pCurList = pIDPList;

    while( pCurList )
    {
        QString strType;

        if( pCurList->sNumVal.nNum == JS_PKI_NAME_TYPE_DNS )
            strType = "DNS";
        else if( pCurList->sNumVal.nNum == JS_PKI_NAME_TYPE_URI )
            strType = "URI";
        else if( pCurList->sNumVal.nNum == JS_PKI_NAME_TYPE_EMAIL )
            strType = "Email";

        if( bShow )
        {
            strVal += QString("[%1] Issuing Distribution Point:\n" ).arg(i);
            strVal += QString( " %1=%2\n" ).arg( strType ).arg( pCurList->sNumVal.pValue );
        }
        else
        {
            strVal += QString( "#%1$%2" ).arg( strType ).arg( pCurList->sNumVal.pValue );
        }

        pCurList = pCurList->pNext;
    }

    if( pIDPList ) JS_UTIL_resetNumValList( &pIDPList );
    return 0;
}

static int _getAltName( const BIN *pBinExt, int nNid, bool bShow, QString& strVal )
{
    int     ret = 0;
    JNumValList    *pAltNameList = NULL;
    JNumValList    *pCurList = NULL;

    ret = JS_PKI_getAlternativNameValue( pBinExt, &pAltNameList );

    pCurList = pAltNameList;

    while( pCurList )
    {
        QString strType;
        if( pCurList->sNumVal.nNum == JS_PKI_NAME_TYPE_DNS )
            strType = "DNS";
        else if( pCurList->sNumVal.nNum == JS_PKI_NAME_TYPE_URI )
            strType = "URI";
        else if( pCurList->sNumVal.nNum == JS_PKI_NAME_TYPE_EMAIL )
            strType = "Email";

        if( bShow )
        {
            strVal += QString( "%1=%2\n" ).arg( strType ).arg( pCurList->sNumVal.pValue );
        }
        else
        {
            strVal += QString( "#%1$%2").arg( strType ).arg(pCurList->sNumVal.pValue);
        }

        pCurList = pCurList->pNext;
    }

    if( pAltNameList ) JS_UTIL_resetNumValList( &pAltNameList );
    return 0;
}

static int _getPM( const BIN *pBinExt, bool bShow, QString& strVal )
{
    int ret = 0;
    int i = 1;

    JExtPolicyMappingsList *pPMList = NULL;
    JExtPolicyMappingsList *pCurList = NULL;

    ret = JS_PKI_getPolicyMappingsValue( pBinExt, &pPMList );

    pCurList = pPMList;

    while( pCurList )
    {
        if( bShow )
        {
            strVal += QString( "[%1]Issuer Domain=%2\n" ).arg(i).arg(pCurList->sPolicyMappings.pIssuerDomainPolicy );
            if( pCurList->sPolicyMappings.pSubjectDomainPolicy )
                strVal += QString( " Subject Domain=%1\n" ).arg( pCurList->sPolicyMappings.pSubjectDomainPolicy );
        }
        else
        {
            if( strVal.length() > 0 ) strVal += "%%";

            strVal += QString( "IDP$%1#SDP$%2")
                .arg( pCurList->sPolicyMappings.pIssuerDomainPolicy )
                .arg( pCurList->sPolicyMappings.pSubjectDomainPolicy );
        }

        pCurList = pCurList->pNext;
        i++;
    }

    if( pPMList ) JS_PKI_resetExtPolicyMappingsList( &pPMList );
    return 0;
}


static int _getNC( const BIN *pBinExt, bool bShow, QString& strVal )
{
    int     ret = 0;
    int     pi = 1;
    int     ei = 1;

    JExtNameConstsList     *pNCList = NULL;
    JExtNameConstsList     *pCurList = NULL;

    ret = JS_PKI_getNameConstraintsValue( pBinExt, &pNCList );

    pCurList = pNCList;

    while( pCurList )
    {
        QString strType;
        if( pCurList->sNameConsts.nType == JS_PKI_NAME_TYPE_URI )
            strType = "URI";
        else if( pCurList->sNameConsts.nType == JS_PKI_NAME_TYPE_DNS )
            strType = "DNS";
        else if( pCurList->sNameConsts.nType == JS_PKI_NAME_TYPE_EMAIL )
            strType = "email";


        if( bShow )
        {
            if( pCurList->sNameConsts.nKind == JS_PKI_NAME_CONSTS_KIND_PST )
            {
                if( pi == 1 ) strVal += QString( "Permitted\n" );
                strVal += QString( " [%1]Subtrees(%2..%3)\n" ).arg( pi ).arg( pCurList->sNameConsts.nMax ).arg( pCurList->sNameConsts.nMin );
                strVal += QString( "  %1=%2\n" ).arg( strType ).arg( pCurList->sNameConsts.pValue );

                pi++;
            }
            else
            {
                if( ei == 1 ) strVal += QString( "Excluded\n" );
                strVal += QString( " [%1]Subtrees(%2..%3)\n" ).arg( ei ).arg( pCurList->sNameConsts.nMax ).arg( pCurList->sNameConsts.nMin );
                strVal += QString( "  %1=%2\n" ).arg( strType ).arg( pCurList->sNameConsts.pValue );

                ei++;
            }
        }
        else
        {
            strVal += QString("#%1$%2$%3$%4$%5")
                .arg( pCurList->sNameConsts.nKind )
                .arg( pCurList->sNameConsts.nType )
                .arg(pCurList->sNameConsts.pValue )
                .arg(pCurList->sNameConsts.nMin )
                .arg(pCurList->sNameConsts.nMax );
        }

        pCurList = pCurList->pNext;
    }

    return 0;
}

static int _getCRLReason( const BIN *pBinExt, bool bShow, QString& strVal )
{
    int     ret = 0;
    int     nReason = -1;

    ret = JS_PKI_getCRLReasonValue( pBinExt, &nReason );

    if( nReason >= 0 ) strVal = crl_reasons[nReason];

    return 0;
}


void getInfoValue( const JExtensionInfo *pExtInfo, QString& strVal )
{
    int ret = 0;
    QString strSN = pExtInfo->pOID;
    BIN     binExt = {0,0};

    JS_BIN_decodeHex( pExtInfo->pValue, &binExt );

    if( strSN == kExtNameKeyUsage )
    {
        ret = _getKeyUsage( &binExt, true, strVal );
    }
    else if( strSN == kExtNameCRLNum )
    {
        ret = _getCRLNum( &binExt, true, strVal );
    }
    else if( strSN == kExtNamePolicy )
    {
        ret = _getCertPolicy( &binExt, true, strVal );
    }
    else if( strSN == kExtNameSKI )
    {
        ret = _getSKI( &binExt, true, strVal );
    }
    else if( strSN == kExtNameAKI )
    {
        ret = _getAKI( &binExt, true, strVal );
    }
    else if( strSN == kExtNameEKU )
    {
        ret = _getEKU( &binExt, true, strVal );
    }
    else if( strSN == kExtNameCRLDP )
    {
        ret = _getCRLDP( &binExt, true, strVal );
    }
    else if( strSN == kExtNameBC )
    {
        ret = _getBC( &binExt, true, strVal );
    }
    else if( strSN == kExtNamePC )
    {
        ret = _getPC( &binExt, true, strVal );
    }
    else if( strSN == kExtNameAIA )
    {
        ret = _getAIA( &binExt, true, strVal );
    }
    else if( strSN == kExtNameIDP )
    {
        ret = _getIDP( &binExt, true, strVal );
    }
    else if( strSN == kExtNameSAN || strSN == kExtNameIAN )
    {
        int nNid = JS_PKI_getNidFromSN( strSN.toStdString().c_str() );
        ret = _getAltName( &binExt, nNid, true, strVal );
    }
    else if( strSN == kExtNamePM )
    {
        ret = _getPM( &binExt, true, strVal );
    }
    else if( strSN == kExtNameNC )
    {
        ret = _getNC( &binExt, true, strVal );
    }
    else if( strSN == kExtNameCRLReason )
    {
        ret = _getCRLReason( &binExt, true, strVal );
    }
    else
    {
        strVal = pExtInfo->pValue;
    }

    JS_BIN_reset( &binExt );
}

static const QString _getKeyUsageProfile( const QString strVal )
{
    QString strShow;
    QStringList valList = strVal.split( "#" );

    for( int i = 0; i < valList.size(); i++ )
    {
        QString strOne = valList.at(i);

        if( i != 0 ) strShow += ",";
        strShow += strOne;
    }

    strShow += "\n";

    return strShow;
}

static const QString _getCertPolicyProfile( const QString strVal )
{
    QString strShow;

    QStringList valList = strVal.split( "%%" );

    for( int i = 0; i < valList.size(); i++ )
    {
        QString infoVal = valList.at(i);
        QStringList infoList = infoVal.split( "#" );

        strShow += QString( "[%1]Certificate Policy:\n" ).arg(i+1);

        for( int k = 0; k < infoList.size(); k++ )
        {
            QString oneVal = infoList.at(k);
            QStringList oneList = oneVal.split( "$" );
            if( oneList.size() < 2 ) continue;
            QString strT = oneList.at(0);
            QString strV = oneList.at(1);

            if( strT == "OID" )
            {
                strShow += QString( " Policy Identifier=%1\n" ).arg( strV );
            }
            else if( strT == "CPS" )
            {
                strShow += QString( " [%1,1] CPS = %2\n" ).arg( i+1 ).arg( strV );
            }
            else if( strT == "UserNotice" )
            {
                strShow += QString( " [%1,2] UserNotice = %2\n" ).arg( i+1 ).arg( strV );
            }
        }
    }

    return strShow;
}

static const QString _getSKIProfile( const QString strVal )
{
    QString strShow = "keyIdentifier = [variable]\n";
    return strShow;
}

static const QString _getAKIProfile( const QString strVal )
{
    QString strShow;

    QStringList valList = strVal.split( "#" );

    strShow = "IssuerKeyIdentifier = [variable]\n";

    for( int i = 0; i < valList.size(); i++ )
    {
        QString strOne = valList.at(i);

        if( strOne == "ISSUER" )
            strShow += " CertificateIssuer = [variable]\n";
        else if( strOne == "SERIAL" )
            strShow += " CertificateSerialNumber = [variable]\n";
    }

    return strShow;
}

static const QString _getEKUProfile( const QString strVal )
{
    QString strShow;
    QStringList valList = strVal.split( "#" );

    for( int i = 0; i < valList.size(); i++ )
    {
        QString strOne = valList.at(i);

        if( i != 0 ) strShow += ",";
        strShow += strOne;
    }

    strShow += "\n";

    return strShow;
}

static const QString _getCRLDPProfile( const QString strVal )
{
    QString strShow;
    QStringList infoList = strVal.split( "#" );

    for( int i = 0; i < infoList.size(); i++ )
    {
        QString strPart = infoList.at(i);
        QStringList partList = strPart.split( "$" );
        if( partList.size() < 2 ) continue;

        strShow += QString( "[%1] CRL Distribution Point\n" ).arg(i);
        strShow += QString( " %1=%2\n" ).arg( partList.at(0) ).arg( partList.at(1) );
    }

    return strShow;
}

static const QString _getBCProfile( const QString strVal )
{
    QString strShow;
    QStringList infoList = strVal.split( "#" );

    if( infoList.size() < 1 ) return strShow;

    QString strType = infoList.at(0);
    strShow += QString( "SubjectType=%1\n").arg(strType);

    if( strType == "CA" )
    {
        if( infoList.size() > 1 )
        {
            int nPathLen = infoList.at(1).toInt();
            strShow += QString( "PathLengthConstraint=%1\n" ).arg(nPathLen);
        }
        else
        {
            strShow = QString( "PathLengthConstraint=None\n" );
        }
    }

    return strShow;
}

static const QString _getPCProfile( const QString strVal )
{
    QString strShow;
    QStringList infoList = strVal.split( "#" );

    for( int i = 0; i < infoList.size(); i++ )
    {
        QString strPart = infoList.at(i);
        QStringList partList = strPart.split( "$" );

        if( partList.size() < 2 ) continue;

        QString strType = partList.at(0);
        QString strNum = partList.at(1);

        if( strType == "REP" )
            strShow += QString("RequiredExplicitPolicySkipCerts=%1\n").arg( strNum );
        else if( strType == "IPM" )
            strShow += QString("InhibitPolicyMappingSkipCerts=%1\n").arg( strNum );
    }

    return strShow;
}

static const QString _getIDPProfile( const QString strVal )
{
    QString strShow;
    QStringList infoList = strVal.split( "#" );

    for( int i = 0; i < infoList.size(); i++ )
    {
        QString strPart = infoList.at(i);
        QStringList partList = strPart.split( "$" );

        if( partList.size() < 2 ) continue;

        QString strType = partList.at(0);
        QString strDP = partList.at(1);

        strShow += QString("[%1] Issuing Distribution Point:\n" ).arg(i);
        strShow += QString( " %1=%2\n" ).arg( strType ).arg( strDP );
    }

    return strShow;
}


static const QString _getAIAProfile( const QString strVal )
{
    QString strShow;
    QStringList infoList = strVal.split( "#" );

    for( int i = 0; i < infoList.size(); i++ )
    {
        QString strPart = infoList.at(i);
        QStringList partList = strPart.split( "$" );

        if( partList.size() < 3 ) continue;

        QString strMethod = partList.at(0);
        QString strType = partList.at(1);
        QString strName = partList.at(2);

        strShow += QString( "[%1]Authority Info Access\n" ).arg(i);
        strShow += QString( " Access Method=%1\n").arg( strMethod );
        strShow += QString( " Alternative Name:\n" );
        strShow += QString( " %1=%2\n" ).arg(strType).arg( strName );
    }

    return strShow;
}

static const QString _getAltNameProfile( int nNid, const QString strVal )
{
    QString strShow;
    QStringList infoList = strVal.split( "#" );

    for( int i = 0; i < infoList.size(); i++ )
    {
        QString strPart = infoList.at(i);
        QStringList partList = strPart.split( "$" );

        if( partList.size() < 2 ) continue;

        QString strType = partList.at(0);
        QString strName = partList.at(1);

        strShow += QString( "%1 : %2\n" ).arg( strType ).arg( strName );
    }

    return strShow;
}

static const QString _getPMProfile( const QString strVal )
{
    QString strShow;
    QStringList infoList = strVal.split( "#" );

    for( int i = 0; i < infoList.size(); i++ )
    {
        QString strPart = infoList.at(i);
        QStringList partList = strPart.split( "$" );

        if( partList.size() < 2 ) continue;

        QString strType = partList.at(0);
        QString strOID = partList.at(1);

        if( strType == "IDP" )
            strShow += QString( "[%1]Issuer Domain=%2\n" ).arg(i).arg( strOID );
        else if( strType == "SDP" )
            strShow += QString( " Subject Domain=%1\n" ).arg( strOID );
    }

    return strShow;
}

static const QString _getNCProfile( const QString strVal )
{
    QString strShow;
    QStringList infoList = strVal.split( "#" );
    int pi = 1;
    int ei = 1;

    for( int i = 0; i < infoList.size(); i++ )
    {
        QString strPart = infoList.at(i);
        QStringList partList = strPart.split( "$" );

        if( partList.size() < 3 ) continue;

        QString strType = partList.at(0);
        QString strKind = partList.at(1);
        QString strData = partList.at(2);
        QString strMin;
        QString strMax;

        if( partList.size() >= 5 )
        {
            strMin = partList.at(3);
            strMax = partList.at(4);
        }

        if( strKind == "permittedSubtrees" )
        {
            if( pi == 1 ) strShow += QString( "Permitted\n" );
            strShow += QString( " [%1]Subtrees(%2..%3)\n" ).arg( pi ).arg( strMax ).arg( strMin );
            strShow += QString( "  %1 : %2\n" ).arg( strType ).arg( strData );

            pi++;
        }
        else if( strKind == "excludedSubtrees" )
        {
            if( ei == 1 ) strShow += QString( "Excluded\n" );
            strShow += QString( " [%1]Subtrees(%2..%3)\n" ).arg( ei ).arg( strMax ).arg( strMin );
            strShow += QString( "  %1 : %2\n" ).arg( strType ).arg( strData );

            ei++;
        }
    }

    return strShow;
}

static const QString _getCRLReasonProfile( const QString strVal )
{
    QString strShow;

    strShow = QString( "%1\n" ).arg( strVal );

    return strShow;
}

static const QString _getCRLNumProfile( const QString strVal )
{
    QString strShow;

    if( strVal == "auto" )
        strShow = QString( "CRLNumber = [variable]\n" );
    else
        strShow = QString( "CRLNumber = %1 (%2 Hex)\n" ).arg( strVal.toInt() ).arg( strVal.toInt(), 0, 16);

    return strShow;
}

const QString getProfileExtInfoValue( const QString strSN, const QString& strVal )
{
    QString strShowVal;

    if( strSN == JS_PKI_ExtNameKeyUsage )
    {
        strShowVal = _getKeyUsageProfile( strVal );
    }
    else if( strSN == JS_PKI_ExtNameCRLNum )
    {
        strShowVal = _getCRLNumProfile( strVal );
    }
    else if( strSN == JS_PKI_ExtNamePolicy )
    {
        strShowVal = _getCertPolicyProfile( strVal );
    }
    else if( strSN == JS_PKI_ExtNameSKI )
    {
        strShowVal = _getSKIProfile( strVal );
    }
    else if( strSN == JS_PKI_ExtNameAKI )
    {
        strShowVal = _getAKIProfile( strVal );
    }
    else if( strSN == JS_PKI_ExtNameEKU )
    {
        strShowVal = _getEKUProfile( strVal );
    }
    else if( strSN == JS_PKI_ExtNameCRLDP )
    {
        strShowVal = _getCRLDPProfile( strVal );
    }
    else if( strSN == JS_PKI_ExtNameBC )
    {
        strShowVal = _getBCProfile( strVal );
    }
    else if( strSN == JS_PKI_ExtNamePC )
    {
        strShowVal = _getPCProfile( strVal );
    }
    else if( strSN == JS_PKI_ExtNameAIA )
    {
        strShowVal = _getAIAProfile( strVal );
    }
    else if( strSN == JS_PKI_ExtNameIDP )
    {
        strShowVal = _getIDPProfile( strVal );
    }
    else if( strSN == JS_PKI_ExtNameSAN || strSN == JS_PKI_ExtNameIAN )
    {
        int nNid = JS_PKI_getNidFromSN( strSN.toStdString().c_str() );
        strShowVal = _getAltNameProfile( nNid, strVal );
    }
    else if( strSN == JS_PKI_ExtNamePM )
    {
        strShowVal = _getPMProfile( strVal );
    }
    else if( strSN == JS_PKI_ExtNameNC )
    {
        strShowVal = _getNCProfile( strVal );
    }
    else if( strSN == JS_PKI_ExtNameCRLReason )
    {
        strShowVal = _getCRLReasonProfile( strVal );
    }
    else
    {
        strShowVal = strVal;
    }

    return strShowVal;
}

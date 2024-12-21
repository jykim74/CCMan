#ifndef COMMON_H
#define COMMON_H

#include <QStringList>
#include "js_pki.h"
#include "js_pki_x509.h"

#define     JS_REC_STATUS_NOT_USED          0
#define     JS_REC_STATUS_USED              1

#define     JS_STATUS_INVALID     0
#define     JS_STATUS_STOP        1
#define     JS_STATUS_VALID       2

#define     JS_ADMIN_TYPE_INVALID       0
#define     JS_ADMIN_TYPE_MASTER        1
#define     JS_ADMIN_TYPE_ADMIN         2
#define     JS_ADMIN_TYPE_AUDIT         3

#define     JS_USER_STATUS_INVALID      0
#define     JS_USER_STATUS_REGISTER     1
#define     JS_USER_STATUS_ISSUED       2
#define     JS_USER_STATUS_STOP         3

#define     JS_SIGNER_TYPE_REG     0
#define     JS_SIGNER_TYPE_OCSP    1

#define     JS_CERT_STATUS_INVALID      0
#define     JS_CERT_STATUS_GOOD         1
#define     JS_CERT_STATUS_REVOKE       2
#define     JS_CERT_STATUS_HOLD         3

enum {
    DATA_STRING,
    DATA_HEX,
    DATA_BASE64,
    DATA_URL
};

const int   kListCount = 15;

const int kPeriodDay = 0;
const int kPeriodMonth = 1;
const int kPeriodYear = 2;


const QStringList kMechList = { "RSA", "EC" };
const QStringList kRSAOptionList = { "1024", "2048", "3072", "4096" };
const QStringList kECCOptionList = {
    "secp112r1", "secp112r2", "secp128r1", "secp128r2", "secp160k1",
    "secp160r1", "secp160r2", "secp192k1", "secp224k1", "secp224r1",
    "secp256k1", "secp384r1", "secp521r1", "sect113r1", "sect113r2",
    "sect131r1", "sect131r2", "sect163k1", "sect163r1", "sect163r2",
    "sect193r1", "sect193r2", "sect233k1", "sect233r1", "sect239k1",
    "sect283k1", "sect283r1", "sect409k1", "sect409r1", "sect571k1",
    "sect571r1"
};

const QString kExtNameAIA = "authorityInfoAccess";
const QString kExtNameAKI = "authorityKeyIdentifier";
const QString kExtNameBC = "basicConstraints";
const QString kExtNameCRLDP = "crlDistributionPoints";
const QString kExtNameEKU = "extendedKeyUsage";
const QString kExtNameIAN = "issuerAltName";
const QString kExtNameKeyUsage = "keyUsage";
const QString kExtNameNC = "nameConstraints";
const QString kExtNamePolicy = "certificatePolicies";
const QString kExtNamePC = "policyConstraints";
const QString kExtNamePM = "policyMappings";
const QString kExtNameSKI = "subjectKeyIdentifier";
const QString kExtNameSAN = "subjectAltName";
const QString kExtNameCRLNum = "crlNumber";
const QString kExtNameIDP = "issuingDistributionPoint";
const QString kExtNameCRLReason = "CRLReason";

const QStringList kStatusList = { "Invalid", "Stop", "Valid" };
const QStringList kUserStatus = { "Invalid", "Reigster", "Issued", "Stop" };

const QStringList kHashList = { "SHA1", "SHA224", "SHA256", "SHA384", "SHA512" };
const QStringList kKeyUsageList = {
    "digitalSignature", "nonRepudiation", "keyEncipherment",
    "dataEncipherment", "keyAgreement", "keyCertSign",
    "cRLSign", "encipherOnly", "decipherOnly"
};


const QStringList kExtKeyUsageList = {
    "serverAuth", "clientAuth", "codeSigning",
    "emailProtection", "timeStamping", "OCSPSigning",
    "ipsecIKE", "msCodeInd", "msCodeCom",
    "msCTLSign", "msEFS"
};

const QStringList kCertVersionList = { "V1", "V2", "V3" };
const QStringList kTypeList = { "URI", "email", "DNS" };
const QStringList kAIATargetList = { "OCSP", "caIssuer" };
const QStringList kNCSubList = { "permittedSubtrees", "excludedSubtrees" };
const QStringList kBCTypeList = { "CA", "End Entity" };

const QStringList kRevokeReasonList = {
    "unspecified", "keyCompromise", "CACompromise",
    "affiliationChanged", "superseded", "cessationOfOperation",
    "certificateHold", "removeFromCRL", "holdInstruction",
    "keyTime","CAKeyTime"
};

const QString kTableStyle = "QHeaderView::section {background-color:#404040;color:#FFFFFF;}";


QString getDateTime( time_t tTime );
QString getRecStatusName( int nStatus );
QString getAdminTypeName( int nType );
QString getStatusName( int nStatus );
QString getUserStatusName( int nStatus );
QString getSignerTypeName( int nType );
QString getCertStatusName( int nStatus );
QString getCertStatusSName( int nStatus );
QString getRevokeReasonName( int nReason );

QString getHexString( unsigned char *pData, int nDataLen );

int getDataLen( int nType, const QString strData );
int getDataLen( const QString strType, const QString strData );

void getBINFromString( BIN *pBin, const QString& strType, const QString& strString );
void getBINFromString( BIN *pBin, int nType, const QString& strString );
QString getStringFromBIN( const BIN *pBin, const QString& strType, bool bSeenOnly = false );
QString getStringFromBIN( const BIN *pBin, int nType, bool bSeenOnly = false );


void getInfoValue( const JExtensionInfo *pExtInfo, QString& strVal );
const QString getProfileExtInfoValue( const QString strSN, const QString& strVal );
void getPeriodString( long start, long end, QString& strStart, QString& strEnd );

#endif // COMMON_H

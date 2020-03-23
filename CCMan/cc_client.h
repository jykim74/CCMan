#ifndef CCCLIENT_H
#define CCCLIENT_H

#include <QString>
#include "js_cc.h"

enum {
    CLIENT_TYPE_USER,
    CLIENT_TYPE_CERT
};

class CCClient
{
public:
    CCClient();

    QString baseUrl() { return base_url_; };
    void setBaseUrl( const QString strUrl );
    int getCount( int nType );
    int getNum( int nType );
    int getUserList( int nOffset, int nLimit, JCC_UserList **ppUserList );
    int getUser( int nNum, JCC_User *pUser );
    int delUser( int nNum );
    int getCertPolicyList( JCC_CertPolicyList **ppCertPolicyList );
    int getCertPolicy( int nNum, JCC_CertPolicy *pCertPolicy );
    int getCertPolicyExtList( int nPolicyNum, JCC_PolicyExtList **ppPolicyExtList );
    int getCRLPolicyList( JCC_CRLPolicyList **ppCRLPolicyList );
    int getCRLPolicy( int nNum, JCC_CRLPolicy *pCRLPolicy );
    int getCRLPolicyExtList( int nPolicyNum, JCC_PolicyExtList **ppPolicyExtList );
    int getSigner( int nNum, JCC_Signer *pSigner );
    int getSignerList( int nType, JCC_SignerList **ppSignerList );
    int getCert( int nNum, JCC_Cert *pCert );
    int getCertList( int nOffset, int nLimit, JCC_CertList **ppCertList );
    int getCRL( int nNum, JCC_CRL *pCRL );
    int getCRLList( int nOffset, int nLimit, JCC_CRLList **ppCRLList );
    int getRevoked( int nSeq, JCC_Revoked *pRevoked );
    int getRevokedList( int nOffset, int nLimit, JCC_RevokedList **ppRevokedList );

    int addCRLPolicy( JCC_CRLPolicy *pCRLPolicy );
    int modCRLPolicy( int nNum, JCC_CRLPolicy *pCRLPolicy );
    int delCRLPolicyExts( int nPolicyNum );
    int delCRLPolicy( int nNum );
    int addCRLPolicyExt( int nPolicyNum, JCC_PolicyExt *pPolicyExt );

    int addCertPolicy( JCC_CertPolicy *pCertPolicy );
    int modCertPolicy( int nNum, JCC_CertPolicy *pCertPolicy );
    int delCertPolicyExts( int nPolicyNum );
    int delCertPolicy( int nNum );
    int addCertPolicyExt( int nPolicyNum, JCC_PolicyExt *pPolicyExt );

    int addSigner( JCC_Signer *pSigner );
    int delSigner( int nNum );

    int addRevoked( JCC_Revoked *pRevoked );
    int delRevoked( int nSeq );

    int issueCert( const JCC_IssueCertReq *pReq, JCC_IssueCertRsp *pRsp );
    int issueCRL( const JCC_IssueCRLReq *pReq, JCC_IssueCRLRsp *pRsp );
    int getCA( JCC_NameVal *pRsp );
    int publishClient( int nItemType, int nNum, JCC_CodeMsg *pCodeMsg );
    int getCRLDPList( JCC_NameValList **ppCRLDPList );

private:
    QString     base_url_;
};

#endif // CCCLIENT_H

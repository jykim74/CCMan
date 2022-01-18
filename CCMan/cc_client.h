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
    int searchCount( int nType, const QString strTarget, const QString strWord );
    int getNum( int nType );
    QString getName( int nNum, const char *pTable );
    QString getDN( int nCertNum );
    int getUserList( int nOffset, int nLimit, JCC_UserList **ppUserList );
    int searchUserList( const QString strTarget, const QString strWord, int nOffset, int nLimit, JCC_UserList **ppUserList );
    int getUser( int nNum, JCC_User *pUser );
    int delUser( int nNum );
    int modUser( int nNum, JCC_User *pUser );
    int getAdmin( int nSeq, JCC_Admin *pAdmin );
    int addAdmin( JCC_Admin *pAdmin );
    int modAdmin( int nSeq, JCC_Admin *pAdmin );
    int delAdmin( int nSeq );
    int getAdminList( JCC_AdminList **ppAdminList );
    int getCertProfileList( JCC_CertProfileList **ppCertProfileList );
    int getCertProfile( int nNum, JCC_CertProfile *pCertProfile );
    int getCertProfileExtList( int nProfileNum, JCC_ProfileExtList **ppProfileExtList );
    int getCRLProfileList( JCC_CRLProfileList **ppCRLProfileList );
    int getCRLProfile( int nNum, JCC_CRLProfile *pCRLProfile );
    int getCRLProfileExtList( int nProfileNum, JCC_ProfileExtList **ppProfileExtList );
    int getSigner( int nNum, JCC_Signer *pSigner );
    int getSignerList( int nType, JCC_SignerList **ppSignerList );
    int getCert( int nNum, JCC_Cert *pCert );
    int getCertList( int nOffset, int nLimit, JCC_CertList **ppCertList );
    int searchCertList( const QString strTarget, const QString strWord, int nOffset, int nLimit, JCC_CertList **ppCertList );
    int getCRL( int nNum, JCC_CRL *pCRL );
    int getCRLList( int nOffset, int nLimit, JCC_CRLList **ppCRLList );
    int searchCRLList( const QString strTarget, const QString strWord, int nOffset, int nLimit, JCC_CRLList **ppCRLList );
    int getRevoked( int nSeq, JCC_Revoked *pRevoked );
    int getRevokedList( int nOffset, int nLimit, JCC_RevokedList **ppRevokedList );
    int searchRevokedList( const QString strTarget, const QString strWord, int nOffset, int nLimit, JCC_RevokedList **ppRevokedList );

    int getKMS( int nNum, JCC_KMS *pKMS );
    int getKMSList( int nOffset, int nLimit, JCC_KMSList **ppKMSList );
    int searchKMSList( const QString strTarget, const QString strWord, int nOffset, int nLimit, JCC_KMSList **ppKMSList );
    int getTSP( int nNum, JCC_TSP *pTSP );
    int getTSPList( int nOffset, int nLimit, JCC_TSPList **ppTSPList );
    int searchTSPList( const QString strTarget, const QString strWord, int nOffset, int nLimit, JCC_TSPList **ppTSPList );
    int getAudit( int nNum, JCC_Audit *pAudit );
    int getAuditList( int nOffset, int nLimit, JCC_AuditList **ppAuditList );
    int searchAuditList( const QString strTarget, const QString strWord, int nOffset, int nLimit, JCC_AuditList **ppAuditList );
    int getStatisticsCount( int nStart, int nEnd, const char *pTable );

    int addCRLProfile( JCC_CRLProfile *pCRLProfile );
    int modCRLProfile( int nNum, JCC_CRLProfile *pCRLProfile );
    int delCRLProfileExts( int nProfileNum );
    int delCRLProfile( int nNum );
    int addCRLProfileExt( int nProfileNum, JCC_ProfileExt *pProfileExt );

    int addCertProfile( JCC_CertProfile *pCertProfile );
    int modCertProfile( int nNum, JCC_CertProfile *pCertProfile );
    int delCertProfileExts( int nProfileNum );
    int delCertProfile( int nNum );
    int addCertProfileExt( int nProfileNum, JCC_ProfileExt *pProfileExt );

    int addSigner( JCC_Signer *pSigner );
    int delSigner( int nNum );

    int addRevoked( JCC_Revoked *pRevoked );
    int delRevoked( int nSeq );

    int issueCert( const JCC_IssueCertReq *pReq, JCC_IssueCertRsp *pRsp );
    int issueCRL( const JCC_IssueCRLReq *pReq, JCC_IssueCRLRsp *pRsp );
    int getCA( JCC_NameVal *pRsp );
    int publishClient( int nItemType, int nNum, JCC_CodeMsg *pCodeMsg );
    int getCRLDPList( JCC_NameValList **ppCRLDPList );
    int getCertStatus( const char *pSerial, JCC_CertStatus *pCertStatus );

private:
    QString     base_url_;
};

#endif // CCCLIENT_H

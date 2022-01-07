#include <QDateTime>

#include "common.h"

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

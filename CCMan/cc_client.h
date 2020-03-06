#ifndef CCCLIENT_H
#define CCCLIENT_H

#include <QString>
#include "js_cc_data.h"

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
    int getUserList( JCC_UserList **ppUserList );
    int getUser( int nNum, JCC_User *pUser );
    int delUser( int nNum );

private:
    QString     base_url_;
};

#endif // CCCLIENT_H

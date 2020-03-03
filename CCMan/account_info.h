#ifndef ACCOUNTINFO_H
#define ACCOUNTINFO_H

#include <QString>

class AccountInfo
{ 
public:
    AccountInfo();

    QString token() { return token_; };
    QString perm() { return perm_; };
    QString baseUrl() { return base_url_; };

    void setToken( const QString strToken );
    void setPerm( const QString strPerm );
    void setBaseUrl( const QString strURL );

private:
    QString token_;
    QString perm_;
    QString base_url_;
};

#endif // ACCOUNTINFO_H

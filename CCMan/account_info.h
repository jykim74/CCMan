#ifndef ACCOUNTINFO_H
#define ACCOUNTINFO_H

#include <QString>

class AccountInfo
{ 
public:
    AccountInfo();

    QString token() { return token_; };
    QString perm() { return perm_; };

    void setToken( const QString strToken );
    void setPerm( const QString strPerm );

private:
    QString token_;
    QString perm_;
};

#endif // ACCOUNTINFO_H

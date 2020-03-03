#ifndef CCCLIENT_H
#define CCCLIENT_H

#include <QString>

class CCClient
{
public:
    CCClient();

    QString baseUrl() { return base_url_; };
    void setBaseUrl( const QString strUrl );

private:
    QString     base_url_;
};

#endif // CCCLIENT_H

#include "account_info.h"

AccountInfo::AccountInfo()
{

}

void AccountInfo::setToken(const QString strToken)
{
    token_ = strToken;
}

void AccountInfo::setPerm(const QString strPerm)
{
    perm_ = strPerm;
}


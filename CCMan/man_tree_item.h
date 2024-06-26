#ifndef MANTREEITEM_H
#define MANTREEITEM_H

#include <QStandardItem>

enum {
    ITEM_TYPE_USER = 0,
    ITEM_TYPE_ADMIN,
    ITEM_TYPE_CONFIG,
    ITEM_TYPE_CERT,
    ITEM_TYPE_CRL,
    ITEM_TYPE_REVOKE,
    ITEM_TYPE_REG_SIGNER,
    ITEM_TYPE_OCSP_SIGNER,
    ITEM_TYPE_CERT_PROFILE,
    ITEM_TYPE_CRL_PROFILE,
    ITEM_TYPE_CA,
    ITEM_TYPE_KMS,
    ITEM_TYPE_TSP,
    ITEM_TYPE_STATISTICS,
    ITEM_TYPE_AUDIT,
    ITEM_TYPE_LICENSE,
};

class ManTreeItem : public QStandardItem
{
public:
    ManTreeItem();
    ManTreeItem( const QString name );

    int type() { return type_; };
    void setType( int type );

private:
    int type_;
};

#endif // MANTREEITEM_H

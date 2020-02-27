#ifndef MANTREEMODEL_H
#define MANTREEMODEL_H

#include <QStandardItemModel>

class ManTreeModel : public QStandardItemModel
{
    Q_OBJECT
public:
    ManTreeModel( QObject *parent = nullptr );
};

#endif // MANTREEMODEL_H

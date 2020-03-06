#ifndef MANTREEVIEW_H
#define MANTREEVIEW_H

#include <QTreeView>

class ManTreeItem;

class ManTreeView : public QTreeView
{
    Q_OBJECT
public:
    ManTreeView( QWidget* parent = nullptr );

private slots:
    void showContextMenu( QPoint point );

public:
    ManTreeItem* currentItem();
};

#endif // MANTREEVIEW_H

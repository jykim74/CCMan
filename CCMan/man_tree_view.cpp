#include <QMenu>

#include "man_tree_view.h"
#include "man_tree_item.h"
#include "man_tree_model.h"
#include "mainwindow.h"
#include "man_applet.h"

ManTreeView::ManTreeView( QWidget *parent ) : QTreeView(parent)
{
    setAcceptDrops(false);
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
}

ManTreeItem* ManTreeView::currentItem()
{
    QModelIndex index = currentIndex();

    ManTreeModel *tree_model = (ManTreeModel *)model();
    ManTreeItem *item = (ManTreeItem *)tree_model->itemFromIndex(index);

    return item;
}

void ManTreeView::showContextMenu(QPoint point)
{
    ManTreeItem* item = currentItem();

    if( item == NULL ) return;

    QMenu menu(this);

    if( item->type() == ITEM_TYPE_USER )
    {
        menu.addAction(tr("RegUser"), manApplet->mainWindow(), &MainWindow::regUser);
    }

    menu.exec(QCursor::pos());
}

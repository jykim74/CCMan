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

    if( item->type() == ITEM_TYPE_ADMIN )
    {
        menu.addAction(tr("Register Admin"), manApplet->mainWindow(), &MainWindow::regAdmin );
    }
    else if( item->type() == ITEM_TYPE_CONFIG )
    {
        menu.addAction(tr("Make Config"), manApplet->mainWindow(), &MainWindow::makeConfig );
    }
    else if( item->type() == ITEM_TYPE_USER )
    {
        menu.addAction(tr("RegUser"), manApplet->mainWindow(), &MainWindow::regUser);
        menu.addAction(tr("IssueCert"), manApplet->mainWindow(), &MainWindow::issueCert );
    }
    else if( item->type() == ITEM_TYPE_CERT_PROFILE )
    {
        menu.addAction(tr("MakeCertProfile"), manApplet->mainWindow(), &MainWindow::makeCertProfile );
    }
    else if( item->type() == ITEM_TYPE_CRL_PROFILE )
    {
        menu.addAction(tr("MakeCRLProfile"), manApplet->mainWindow(), &MainWindow::makeCRLProfile );
    }
    else if( item->type() == ITEM_TYPE_REG_SIGNER || item->type() == ITEM_TYPE_OCSP_SIGNER )
    {
        menu.addAction(tr("RegSigner"), manApplet->mainWindow(), &MainWindow::regSigner );
    }
    else if( item->type() == ITEM_TYPE_CRL )
    {
        menu.addAction(tr("IssueCRL"), manApplet->mainWindow(), &MainWindow::issueCRL );
    }
    else if( item->type() == ITEM_TYPE_CA )
    {
        menu.addAction(tr("PublishCACert"), manApplet->mainWindow(), &MainWindow::publishLDAP );
        menu.addAction( tr("CertCAInfo" ), manApplet->mainWindow(), &MainWindow::certCAInfo );
    }
    else if( item->type() == ITEM_TYPE_LICENSE )
    {
        menu.addAction(tr("MakeLicense"), manApplet->mainWindow(), &MainWindow::makeLicense );
    }

    menu.exec(QCursor::pos());
}

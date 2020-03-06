#include "man_tree_item.h"

ManTreeItem::ManTreeItem()
{
    type_ = -1;
    setEditable(false);
}

ManTreeItem::ManTreeItem( const QString name )
{
    type_ = -1;
    setEditable(false);
    setText( name );
}

void ManTreeItem::setType(int type)
{
    type_ = type;
}

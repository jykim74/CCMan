#include "man_right_widget.h"

ManRightWidget::ManRightWidget( QWidget* parent) : QTableWidget(parent)
{
    type_ = -1;
}

void ManRightWidget::setType(int type)
{
    type_ = type;
}

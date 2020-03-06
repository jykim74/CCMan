#ifndef MANRIGHTWIDGET_H
#define MANRIGHTWIDGET_H

#include <QWidget>
#include <QTableWidget>

class ManRightWidget : public QTableWidget
{
public:
    ManRightWidget( QWidget *parent = nullptr );
    int type() { return type_; };
    void setType( int type );

private:
    int type_;
};

#endif // MANRIGHTWIDGET_H

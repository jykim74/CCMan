#include <QGridLayout>

#include "search_menu.h"
#include "mainwindow.h"
#include "man_applet.h"

static QStringList s_condBaseList = {"Page"};

SearchMenu::SearchMenu(QWidget *parent) : QWidget(parent)
{
    page_label_ = new QLabel( tr("0-0 of 0"));
    left_end_btn_ = new QPushButton( tr("<<"));
    left_btn_ = new QPushButton(tr("<"));
    right_end_btn_ = new QPushButton( tr(">>"));
    right_btn_ = new QPushButton( tr(">"));
    cond_combo_ = new QComboBox;
    input_text_ = new QLineEdit;
    search_btn_ = new QPushButton( tr("Search"));

    cond_combo_->addItems( s_condBaseList );

    cur_page_ = 0;
    total_count_ = 0;
    limit_ = 0;

    connect( left_end_btn_, SIGNAL(clicked()), this, SLOT(leftEndPage()));
    connect( left_btn_, SIGNAL(clicked()), this, SLOT(leftPage()));
    connect( right_end_btn_, SIGNAL(clicked()), this, SLOT(rightEndPage()));
    connect( right_btn_, SIGNAL(clicked()), this, SLOT(rightPage()));
    connect( search_btn_, SIGNAL(clicked()), this, SLOT(search()));

    setupUI();
}

void SearchMenu::setupUI()
{
    QGridLayout *layout = new QGridLayout;

    layout->addWidget( left_end_btn_, 0, 0 );
    layout->addWidget( left_btn_, 0, 1 );
    layout->addWidget( page_label_, 0, 2 );
    layout->addWidget( right_btn_, 0, 3 );
    layout->addWidget( right_end_btn_, 0, 4 );
    layout->addWidget( cond_combo_, 0, 5 );
    layout->addWidget( input_text_, 0, 6 );
    layout->addWidget( search_btn_, 0, 7 );

    setLayout( layout );
}

void SearchMenu::setTotalCount( int count )
{
    total_count_ = count;
}

void SearchMenu::setCurPage( int page )
{
    cur_page_ = page;
}

void SearchMenu::setLimit( int limit )
{
    limit_ = limit;
}

void SearchMenu::leftPage()
{
    int type = manApplet->mainWindow()->rightType();
    cur_page_ = cur_page_ - 1;
    if( cur_page_ < 0 ) cur_page_ = 0;

    manApplet->mainWindow()->createRightList( type );
}

void SearchMenu::leftEndPage()
{
    int type = manApplet->mainWindow()->rightType();
    cur_page_ = 0;
    manApplet->mainWindow()->createRightList( type );
}

void SearchMenu::rightPage()
{
    if( limit_ <= 0 ) return;

    int end_page = int ( total_count_ / limit_ );
    int type = manApplet->mainWindow()->rightType();

    cur_page_ = cur_page_ + 1;
    if( cur_page_ > end_page ) cur_page_ = end_page;

    manApplet->mainWindow()->createRightList( type );
}

void SearchMenu::rightEndPage()
{
    if( limit_ <= 0 ) return;

    int type = manApplet->mainWindow()->rightType();
    cur_page_ = int(total_count_ / limit_ );
    manApplet->mainWindow()->createRightList( type );
}

void SearchMenu::search()
{
    if( limit_ <= 0 ) return;

    QString strTarget = cond_combo_->currentText();
    QString strWord = input_text_->text();
    int type = manApplet->mainWindow()->rightType();

    if( strTarget == "Page" )
    {
        cur_page_ = strWord.toInt();
        input_text_->clear();
    }

    manApplet->mainWindow()->createRightList( type );
}

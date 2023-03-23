#include <QGridLayout>

#include "search_menu.h"
#include "mainwindow.h"
#include "man_applet.h"
#include "settings_mgr.h"
#include "man_tree_item.h"

static QStringList s_condBaseList = {"Page"};
static QStringList  s_condCertList = { "Page", "SubjectDN", "Serial" };
static QStringList  s_condCRLList = { "Page" };
static QStringList  s_condKeyPairList = { "Page", "Name" };
static QStringList  s_condReqList = { "Page", "Name" };
static QStringList  s_condRevokeList = { "Page", "Serial" };
static QStringList  s_condUserList = { "Page", "Name", "Email", "SSN" };

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

    cond_combo_->setMinimumWidth( 80 );


    layout->addWidget( left_end_btn_, 0, 0 );
    layout->addWidget( left_btn_, 0, 1 );
    layout->addWidget( page_label_, 0, 2 );
    layout->addWidget( right_btn_, 0, 3 );
    layout->addWidget( right_end_btn_, 0, 4 );
    layout->addWidget( cond_combo_, 0, 5 );
    layout->addWidget( input_text_, 0, 6 );
    layout->addWidget( search_btn_, 0, 7 );

    layout->setMargin(0);
    setLayout( layout );
}

void SearchMenu::setCondCombo()
{
    cond_combo_->clear();
    input_text_->clear();
    int nType = manApplet->mainWindow()->rightType();

    if( nType == ITEM_TYPE_CERT )
        cond_combo_->addItems( s_condCertList );
    else if( nType == ITEM_TYPE_REVOKE )
        cond_combo_->addItems( s_condRevokeList );
    else if( nType == ITEM_TYPE_USER  )
        cond_combo_->addItems( s_condUserList );
    else if( nType == ITEM_TYPE_CRL )
        cond_combo_->addItems( s_condCRLList );
    else
        cond_combo_->addItems( s_condBaseList );
}


void SearchMenu::updatePageLabel()
{
    int nOffset = cur_page_ * manApplet->settingsMgr()->listCount();
    int nEnd = nOffset + manApplet->mainWindow()->rightCount();

    QString label = QString( "%1-%2 of %3 [%4p]" )
            .arg( nOffset + 1 )
            .arg( nEnd )
            .arg( total_count_ )
            .arg( cur_page_ + 1 );

    page_label_->setText( label );
    setCondCombo();
}

void SearchMenu::setTotalCount( int count )
{
    total_count_ = count;
}

void SearchMenu::setCurPage( int page )
{
    cur_page_ = page;
}

QString SearchMenu::getCondName()
{
    return cond_combo_->currentText();
}

QString SearchMenu::getInputWord()
{
    return input_text_->text();
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
    int nListCount = manApplet->settingsMgr()->listCount();
    int end_page = int ( (total_count_ - 1 ) / nListCount );
    int type = manApplet->mainWindow()->rightType();

    fprintf( stderr, "cur_page : %d, end_page : %d\n", cur_page_, end_page );

    cur_page_ = cur_page_ + 1;
    if( cur_page_ >= end_page ) cur_page_ = end_page;

    manApplet->mainWindow()->createRightList( type );
}

void SearchMenu::rightEndPage()
{
    int nListCount = manApplet->settingsMgr()->listCount();
    int type = manApplet->mainWindow()->rightType();
    int end_page = int(( total_count_ - 1 ) / nListCount);

    cur_page_ = end_page;

    manApplet->mainWindow()->createRightList( type );
}

void SearchMenu::search()
{
    QString strTarget = cond_combo_->currentText();
    QString strWord = input_text_->text();
    int type = manApplet->mainWindow()->rightType();

    if( strTarget == "Page" )
    {
        cur_page_ = strWord.toInt();
        cur_page_--;
        if( cur_page_ < 0 ) cur_page_ = 0;

        input_text_->clear();
    }

    manApplet->mainWindow()->createRightList( type );
}

#include "search_form.h"
#include "common.h"
#include "man_tree_item.h"
#include "mainwindow.h"
#include "man_applet.h"
#include "settings_mgr.h"

static QStringList  s_condBaseList = { "Page" };
static QStringList  s_condCertList = { "Page", "SubjectDN", "Serial" };
static QStringList  s_condCRLList = { "Page" };
static QStringList  s_condKeyPairList = { "Page", "Name" };
static QStringList  s_condReqList = { "Page", "Name" };
static QStringList  s_condRevokeList = { "Page", "Serial" };
static QStringList  s_condUserList = { "Page", "Name", "Email", "SSN" };

SearchForm::SearchForm(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    mCondCombo->addItems( s_condCertList );

    cur_page_ = 0;
    total_count_ = 0;
    left_type_ = -1;
    left_num_ = -1;

    connect( mLeftEndBtn, SIGNAL(clicked()), this, SLOT(leftEndPage()));
    connect( mLeftBtn, SIGNAL(clicked()), this, SLOT(leftPage()));
    connect( mRightEndBtn, SIGNAL(clicked()), this, SLOT(rightEndPage()));
    connect( mRightBtn, SIGNAL(clicked()), this, SLOT(rightPage()));
    connect( mSearchBtn, SIGNAL(clicked()), this, SLOT(search()));
}

SearchForm::~SearchForm()
{

}

void SearchForm::setTotalCount( int nCount )
{
    total_count_ = nCount;
}

void SearchForm::setCurPage( int nPage )
{
   cur_page_ = nPage;
}


void SearchForm::setLeftType( int nType )
{
    left_type_ = nType;
    setCondCombo();
}

void SearchForm::setLeftNum( int nNum )
{
    left_num_ = nNum;
}

void SearchForm::updatePageLabel()
{
    int nOffset = cur_page_ * manApplet->settingsMgr()->listCount();
    int nEnd = nOffset + manApplet->mainWindow()->rightCount();

    QString label = QString( "%1-%2 of %3 [%4p]" )
            .arg( nOffset + 1 )
            .arg( nEnd )
            .arg( total_count_ )
            .arg(cur_page_+1);

    mPageLabel->setText( label );
}

void SearchForm::setCondCombo()
{
    mCondCombo->clear();
    mInputText->clear();

    int nType = manApplet->mainWindow()->rightType();

    if( nType == ITEM_TYPE_CERT )
        mCondCombo->addItems( s_condCertList );
    else if( nType == ITEM_TYPE_REVOKE )
        mCondCombo->addItems( s_condRevokeList );
    else if( nType == ITEM_TYPE_USER  )
        mCondCombo->addItems( s_condUserList );
    else if( nType == ITEM_TYPE_CRL )
        mCondCombo->addItems( s_condCRLList );
    else
        mCondCombo->addItems( s_condBaseList );
}

QString SearchForm::getCondName()
{
    return mCondCombo->currentText();
}

QString SearchForm::getInputWord()
{
    return mInputText->text();
}

void SearchForm::leftPage()
{
    int type = manApplet->mainWindow()->rightType();
    cur_page_ = cur_page_ - 1;
    if( cur_page_ < 0 ) cur_page_ = 0;

    manApplet->mainWindow()->createRightList( type );
}

void SearchForm::leftEndPage()
{
    int type = manApplet->mainWindow()->rightType();
    cur_page_ = 0;
    manApplet->mainWindow()->createRightList( type );
}

void SearchForm::rightPage()
{
    int nListCount = manApplet->settingsMgr()->listCount();
    int end_page = int ( (total_count_ - 1 ) / nListCount );
    int type = manApplet->mainWindow()->rightType();

    fprintf( stderr, "cur_page : %d, end_page : %d\n", cur_page_, end_page );

    cur_page_ = cur_page_ + 1;
    if( cur_page_ >= end_page ) cur_page_ = end_page;

    manApplet->mainWindow()->createRightList( type );
}

void SearchForm::rightEndPage()
{
    int nListCount = manApplet->settingsMgr()->listCount();
    int type = manApplet->mainWindow()->rightType();
    int end_page = int(( total_count_ - 1 ) / nListCount);

    cur_page_ = end_page;

    manApplet->mainWindow()->createRightList( type );
}

void SearchForm::search()
{
    QString strTarget = mCondCombo->currentText();
    QString strWord = mInputText->text();
    int type = manApplet->mainWindow()->rightType();

    if( strTarget == "Page" )
    {
        cur_page_ = strWord.toInt();
        cur_page_--;
        if( cur_page_ < 0 ) cur_page_ = 0;

        mInputText->clear();
    }

    manApplet->mainWindow()->createRightList( type );
}

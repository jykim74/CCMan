#include "admin_dlg.h"
#include "common.h"
#include "man_applet.h"
#include "cc_client.h"

const QStringList kAdminType = { "Invalid", "Master", "Admin", "Audit" };

AdminDlg::AdminDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    connect( mRegisterBtn, SIGNAL(clicked()), this, SLOT(clickRegister()));
    connect( mModifyBtn, SIGNAL(clicked()), this, SLOT(clickModify()));
    connect( mDeleteBtn, SIGNAL(clicked()), this, SLOT(clickDelete()));

    initialize();
}

AdminDlg::~AdminDlg()
{

}

void AdminDlg::initialize()
{
    mStatusCombo->addItems( kStatusList );
    mStatusCombo->setCurrentIndex( 2 );

    mTypeCombo->addItems( kAdminType );
    mTypeCombo->setCurrentIndex( 2 );

    mRegisterBtn->show();
    mModifyBtn->hide();
    mDeleteBtn->hide();
}

void AdminDlg::setEditMode( int nSeq )
{
    int ret = 0;
    JCC_Admin sAdmin;
    CCClient* ccClient = manApplet->ccClient();

    memset( &sAdmin, 0x00, sizeof(sAdmin));

    ret = ccClient->getAdmin( nSeq, &sAdmin );

    mRegisterBtn->hide();
    mModifyBtn->show();
    mDeleteBtn->show();

    mNameText->setText( sAdmin.pName );
    mPasswordText->setText( sAdmin.pPassword );
    mEmailText->setText( sAdmin.pEmail );

    mStatusCombo->setCurrentIndex( sAdmin.nStatus );
    mTypeCombo->setCurrentIndex( sAdmin.nType );

    JS_DB_resetAdmin( &sAdmin );
}

void AdminDlg::showEvent( QShowEvent *event )
{

}

void AdminDlg::clickRegister()
{
    CCClient* ccClient = manApplet->ccClient();
    QString strName = mNameText->text();
    QString strPassword = mPasswordText->text();
    QString strEmail = mEmailText->text();
    int nStatus = mStatusCombo->currentIndex();
    int nType = mTypeCombo->currentIndex();

    JCC_Admin sAdmin;

    memset( &sAdmin, 0x00, sizeof(sAdmin));

    if( strName.isEmpty() )
    {
        manApplet->warningBox( tr("You have to set name"), this );
        return;
    }

    if( strPassword.isEmpty() )
    {
        manApplet->warningBox( tr( "You have to set password" ), this );
        return;
    }

    if( strEmail.isEmpty() )
    {
        manApplet->warningBox( tr( "You have to set email" ), this );
        return;
    }

    JS_DB_setAdmin( &sAdmin,
                    -1,
                    strName.toStdString().c_str(),
                    strPassword.toStdString().c_str(),
                    strEmail.toStdString().c_str(),
                    nStatus,
                    nType );

    ccClient->addAdmin( &sAdmin );
    JS_DB_resetAdmin( &sAdmin );
}

void AdminDlg::clickModify()
{

}

void AdminDlg::clickDelete()
{

}

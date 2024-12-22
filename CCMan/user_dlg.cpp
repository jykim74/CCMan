#include "mainwindow.h"
#include "man_applet.h"
#include "js_cc.h"
#include "js_http.h"

#include "user_dlg.h"
#include "account_info.h"
#include "cc_client.h"
#include "common.h"

UserDlg::UserDlg(QWidget *parent) :
    QDialog(parent)
{
    seq_ = -1;
    setupUi(this);

    connect( mCloseBtn, SIGNAL(clicked()), this, SLOT(close()));
    connect( mRegisterBtn, SIGNAL(clicked()), this, SLOT(clickRegister()));
    connect( mModifyBtn, SIGNAL(clicked()), this, SLOT(clickModify()));
    connect( mDeleteBtn, SIGNAL(clicked()), this ,SLOT(clickDelete()));

    initialize();

#if defined(Q_OS_MAC)
    layout()->setSpacing(5);
#endif
    resize(minimumSizeHint().width(), minimumSizeHint().height());
}

UserDlg::~UserDlg()
{

}

void UserDlg::initialize()
{
    mStatusCombo->addItems( kUserStatus );

    mStatusCombo->setEnabled( false );
    mRefNumText->setEnabled( false );
    mAuthCodeText->setEnabled( false );

    mRegisterBtn->show();
    mModifyBtn->hide();
    mDeleteBtn->hide();
}

void UserDlg::setEditMode( int nSeq )
{
    int ret = 0;
    CCClient* ccClient = manApplet->ccClient();

    JCC_User sUser;

    memset( &sUser, 0x00, sizeof(sUser));

    ret = ccClient->getUser( nSeq, &sUser );

    mRegisterBtn->hide();
    mModifyBtn->show();
    mDeleteBtn->show();

    mNameText->setText( sUser.pName );
    mSSNText->setText( sUser.pSSN );
    mEmailText->setText( sUser.pEmail );

    mStatusCombo->setCurrentIndex( sUser.nStatus );
    mRefNumText->setText( sUser.pRefNum );
    mAuthCodeText->setText( sUser.pAuthCode );
    mRefNumText->setReadOnly(true);
    mAuthCodeText->setReadOnly(true);

    JS_DB_resetUser( &sUser );
    seq_ = nSeq;
}

void UserDlg::clickRegister()
{
    int     ret = 0;
    char    *pReq = NULL;
    char    *pRsp = NULL;
    const char *pContentType = "application/json";
    int     nStatusCode = -1;

    JCC_RegUserReq  sRegUserReq;
    JCC_RegUserRsp  sRegUserRsp;

    memset( &sRegUserReq, 0x00, sizeof(sRegUserReq));
    memset( &sRegUserRsp, 0x00, sizeof(sRegUserRsp));

    QString strName = mNameText->text();
    QString strSSN = mSSNText->text();
    QString strEmail = mEmailText->text();

    QString strURL = manApplet->ccClient()->baseUrl();
    strURL += JS_CC_PATH_USER;

    QString strToken = manApplet->accountInfo()->token();

    JS_CC_setRegUserReq( &sRegUserReq,
                         strName.toStdString().c_str(),
                         strSSN.toStdString().c_str(),
                         strEmail.toStdString().c_str() );

    JS_CC_encodeRegUserReq( &sRegUserReq, &pReq );

    ret = JS_HTTP_requestTokenPost(
                strURL.toStdString().c_str(),
                pContentType,
                strToken.toStdString().c_str(),
                pReq,
                &nStatusCode,
                &pRsp );

    JS_CC_decodeRegUserRsp( pRsp, &sRegUserRsp );

    JS_CC_resetRegUserReq( &sRegUserReq );
    if( pReq ) JS_free( pReq );
    if( pRsp ) JS_free( pRsp );

    if( nStatusCode == JS_HTTP_STATUS_OK )
    {
        QString strMsg = QString( "The success to register user(%1:%2)")
                .arg( sRegUserRsp.pRefNum)
                .arg( sRegUserRsp.pAuthCode );
        manApplet->messageBox( strMsg, this );
        JS_CC_resetRegUserRsp( &sRegUserRsp );
        QDialog::accept();
    }
    else
    {
        JS_CC_resetRegUserRsp( &sRegUserRsp );
        manApplet->warningBox(tr("fail to register user"), this );
        QDialog::reject();
    }

    manApplet->mainWindow()->createRightUserList();
    QDialog::accept();
}


void UserDlg::clickModify()
{
    int ret = 0;
    JCC_User sUser;
    CCClient* ccClient = manApplet->ccClient();

    memset( &sUser, 0x00, sizeof(sUser));

    if(seq_ < 0 )
    {
        manApplet->warningBox(tr("User is not selected"), this);
        return;
    }

    JS_DB_setUser( &sUser,
                   seq_,
                   -1,
                   mNameText->text().toStdString().c_str(),
                   mSSNText->text().toStdString().c_str(),
                   mEmailText->text().toStdString().c_str(),
                   mStatusCombo->currentIndex(),
                   mRefNumText->text().toStdString().c_str(),
                   mAuthCodeText->text().toStdString().c_str() );

    ret = ccClient->modUser( seq_, &sUser );
    JS_DB_resetUser( &sUser );

    manApplet->mainWindow()->createRightUserList();
    QDialog::accept();
}

void UserDlg::clickDelete()
{
    int ret = 0;
    CCClient* ccClient = manApplet->ccClient();

    if(seq_ < 0 )
    {
        manApplet->warningBox(tr("User is not selected"), this);
        return;
    }

    ret = ccClient->delUser( seq_ );
    manApplet->mainWindow()->createRightUserList();
    QDialog::accept();
}

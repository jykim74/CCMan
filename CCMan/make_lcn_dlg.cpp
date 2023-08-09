#include "js_license.h"

#include "make_lcn_dlg.h"
#include "common.h"
#include "man_applet.h"
#include "mainwindow.h"
#include "cc_client.h"

const QStringList kLCNStatus = { "None", "Issue" };

MakeLCNDlg::MakeLCNDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    connect( mClearBtn, SIGNAL(clicked()), this, SLOT(clickClear()));
    connect( mMakeBtn, SIGNAL(clicked()), this, SLOT(clickMake()));
    connect( mCloseBtn, SIGNAL(clicked()), this, SLOT(close()));
    connect( mUseDaysCheck, SIGNAL(clicked()), this, SLOT(checkUseDays()));

    initialize();
}

MakeLCNDlg::~MakeLCNDlg()
{

}

void MakeLCNDlg::initialize()
{
    QString strSID = "jykim74@gmail.com";
    QString strUser = "JSInc";
    QString strProductName = "BerEditor|CertMan|CryptokiMan";
    QString strQuantity = "1";
    QString strExtension = "Base";
    QString strDays = "90";

    mStatusCombo->addItems( kLCNStatus );

    mSIDText->setText( strSID );
    mUserText->setText( strUser );
    mProductNameText->setText( strProductName );
    mQuantityText->setText( strQuantity );
    mExtensionText->setText( strExtension );
    mDaysText->setText( strDays );

    mIssueDateTime->setDateTime( QDateTime::currentDateTime() );
    mExpireDateTime->setDateTime( QDateTime::currentDateTime() );

    mUseDaysCheck->setChecked(true);
    checkUseDays();
}

void MakeLCNDlg::clickClear()
{
    mSIDText->clear();
    mUserText->clear();
    mProductNameText->clear();
    mQuantityText->clear();
    mExtensionText->clear();
    mDaysText->clear();
}

void MakeLCNDlg::checkUseDays()
{
    bool bVal = mUseDaysCheck->isChecked();

    mDaysText->setEnabled( bVal );
    mDaysLabel->setEnabled( bVal );
    mIssueDateLabel->setEnabled( !bVal );
    mIssueDateTime->setEnabled( !bVal );
    mExpireDateLabel->setEnabled( !bVal );
    mExpireDateTime->setEnabled( !bVal );
}

void MakeLCNDlg::clickMake()
{
    CCClient* ccClient = manApplet->ccClient();
    JCC_LCN sLCN;

    QString strSID = mSIDText->text();
    QString strUser = mUserText->text();
    int nStatus = mStatusCombo->currentIndex();
    QString strQuantity = mQuantityText->text();
    QString strProductName = mProductNameText->text();
    QString strExtension = mExtensionText->text();

    QString strIssueDate;
    QString strExpireDate;
    QString strValidPeriod = mDaysText->text();

    time_t now_t = time(NULL);


    memset( &sLCN, 0x00, sizeof(sLCN));

    if( mUseDaysCheck->isChecked() )
    {

        QDateTime dateTime;

        dateTime.setTime_t( now_t );
        strIssueDate = dateTime.toString( LICENSE_TIME_FORMAT );

        dateTime.setTime_t( now_t + strValidPeriod.toInt() * 86400 );
        strExpireDate = dateTime.toString( LICENSE_TIME_FORMAT );
    }
    else
    {
        strIssueDate = mIssueDateTime->dateTime().toString( LICENSE_TIME_FORMAT );
        strExpireDate = mExpireDateTime->dateTime().toString( LICENSE_TIME_FORMAT );
    }

    JS_DB_setLCN( &sLCN,
                  -1,
                  now_t,
                  nStatus,
                  strQuantity.toInt(),
                  strSID.toStdString().c_str(),
                  strUser.toStdString().c_str(),
                  strIssueDate.toStdString().c_str(),
                  strExpireDate.toStdString().c_str(),
                  strProductName.toStdString().c_str(),
                  strExtension.toStdString().c_str(),
                  NULL,
                  NULL );

    ccClient->addLCN( &sLCN );
    manApplet->mainWindow()->createRightLCNList();
    QDialog::accept();
}

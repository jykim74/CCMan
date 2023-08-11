#include "js_license.h"

#include "make_lcn_dlg.h"
#include "common.h"
#include "man_applet.h"
#include "mainwindow.h"
#include "cc_client.h"

const QStringList kLCNStatus = { "None", "Issue" };
const QStringList kLCNTypeList = { "Normal", "Update" };
const QStringList kExtList = { "Base", "Person", "Company", "Group", "Demo" };

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
    QString strProduct = "BerEditor|CertMan|CryptokiMan";
    QString strQuantity = "1";
    QString strExtension = "Base";
    QString strDays = "90";

    mStatusCombo->addItems( kLCNStatus );
    mTypeCombo->addItems( kLCNTypeList );

    mSIDText->setText( strSID );
    mUserText->setText( strUser );
    mProductText->setText( strProduct );
    mQuantityText->setText( strQuantity );
    mExtensionCombo->addItems( kExtList );
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
    mProductText->clear();
    mRefText->clear();
    mQuantityText->clear();
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

    int nType = mTypeCombo->currentIndex();
    QString strSID = mSIDText->text();
    QString strUser = mUserText->text();
    int nStatus = mStatusCombo->currentIndex();
    QString strQuantity = mQuantityText->text();
    QString strProduct = mProductText->text();
    QString strRef = mRefText->text();
    QString strExtension = mExtensionCombo->currentText();

    QString strIssueDate;
    QString strExpireDate;
    QString strValidPeriod = mDaysText->text();

    time_t now_t = time(NULL);


    memset( &sLCN, 0x00, sizeof(sLCN));

    if( mUseDaysCheck->isChecked() )
    {

        QDateTime dateTime;

        dateTime.setTime_t( now_t );
        strIssueDate = dateTime.toString( JS_LCN_TIME_FORMAT );

        dateTime.setTime_t( now_t + strValidPeriod.toInt() * 86400 );
        strExpireDate = dateTime.toString( JS_LCN_TIME_FORMAT );
    }
    else
    {
        strIssueDate = mIssueDateTime->dateTime().toString( JS_LCN_TIME_FORMAT );
        strExpireDate = mExpireDateTime->dateTime().toString( JS_LCN_TIME_FORMAT );
    }

    JS_DB_setLCN( &sLCN,
                  -1,
                  now_t,
                  nStatus,
                  nType,
                  strQuantity.toInt(),
                  strSID.toStdString().c_str(),
                  strUser.toStdString().c_str(),
                  strIssueDate.toStdString().c_str(),
                  strExpireDate.toStdString().c_str(),
                  strProduct.toStdString().c_str(),
                  strRef.toStdString().c_str(),
                  strExtension.toStdString().c_str(),
                  NULL,
                  NULL );

    ccClient->addLCN( &sLCN );
    manApplet->mainWindow()->createRightLCNList();
    QDialog::accept();
}

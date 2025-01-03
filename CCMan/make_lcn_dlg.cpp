#include "js_license.h"

#include "make_lcn_dlg.h"
#include "common.h"
#include "man_applet.h"
#include "mainwindow.h"
#include "cc_client.h"

const QStringList kLCNStatus = { "None", "Issue" };
const QStringList kLCNTypeList = { "Normal", "Update" };
const QStringList kExtList = { "Base", "Person", "Company", "Group", "Demo" };
const QStringList kDaysMethodList = { "Normal", "#Start" };

MakeLCNDlg::MakeLCNDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    connect( mClearBtn, SIGNAL(clicked()), this, SLOT(clickClear()));
    connect( mMakeBtn, SIGNAL(clicked()), this, SLOT(clickMake()));
    connect( mCloseBtn, SIGNAL(clicked()), this, SLOT(close()));
    connect( mUseDaysCheck, SIGNAL(clicked()), this, SLOT(checkUseDays()));

    initialize();

#if defined(Q_OS_MAC)
    layout()->setSpacing(5);
#endif
    resize(minimumSizeHint().width(), minimumSizeHint().height());
}

MakeLCNDlg::~MakeLCNDlg()
{

}

void MakeLCNDlg::initialize()
{
    QString strSID = "SID";
    QString strUser = "jykim74@gmail.com";
    QString strProduct = "BerEditor|CertMan|CryptokiMan";
    QString strQuantity = "1";
    QString strExtension = "Base";
    QString strDays = "90";

    mStatusCombo->addItems( kLCNStatus );
    mTypeCombo->addItems( kLCNTypeList );
    mDaysMethodCombo->addItems( kDaysMethodList );

    mSIDText->setText( strSID );
    mUserText->setText( strUser );
    mProductText->setText( strProduct );
    mQuantityText->setText( strQuantity );
    mExtensionCombo->addItems( kExtList );
    mDaysText->setText( strDays );

    QTime startTime;
    startTime.setHMS(0,0,0);
    QTime endTime;
    endTime.setHMS( 23,59,59);

    mIssueDateTime->setDateTime( QDateTime::currentDateTime() );
    mIssueDateTime->setTime( startTime );
    mExpireDateTime->setDateTime( QDateTime::currentDateTime() );
    mExpireDateTime->setTime( endTime );

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

    mDaysMethodCombo->setEnabled( bVal );
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
        QString strMethod = mDaysMethodCombo->currentText();

        if( strMethod == "Normal" )
        {
            dateTime.setTime_t( now_t );
            strIssueDate = dateTime.toUTC().toString( JS_LCN_TIME_FORMAT );

            dateTime.setTime_t( now_t + strValidPeriod.toInt() * 86400 );
            strExpireDate = dateTime.toUTC().toString( JS_LCN_TIME_FORMAT );
        }
        else
        {
            strIssueDate = strMethod;
            strExpireDate = strValidPeriod;
        }
    }
    else
    {
        strIssueDate = mIssueDateTime->dateTime().toUTC().toString( JS_LCN_TIME_FORMAT );
        strExpireDate = mExpireDateTime->dateTime().toUTC().toString( JS_LCN_TIME_FORMAT );
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

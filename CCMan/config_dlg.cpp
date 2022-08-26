#include "config_dlg.h"
#include "man_applet.h"
#include "mainwindow.h"
#include "js_cc.h"
#include "js_cc_client.h"
#include "cc_client.h"

ConfigDlg::ConfigDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    cur_num_ = -1;

    connect( mOKBtn, SIGNAL(clicked()), this, SLOT(clickOK()));
    connect( mCloseBtn, SIGNAL(clicked()), this, SLOT(close()));
}

ConfigDlg::~ConfigDlg()
{

}

void ConfigDlg::setCurNum(int nNum)
{
    cur_num_ = nNum;
}

void ConfigDlg::clickOK()
{
    CCClient* ccClient = manApplet->ccClient();

    JDB_Config sConfig;

    memset( &sConfig, 0x00, sizeof(sConfig));

    int nKind = mKindText->text().toInt();
    QString strName = mNameText->text();
    QString strValue = mValueText->text();

    JS_DB_setConfig( &sConfig,
                     -1,
                     nKind,
                     strName.toStdString().c_str(),
                     strValue.toStdString().c_str() );

    if( cur_num_ > 0)
        ccClient->modConfig( cur_num_, &sConfig );
    else
        ccClient->addConfig( &sConfig );

    manApplet->mainWindow()->createRightConfigList();
    JS_DB_resetConfig( &sConfig );
    close();
}

void ConfigDlg::showEvent(QShowEvent *)
{
    if( cur_num_ > 0 )
    {
        CCClient* ccClient = manApplet->ccClient();

        JDB_Config sConfig;

        memset( &sConfig, 0x00, sizeof(sConfig));
        ccClient->getConfig( cur_num_, &sConfig );

        mNumText->setText( QString( "%1").arg(sConfig.nNum));
        mKindText->setText( QString( "%1").arg(sConfig.nKind));
        mNameText->setText( QString( "%1").arg(sConfig.pName));
        mValueText->setText( QString( "%1").arg(sConfig.pValue));

        JS_DB_resetConfig( &sConfig );
    }
}

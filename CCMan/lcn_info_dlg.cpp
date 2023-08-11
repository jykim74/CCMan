#include "lcn_info_dlg.h"
#include "js_license.h"
#include "js_cc.h"
#include "mainwindow.h"
#include "man_applet.h"
#include "cc_client.h"

LCNInfoDlg::LCNInfoDlg(QWidget *parent) :
    QDialog(parent)
{
    seq_ = -1;
    setupUi(this);

    connect( mCloseBtn, SIGNAL(clicked()), this, SLOT(close()));
}

LCNInfoDlg::~LCNInfoDlg()
{

}

void LCNInfoDlg::initialize()
{
    JCC_LCN sLCN;
    JS_LICENSE_INFO sLCNInfo;
    BIN binLCN = {0,0};

    if( seq_ <= 0 ) return;

    CCClient* ccClient = manApplet->ccClient();

    memset( &sLCN, 0x00, sizeof(sLCN));
    memset( &sLCNInfo, 0x00, sizeof(sLCNInfo));

    ccClient->getLCN( seq_, &sLCN );

    if( sLCN.pLicense )
    {
        JS_BIN_decodeHex( sLCN.pLicense, &binLCN );
        memcpy( &sLCNInfo, binLCN.pVal, binLCN.nLen );

        mTypeText->setText( QString("%1").arg( sLCNInfo.nType ));
        mSIDText->setText( sLCNInfo.sSID );
        mUserText->setText( sLCNInfo.sUser );
        mProductText->setText( sLCNInfo.sProduct );
        mIssuedText->setText( sLCNInfo.sIssued );
        mExpireText->setText( sLCNInfo.sExpire );
        mExtensionText->setText( sLCNInfo.sExt );
        mKeyText->setText( sLCNInfo.sKey );
        mQuantityText->setText( QString("%1").arg( sLCNInfo.nQTY ));
    }

    JS_DB_resetLCN( &sLCN );
    JS_BIN_reset( &binLCN );
}

void LCNInfoDlg::showEvent(QShowEvent *event)
{
    initialize();
}

void LCNInfoDlg::setSeq(int nSeq)
{
    seq_ = nSeq;
}

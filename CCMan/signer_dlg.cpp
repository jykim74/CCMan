#include <QFileDialog>

#include "mainwindow.h"
#include "man_applet.h"
#include "signer_dlg.h"

#include "js_bin.h"
#include "js_pki_x509.h"
#include "js_cc.h"
#include "js_db.h"
#include "js_db_data.h"
#include "cc_client.h"
#include "man_tree_item.h"
#include "common.h"

static QStringList sTypeList = { "REG Signer", "OCSP Signer" };

SignerDlg::SignerDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);
    initUI();
    initialize();
}

SignerDlg::~SignerDlg()
{

}

void SignerDlg::setType(int nType)
{
    mTypeCombo->setCurrentIndex(nType);
}

void SignerDlg::findCert()
{
    QFileDialog::Options options;
    options |= QFileDialog::DontUseNativeDialog;

    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName( this,
                                                     tr("Certificate files"),
                                                     QDir::currentPath(),
                                                     tr("Cert Files (*.crt);;DER Files (*.der);;All Files (*.*)"),
                                                     &selectedFilter,
                                                     options );

    mCertPathText->setText(fileName);
}

void SignerDlg::accept()
{
    int ret = 0;
    BIN binCert = {0,0};
    JCertInfo   sCertInfo;
    JExtensionInfoList  *pExtInfoList = NULL;

    char *pCert = NULL;
    JCC_Signer  sSigner;
    time_t now_t = time(NULL);
    int nItemType = 0;


    memset( &sCertInfo, 0x00, sizeof(sCertInfo));
    memset( &sSigner, 0x00, sizeof(sSigner));

    QString strCertPath = mCertPathText->text();
    int nType = mTypeCombo->currentIndex();
    int nStatus = mStatusCombo->currentIndex();

    JS_BIN_fileReadBER( strCertPath.toStdString().c_str(), &binCert );
    JS_BIN_encodeHex( &binCert, &pCert );

    ret = JS_PKI_getCertInfo( &binCert, &sCertInfo, &pExtInfoList );
    if( ret != 0 )
    {
        goto end;
    }



    JS_DB_setSigner( &sSigner, -1, now_t, nType, sCertInfo.pSubjectName, sCertInfo.pDNHash, nStatus, pCert, mDescText->toPlainText().toStdString().c_str() );
    manApplet->ccClient()->addSigner( &sSigner );

end :
    if( pCert ) JS_free( pCert );
    JS_BIN_reset( &binCert );
    JS_PKI_resetCertInfo( &sCertInfo );
    if( pExtInfoList ) JS_PKI_resetExtensionInfoList( &pExtInfoList );

    JS_DB_resetSigner( &sSigner );

    if( ret == 0 )
    {
        QDialog::accept();
        if(nType == 0 )
            nItemType = ITEM_TYPE_REG_SIGNER;
        else
            nItemType = ITEM_TYPE_OCSP_SIGNER;

        manApplet->mainWindow()->createRightSignerList(nItemType);
    }
}

void SignerDlg::initialize()
{
    mStatusCombo->addItems( kStatusList );
}

void SignerDlg::initUI()
{
    mTypeCombo->addItems(sTypeList);
    connect( mFindCertBtn, SIGNAL(clicked()), this, SLOT(findCert()));
}

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
    BIN binCert = {0,0};
    JCertInfo   sCertInfo;
    JExtensionInfoList  *pExtInfoList = NULL;

    char *pCert = NULL;
    JCC_Signer  sSigner;
    time_t now_t = time(NULL);


    memset( &sCertInfo, 0x00, sizeof(sCertInfo));
    memset( &sSigner, 0x00, sizeof(sSigner));

    QString strCertPath = mCertPathText->text();

    JS_BIN_fileRead( strCertPath.toStdString().c_str(), &binCert );
    JS_BIN_encodeHex( &binCert, &pCert );

    JS_PKI_getCertInfo( &binCert, &sCertInfo, &pExtInfoList );

    int nType = mTypeCombo->currentIndex();

    JS_DB_setSigner( &sSigner, -1, now_t, nType, sCertInfo.pSubjectName, sCertInfo.pDNHash, mStatusText->text().toInt(), pCert, mDescText->toPlainText().toStdString().c_str() );
    manApplet->ccClient()->addSigner( &sSigner );

    if( pCert ) JS_free( pCert );
    JS_BIN_reset( &binCert );
    JS_PKI_resetCertInfo( &sCertInfo );
    if( pExtInfoList ) JS_PKI_resetExtensionInfoList( &pExtInfoList );

    JS_DB_resetSigner( &sSigner );
    QDialog::accept();
    manApplet->mainWindow()->createRightSignerList(nType);
}

void SignerDlg::initialize()
{

}

void SignerDlg::initUI()
{
    mTypeCombo->addItems(sTypeList);
    connect( mFindCertBtn, SIGNAL(clicked()), this, SLOT(findCert()));
}

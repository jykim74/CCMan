#include <QtGlobal>
#include <QtWidgets>

#include "i18n_helper.h"
#include "settings_mgr.h"
#include "settings_dlg.h"
#include "man_applet.h"
#include "auto_update_service.h"

const QStringList kListCountList = { "10", "15", "20", "25", "30" };

SettingsDlg::SettingsDlg(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    mLangCombo->addItems(I18NHelper::getInstance()->getLanguages());

    initialize();
    initFontFamily();

#if defined(Q_OS_MAC)
    layout()->setSpacing(5);
#endif
    resize(minimumSizeHint().width(), minimumSizeHint().height());
}

SettingsDlg::~SettingsDlg()
{

}

void SettingsDlg::updateSettings()
{
    SettingsMgr *mgr = manApplet->settingsMgr();

#ifdef _AUTO_UPDATE
    if( AutoUpdateService::instance()->shouldSupportAutoUpdate() ) {
        bool enabled = mCheckLatestVersionCheck->checkState() == Qt::Checked;
        AutoUpdateService::instance()->setAutoUpdateEnabled(enabled);
    }
#endif

    mgr->setListCount( mListCountCombo->currentText().toInt() );

    bool language_changed = false;

    if( mLangCombo->currentIndex() != I18NHelper::getInstance()->preferredLanguage() )
    {
        language_changed = true;
        I18NHelper::getInstance()->setPreferredLanguage(mLangCombo->currentIndex());
    }

    if( language_changed && manApplet->yesOrNoBox(tr("You have changed language. Restart to apply it?"), this, true))
        manApplet->restartApp();

    mgr->setFontFamily( mFontFamilyCombo->currentText());
}

void SettingsDlg::accept()
{
    updateSettings();
    QDialog::accept();
}

void SettingsDlg::initialize()
{
    SettingsMgr *mgr = manApplet->settingsMgr();

    Qt::CheckState state;

    int nListCnt = mgr->listCount();
    mListCountCombo->addItems( kListCountList );
    mListCountCombo->setCurrentText( QString("%1").arg( nListCnt )  );

#ifdef _AUTO_UPDATE
    if( AutoUpdateService::instance()->shouldSupportAutoUpdate()) {
        state = AutoUpdateService::instance()->autoUpdateEnabled() ? Qt::Checked : Qt::Unchecked;
        mCheckLatestVersionCheck->setCheckState(state);
    }
#endif

    mFontFamilyCombo->setCurrentText( mgr->getFontFamily() );
    mLangCombo->setCurrentIndex(I18NHelper::getInstance()->preferredLanguage());
}

void SettingsDlg::initFontFamily()
{
    SettingsMgr *mgr = manApplet->settingsMgr();

    QFontDatabase fontDB;
    QStringList fontList = fontDB.families();
    mFontFamilyCombo->addItems( fontList );
}

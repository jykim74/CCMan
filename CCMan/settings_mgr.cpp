#include <QSettings>

#include "common.h"
#include "settings_mgr.h"

namespace  {
    const char *kSetBehaviorGroup = "CCMan";
    const char *kSetListCount = "listCount";
    const char *kTSPSrvCertPath = "TSPSrvCertPath";
}

SettingsMgr::SettingsMgr( QObject *parent ) : QObject( parent )
{
    loadSettings();
}

void SettingsMgr::loadSettings()
{
    QSettings settings;

    settings.beginGroup( kSetBehaviorGroup );
    settings.endGroup();
}

void SettingsMgr::setListCount( int nCount )
{
    QSettings settings;

    settings.beginGroup( kSetBehaviorGroup );
    settings.setValue( kSetListCount, nCount );
    settings.endGroup();
}

int SettingsMgr::listCount()
{
    int nCount = kListCount;

    QSettings settings;
    settings.beginGroup( kSetBehaviorGroup );
    nCount = settings.value( kSetListCount, kListCount ).toInt();
    settings.endGroup();

    return nCount;
}

void SettingsMgr::setTSPSrvCertPath(QString strPath)
{
    QSettings   settings;

    settings.beginGroup( kSetBehaviorGroup );
    settings.setValue( kTSPSrvCertPath, strPath );
    settings.endGroup();
}

QString SettingsMgr::getTSPSrvCertPath()
{
    QString strPath;

    QSettings   settings;
    settings.beginGroup( kSetBehaviorGroup );
    strPath = settings.value( kTSPSrvCertPath, "").toString();
    settings.endGroup();

    return strPath;
}

#ifndef SETTINGSMGR_H
#define SETTINGSMGR_H

#include <QObject>

class SettingsMgr : public QObject
{
    Q_OBJECT

public:
    SettingsMgr( QObject *parent = nullptr );

    void setListCount( int nCount );
    int listCount();

    void loadSettings();

private:
    Q_DISABLE_COPY(SettingsMgr)
};

#endif // SETTINGSMGR_H

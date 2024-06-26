#ifndef AUTOUPDATESERVICE_H
#define AUTOUPDATESERVICE_H

#include <QObject>
#include "singleton.h"

#define STR(s)          #s
#define STRINGIZE(x)    STR(x)


class AutoUpdateAdapter;

class AutoUpdateService : public QObject
{
    SINGLETON_DEFINE(AutoUpdateService)
    Q_OBJECT
public:
    AutoUpdateService(QObject *parent = nullptr);

    bool shouldSupportAutoUpdate() const;
    void setRequestParams();
    bool autoUpdateEnabled() const;
    void setAutoUpdateEnabled(bool enabled);

    void start();
    void stop();

    void checkUpdate();

private:
    void enableUpdateByDefault();
    QString getAppcastURI();
    AutoUpdateAdapter *adapter_;
};

#endif // AUTOUPDATESERVICE_H

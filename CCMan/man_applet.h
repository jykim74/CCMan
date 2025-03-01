#ifndef MANAPPLET_H
#define MANAPPLET_H

#include <QObject>
#include <QMessageBox>

class MainWindow;
class SettingsMgr;
class AccountInfo;
class CCClient;

class ManApplet : public QObject
{
    Q_OBJECT
public:
    ManApplet(QObject *parent = nullptr );
    ~ManApplet();

    void start();
    void log( const QString strLog, QColor cr = QColor(00,00,00) );

    MainWindow* mainWindow() { return main_win_; };
    SettingsMgr* settingsMgr() { return settings_mgr_; };
    AccountInfo* accountInfo() { return account_info_; };
    CCClient* ccClient() { return cc_client_; };

    void messageBox(const QString& msg, QWidget *parent=0);
    void warningBox(const QString& msg, QWidget *parent=0);
    bool yesOrNoBox(const QString& msg, QWidget *parent=0, bool default_val=true);
    bool detailedYesOrNoBox(const QString& msg, const QString& detailed_text, QWidget *parent, bool default_val=true);
    QMessageBox::StandardButton yesNoCancelBox(const QString& msg,
                                               QWidget *parent,
                                               QMessageBox::StandardButton default_btn);
    bool yesOrCancelBox(const QString& msg, QWidget *parent, bool default_ok);

    QString getBrand();
    void restartApp();

    void exitApp( int nNum = 0 );
private:

public slots:

private:
    Q_DISABLE_COPY(ManApplet)

    MainWindow* main_win_;
    SettingsMgr* settings_mgr_;
    AccountInfo* account_info_;
    CCClient*   cc_client_;

    bool in_exit_;
};

extern ManApplet *manApplet;

#endif // MANAPPLET_H

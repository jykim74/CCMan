#ifndef CONFIG_DLG_H
#define CONFIG_DLG_H

#include <QDialog>
#include "ui_config_dlg.h"

namespace Ui {
class ConfigDlg;
}

class ConfigDlg : public QDialog, public Ui::ConfigDlg
{
    Q_OBJECT

public:
    explicit ConfigDlg(QWidget *parent = nullptr);
    ~ConfigDlg();

    void setCurNum( int nNum );

private slots:
    void clickOK();
    void showEvent(QShowEvent *);

private:
    Ui::ConfigDlg *ui;
    int cur_num_;
};

#endif // CONFIG_DLG_H

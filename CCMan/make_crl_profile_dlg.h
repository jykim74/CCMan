#ifndef MAKE_CRL_PROFILE_DLG_H
#define MAKE_CRL_PROFILE_DLG_H

#include <QDialog>
#include "ui_make_crl_profile_dlg.h"
#include "js_cc.h"

class ProfileExtRec;

namespace Ui {
class MakeCRLProfileDlg;
}

class MakeCRLProfileDlg : public QDialog, public Ui::MakeCRLProfileDlg
{
    Q_OBJECT

public:
    explicit MakeCRLProfileDlg(QWidget *parent = nullptr);
    ~MakeCRLProfileDlg();

    void setEdit( bool is_edit );
    bool isEdit() { return is_edit_; };
    void setProfileNum( int profile_num );
    int profileNum() { return profile_num_; };

private slots:
    void showEvent(QShowEvent *event);
    virtual void accept();

    void clickCRLNum();
    void clickAKI();
    void clickIDP();
    void clickIAN();
    void clickUseFromNow();

    void addIDP();
    void addIAN();

    void clearIDP();
    void clearIAN();

private:
    void initUI();
    void connectExtends();
    void setExtends();
    void setTableMenus();

    void saveCRLNumUse( int nProfileNum );
    void saveAKIUse( int nProfileNum );
    void saveIDPUse( int nProfileNum );
    void saveIANUse( int nProfileNum );

    void setCRLNumUse( JCC_ProfileExt *pProfileExt );
    void setAKIUse( JCC_ProfileExt *pProfileExt );
    void setIDPUse( JCC_ProfileExt *pProfileExt );
    void setIANUse( JCC_ProfileExt *pProfileExt );

    void initialize();
    void loadProfile();
    void defaultProfile();

    bool is_edit_;
    int profile_num_;
};

#endif // MAKE_CRL_PROFILE_DLG_H
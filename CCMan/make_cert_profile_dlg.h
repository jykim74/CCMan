#ifndef MAKE_CERT_PROFILE_DLG_H
#define MAKE_CERT_PROFILE_DLG_H

#include <QDialog>
#include "ui_make_cert_profile_dlg.h"
#include "js_cc.h"

class ProfileExtRec;

namespace Ui {
class MakeCertProfileDlg;
}

class MakeCertProfileDlg : public QDialog, public Ui::MakeCertProfileDlg
{
    Q_OBJECT

public:
    explicit MakeCertProfileDlg(QWidget *parent = nullptr);
    ~MakeCertProfileDlg();
    void setEdit( bool is_edit );
    bool isEdit() { return is_edit_; };
    void setProfileNum( int profile_num );
    int profileNum() { return profile_num_; };


private slots:
    void showEvent(QShowEvent *event);
    virtual void accept();

    void clickUseCSR();
    void clickUseDays();

    void clickAIAUse();
    void clickAKIUse();
    void clickBCUse();
    void clickCRLDPUse();
    void clickEKUUse();
    void clickIANUse();
    void clickKeyUsageUse();
    void clickNCUse();
    void clickPolicyUse();
    void clickPCUse();
    void clickPMUse();
    void clickSKIUse();
    void clickSANUse();

    void addKeyUsage();
    void addPolicy();
    void addEKU();
    void addCRLDP();
    void addAIA();
    void addSAN();
    void addIAN();
    void addPM();
    void addNC();

    void clearKeyUsage();
    void clearPolicy();
    void clearEKU();
    void clearCRLDP();
    void clearAIA();
    void clearSAN();
    void clearIAN();
    void clearPM();
    void clearNC();

private:
    void initUI();
    void connectExtends();
    void setExtends();
    void setTableMenus();

    void saveAIAUse( int nProfileNum );
    void saveAKIUse(int nProfileNum );
    void saveBCUse(int nProfileNum );
    void saveCRLDPUse(int nProfileNum );
    void saveEKUUse(int nProfileNum );
    void saveIANUse(int nProfileNum );
    void saveKeyUsageUse(int nProfileNum );
    void saveNCUse(int nProfileNum );
    void savePolicyUse(int nProfileNum );
    void savePCUse(int nProfileNum );
    void savePMUse(int nProfileNum );
    void saveSKIUse(int nProfileNum );
    void saveSANUse(int nProfileNum );

    void setAIAUse( JCC_ProfileExt *pProfileExt );
    void setAKIUse( JCC_ProfileExt *pProfileExt );
    void setBCUse( JCC_ProfileExt *pProfileExt );
    void setCRLDPUse( JCC_ProfileExt *pProfileExt );
    void setEKUUse( JCC_ProfileExt *pProfileExt );
    void setIANUse( JCC_ProfileExt *pProfileExt );
    void setKeyUsageUse( JCC_ProfileExt *pProfileExt );
    void setNCUse( JCC_ProfileExt *pProfileExt );
    void setPolicyUse( JCC_ProfileExt *pProfileExt );
    void setPCUse( JCC_ProfileExt *pProfileExt );
    void setPMUse( JCC_ProfileExt *pProfileExt );
    void setSKIUse( JCC_ProfileExt *pProfileExt );
    void setSANUse( JCC_ProfileExt *pProfileExt );

    void initialize();
    void loadProfile();
    void defaultProfile();

    bool is_edit_;
    int profile_num_;
};

#endif // MAKE_CERT_PROFILE_DLG_H
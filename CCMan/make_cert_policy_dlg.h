#ifndef MAKE_CERT_POLICY_DLG_H
#define MAKE_CERT_POLICY_DLG_H

#include <QDialog>
#include "ui_make_cert_policy_dlg.h"
#include "js_cc.h"

class PolicyExtRec;

namespace Ui {
class MakeCertPolicyDlg;
}

class MakeCertPolicyDlg : public QDialog, public Ui::MakeCertPolicyDlg
{
    Q_OBJECT

public:
    explicit MakeCertPolicyDlg(QWidget *parent = nullptr);
    ~MakeCertPolicyDlg();
    void setEdit( bool is_edit );
    bool isEdit() { return is_edit_; };
    void setPolicyNum( int policy_num );
    int policyNum() { return policy_num_; };


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


private:
    void initUI();
    void connectExtends();
    void setExtends();
    void setTableMenus();

    void saveAIAUse( int nPolicyNum );
    void saveAKIUse(int nPolicyNum );
    void saveBCUse(int nPolicyNum );
    void saveCRLDPUse(int nPolicyNum );
    void saveEKUUse(int nPolicyNum );
    void saveIANUse(int nPolicyNum );
    void saveKeyUsageUse(int nPolicyNum );
    void saveNCUse(int nPolicyNum );
    void savePolicyUse(int nPolicyNum );
    void savePCUse(int nPolicyNum );
    void savePMUse(int nPolicyNum );
    void saveSKIUse(int nPolicyNum );
    void saveSANUse(int nPolicyNum );

    void setAIAUse( JCC_PolicyExt *pPolicyExt );
    void setAKIUse( JCC_PolicyExt *pPolicyExt );
    void setBCUse( JCC_PolicyExt *pPolicyExt );
    void setCRLDPUse( JCC_PolicyExt *pPolicyExt );
    void setEKUUse( JCC_PolicyExt *pPolicyExt );
    void setIANUse( JCC_PolicyExt *pPolicyExt );
    void setKeyUsageUse( JCC_PolicyExt *pPolicyExt );
    void setNCUse( JCC_PolicyExt *pPolicyExt );
    void setPolicyUse( JCC_PolicyExt *pPolicyExt );
    void setPCUse( JCC_PolicyExt *pPolicyExt );
    void setPMUse( JCC_PolicyExt *pPolicyExt );
    void setSKIUse( JCC_PolicyExt *pPolicyExt );
    void setSANUse( JCC_PolicyExt *pPolicyExt );

    void initialize();
    void loadPolicy();
    void defaultPolicy();

    bool is_edit_;
    int policy_num_;
};

#endif // MAKE_CERT_POLICY_DLG_H
#ifndef MAKE_CRL_POLICY_DLG_H
#define MAKE_CRL_POLICY_DLG_H

#include <QDialog>
#include "ui_make_crl_policy_dlg.h"
#include "js_cc.h"

class PolicyExtRec;

namespace Ui {
class MakeCRLPolicyDlg;
}

class MakeCRLPolicyDlg : public QDialog, public Ui::MakeCRLPolicyDlg
{
    Q_OBJECT

public:
    explicit MakeCRLPolicyDlg(QWidget *parent = nullptr);
    ~MakeCRLPolicyDlg();

    void setEdit( bool is_edit );
    bool isEdit() { return is_edit_; };
    void setPolicyNum( int policy_num );
    int policyNum() { return policy_num_; };

private slots:
    void showEvent(QShowEvent *event);
    virtual void accept();

    void clickCRLNum();
    void clickAKI();
    void clickIDP();
    void clickIAN();

    void addIDP();
    void addIAN();


private:
    void initUI();
    void connectExtends();
    void setExtends();
    void setTableMenus();

    void saveCRLNumUse( int nPolicyNum );
    void saveAKIUse( int nPolicyNum );
    void saveIDPUse( int nPolicyNum );
    void saveIANUse( int nPolicyNum );

    void setCRLNumUse( JCC_PolicyExt *pPolicyExt );
    void setAKIUse( JCC_PolicyExt *pPolicyExt );
    void setIDPUse( JCC_PolicyExt *pPolicyExt );
    void setIANUse( JCC_PolicyExt *pPolicyExt );

    void initialize();
    void loadPolicy();
    void defaultPolicy();

    bool is_edit_;
    int policy_num_;
};

#endif // MAKE_CRL_POLICY_DLG_H
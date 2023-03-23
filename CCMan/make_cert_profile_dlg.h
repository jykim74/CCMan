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
    void setEdit( int nProfileNum );
    bool isEdit() { return is_edit_; };
    int profileNum() { return profile_num_; };
    void loadProfile( int nProfileNum, bool bCopy = false );

private slots:
    virtual void accept();

    void slotKeyUsageMenuRequested(QPoint pos);
    void deleteKeyUsageMenu();

    void slotEKUMenuRequested(QPoint pos);
    void deleteEKUMenu();

    void slotPolicyMenuRequested(QPoint pos);
    void deletePolicyMenu();

    void slotCRLDPMenuRequested(QPoint pos);
    void deleteCRLDPMenu();

    void slotAIAMenuRequested(QPoint pos);
    void deleteAIAMenu();

    void slotSANMenuRequested(QPoint pos);
    void deleteSANMenu();

    void slotIANMenuRequested(QPoint pos);
    void deleteIANMenu();

    void slotPMMenuRequested(QPoint pos);
    void deletePMMenu();

    void slotNCMenuRequested(QPoint pos);
    void deleteNCMenu();

    void slotExtensionsMenuRequested(QPoint pos);
    void deleteExtensionsMenu();

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
    void clickExtensionsUse();

    void addKeyUsage();
    void addPolicy();
    void addEKU();
    void addCRLDP();
    void addAIA();
    void addSAN();
    void addIAN();
    void addPM();
    void addNC();
    void addExtensions();

    void clearKeyUsage();
    void clearPolicy();
    void clearEKU();
    void clearCRLDP();
    void clearAIA();
    void clearSAN();
    void clearIAN();
    void clearPM();
    void clearNC();
    void clearExtensions();

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
    void saveExtensionsUse( int nProfileNum );

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
    void setExtensionsUse( JCC_ProfileExt *pProfileExt );

    void initialize();
    void defaultProfile();

    bool is_edit_;
    int profile_num_;
};

#endif // MAKE_CERT_PROFILE_DLG_H

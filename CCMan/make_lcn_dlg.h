#ifndef MAKE_LCN_DLG_H
#define MAKE_LCN_DLG_H

#include <QDialog>
#include "ui_make_lcn_dlg.h"

namespace Ui {
class MakeLCNDlg;
}

class MakeLCNDlg : public QDialog, public Ui::MakeLCNDlg
{
    Q_OBJECT

public:
    explicit MakeLCNDlg(QWidget *parent = nullptr);
    ~MakeLCNDlg();

private slots:
    void clickClear();
    void clickMake();
    void checkUseDays();

private:
    void initialize();
};

#endif // MAKE_LCN_DLG_H

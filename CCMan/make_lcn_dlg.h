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

private:

};

#endif // MAKE_LCN_DLG_H

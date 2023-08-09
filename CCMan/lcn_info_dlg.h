#ifndef LCN_INFO_DLG_H
#define LCN_INFO_DLG_H

#include <QDialog>
#include "ui_lcn_info_dlg.h"

namespace Ui {
class LCNInfoDlg;
}

class LCNInfoDlg : public QDialog, public Ui::LCNInfoDlg
{
    Q_OBJECT

public:
    explicit LCNInfoDlg(QWidget *parent = nullptr);
    ~LCNInfoDlg();

    void setSeq( int nSeq );

private slots:
    void showEvent(QShowEvent *event);

private:
    int seq_;
    void initialize();
};

#endif // LCN_INFO_DLG_H

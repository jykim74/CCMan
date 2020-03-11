#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QTreeView>
#include <QTableWidget>
#include <QTextEdit>

class ManTreeView;
class ManTreeItem;
class ManTreeModel;
class ManRightWidget;
class SearchMenu;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void showWindow();
    void initialize();

    void createActions();
    void createStatusBar();
    void createTableMenu();


    void createRightList( int nItemType );
    void createRightUserList();
    void createRightCertPolicyList();
    void createRightCRLPolicyList();
    void createRightSignerList( int nItemType );
    void createRightCertList();
    void createRightCRLList();
    void createRightRevokedList();
    void createRightCA();

    void showRightBottomUser( int nSeq );
    void showRightBottomCertPolicy( int nNum );
    void showRightBottomCRLPolicy( int nNum );
    void showRightBottomSigner( int nNum );
    void showRightBottomCert( int nNum );
    void showRightBottomCRL( int nNum );
    void showRightBottomRevoked( int nSeq );
    void showRightBottomCA();

    int rightType();
    int rightCount();

private:
    void removeAllRight();
    void createTreeMenu();

public slots:
    void regUser();
    void treeMenuClick( QModelIndex index );
    void rightTableClick( QModelIndex index );

    void showRightMenu(QPoint point);
    void deleteUser();
    void modifyUser();
    void makeCertPolicy();
    void makeCRLPolicy();
    void regSigner();
    void modifyCertPolicy();
    void modifyCRLPolicy();
    void deleteCertPolicy();
    void deleteCRLPolicy();
    void deleteSigner();
    void revokeCert();
    void deleteRevoke();

private:
    QSplitter   *hsplitter_;
    QSplitter   *vsplitter_;

    ManTreeView*    left_tree_;
    ManTreeModel*   left_model_;
//    QTableWidget*   right_table_;
    ManRightWidget*  right_table_;
    QTextEdit*      right_text_;
    SearchMenu*     right_menu_;
};
#endif // MAINWINDOW_H

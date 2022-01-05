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
    void createRightAdminList();
    void createRightUserList();
    void createRightCertProfileList();
    void createRightCRLProfileList();
    void createRightSignerList( int nItemType );
    void createRightCertList();
    void createRightCRLList();
    void createRightRevokedList();
    void createRightCA();

    void showRightBottomUser( int nSeq );
    void showRightBottomCertProfile( int nNum );
    void showRightBottomCRLProfile( int nNum );
    void showRightBottomSigner( int nNum );
    void showRightBottomCert( int nNum );
    void showRightBottomCRL( int nNum );
    void showRightBottomRevoked( int nSeq );
    void showRightBottomCA( int row );

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
    void makeCertProfile();
    void makeCRLProfile();
    void regSigner();
    void modifyCertProfile();
    void modifyCRLProfile();
    void deleteCertProfile();
    void deleteCRLProfile();
    void deleteSigner();
    void revokeCert();
    void deleteRevoke();
    void issueCert();
    void issueCRL();
    void publishLDAP();
    void about();
    void settings();
    void certInfo();
    void crlInfo();
    void certStatus();

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

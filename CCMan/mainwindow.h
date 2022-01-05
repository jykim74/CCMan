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

    void log( const QString strLog, QColor cr = QColor(0x00, 0x00, 0x00) );

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

    void logAdmin( int nSeq );
    void logUser( int nSeq );
    void logCertProfile( int nNum );
    void logCRLProfile( int nNum );
    void logSigner( int nNum );
    void logCert( int nNum );
    void logCRL( int nNum );
    void logRevoked( int nSeq );
    void logCA( int row );

    int rightType();
    int rightCount();

private:
    void logClear();
    void logCursorTop();
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

    ManRightWidget*  right_table_;
    QTextEdit*      log_text_;
    SearchMenu*     right_menu_;
};
#endif // MAINWINDOW_H

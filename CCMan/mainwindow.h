#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QTreeView>
#include <QTableWidget>
#include <QTextEdit>
#include <QStackedLayout>
#include "code_editor.h"

class ManTreeView;
class ManTreeItem;
class ManTreeModel;
class ManRightWidget;
//class SearchMenu;
class SearchForm;
class StatForm;

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
    void createRightConfigList();
    void createRightUserList();
    void createRightCertProfileList();
    void createRightCRLProfileList();
    void createRightSignerList( int nItemType );
    void createRightCertList();
    void createRightCRLList();
    void createRightRevokedList();
    void createRightCA();
#ifdef _ENABLE_CHARTS
    void createRightStatistics();
#endif
    void createRightKMS();
    void createRightTSP();
    void createRightAudit();
    void createRightLCNList();

    void logAdmin( int nSeq );
    void logConfig( int nNum );
    void logUser( int nSeq );
    void logCertProfile( int nNum );
    void logCRLProfile( int nNum );
    void logSigner( int nNum );
    void logCert( int nNum );
    void logCRL( int nNum );
    void logRevoked( int nSeq );
    void logCA( int row );
    void logKMS( int nSeq );
    void logTSP( int nSeq );
    void logAudit( int nSeq );
    void logLCN( int nSeq );

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

    void regAdmin();
    void showRightMenu(QPoint point);
    void deleteUser();
    void modifyUser();
    void modifyAdmin();
    void makeCertProfile();
    void makeCRLProfile();
    void regSigner();
    void modifyCertProfile();
    void modifyCRLProfile();
    void deleteCertProfile();
    void deleteCRLProfile();
    void copyCertProfile();
    void copyCRLProfile();
    void deleteSigner();
    void revokeCert();
    void deleteRevoke();
    void issueCert();
    void issueCRL();
    void publishLDAP();
    void about();
    void settings();
    void certInfo();
    void certCAInfo();
    void crlInfo();
    void certStatus();
    void verifyAudit();

    void makeConfig();
    void editConfig();
    void deleteConfig();

    void viewTSTInfo();
    void verifyTSMessage();

    void makeLicense();
    void deleteLicense();
    void viewLicense();
    void saveLicense();

private:
    QSplitter   *hsplitter_;
    QSplitter   *vsplitter_;

    ManTreeView*    left_tree_;
    ManTreeModel*   left_model_;

    ManRightWidget*  right_table_;
    CodeEditor*      log_text_;
//    SearchMenu*     right_menu_;
    SearchForm*      right_menu_;
#ifdef _ENABLE_CHARTS
    QStackedLayout  *stack_;
    StatForm*       stat_;
#endif
};
#endif // MAINWINDOW_H

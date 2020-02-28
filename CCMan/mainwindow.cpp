#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "man_tree_item.h"
#include "man_tree_model.h"
#include "man_tree_view.h"
#include "search_menu.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initialize();

    setUnifiedTitleAndToolBarOnMac(true);
}

MainWindow::~MainWindow()
{

}

void MainWindow::showWindow()
{
    showNormal();
    show();
    raise();
    activateWindow();
}

void MainWindow::initialize()
{
    hsplitter_ = new QSplitter(Qt::Horizontal);
    vsplitter_ = new QSplitter(Qt::Vertical );

    left_tree_ = new ManTreeView(this);
    left_model_ = new ManTreeModel(this);
    right_menu_ = new SearchMenu;
    right_text_ = new QTextEdit;
    right_table_ = new QTableWidget;

    left_tree_->setModel(left_model_);

    hsplitter_->addWidget(left_tree_);
    hsplitter_->addWidget(vsplitter_);

    vsplitter_->addWidget(right_table_);
    vsplitter_->addWidget(right_menu_);
    vsplitter_->addWidget(right_text_);

    QList<int> vsizes;
    vsizes << 1200 << 10 << 500;
    vsplitter_->setSizes(vsizes);

    QList<int> hsizes;
    hsizes << 500 << 1200;
    hsplitter_->setSizes( hsizes );
    resize(1024, 768);

    setCentralWidget(hsplitter_);
}

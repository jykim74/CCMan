#ifndef STAT_FORM_H
#define STAT_FORM_H

#include <QWidget>
#include <QTableWidget>
#include <QtCharts/QChartGlobal>
#include "ui_stat_form.h"

namespace Ui {
class StatForm;
}

QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_CHARTS_END_NAMESPACE

typedef QPair<QPointF, QString> Data;
typedef QList<Data> DataList;
typedef QList<DataList> DataTable;

QT_CHARTS_USE_NAMESPACE

class StatForm : public QWidget, public Ui::StatForm
{
    Q_OBJECT
private:
    void initialize();
    void getData();
    QChart* createSimpleBarChart() const;
    QChart* createBarChart() const;
    QTableWidget* createSimpleBarTable() const;
    DataTable generateRandomData(int listCount, int valueMax, int valueCount) const;

public:
    explicit StatForm(QWidget *parent = nullptr);
    ~StatForm();

private slots:
    void showEvent(QShowEvent *event);
    void clickUpdate();
    void clickDay();
    void clickMonth();
    void clickYear();

private:
    QList<qreal> cert_list_;
    QList<qreal> revoke_list_;
    QList<qreal> crl_list_;
    QList<qreal> user_list_;
    QList<qreal> tsp_list_;

    QStringList unit_list_;

    QTableWidget*   stat_table_;
    QChartView*     simple_view_;
    QChartView*     bar_view_;

    qreal           max_num_;
};

#endif // STAT_FORM_H
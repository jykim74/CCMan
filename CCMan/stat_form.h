#ifndef STAT_FORM_H
#define STAT_FORM_H

#ifdef _ENABLE_CHARTS

#include <QWidget>
#include <QTableWidget>
#include <QtCharts/QChartGlobal>
#include "ui_stat_form.h"

namespace Ui {
class StatForm;
}

#if QT_VERSION >= 0x060000
QT_BEGIN_NAMESPACE
#else
QT_CHARTS_BEGIN_NAMESPACE
#endif

class QChartView;
class QChart;

#if QT_VERSION >= 0x060000
QT_END_NAMESPACE
#else
QT_CHARTS_END_NAMESPACE
#endif

typedef QPair<QPointF, QString> Data;
typedef QList<Data> DataList;
typedef QList<DataList> DataTable;

#if QT_VERSION >= 0x060000
QT_USE_NAMESPACE
#else
QT_CHARTS_USE_NAMESPACE
#endif

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

#endif

#endif // STAT_FORM_H

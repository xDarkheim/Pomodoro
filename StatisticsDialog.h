#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include <QMap>
#include <QDate>

class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QTabWidget;
class QTextEdit;
class QScrollArea;

class StatisticsChart : public QWidget
{
    Q_OBJECT

public:
    explicit StatisticsChart(QWidget *parent = nullptr);
    void setData(const QMap<QDate, int> &workTimeData);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QMap<QDate, int> m_workTimeData;
    int m_maxValue;
};

class StatisticsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatisticsDialog(QWidget *parent = nullptr);
    void setStatistics(int totalSessions, int totalWorkTime, int totalBreakTime);

private:
    void setupUI();
    void setupOverviewTab();
    void setupChartTab();
    void setupDetailsTab();
    void loadDailyStatistics();
    void updateOverview() const;
    void updateChart() const;

    // UI elements
    QTabWidget *m_tabWidget;

    // Overview tab
    QWidget *m_overviewTab;
    QLabel *m_totalSessionsLabel;
    QLabel *m_totalTimeLabel;
    QLabel *m_workTimeLabel;
    QLabel *m_breakTimeLabel;
    QLabel *m_avgSessionLabel;
    QLabel *m_efficiencyLabel;
    QLabel *m_todayLabel;
    QLabel *m_weekLabel;
    QLabel *m_monthLabel;

    // Chart tab
    QWidget *m_chartTab;
    StatisticsChart *m_chart;
    QLabel *m_chartPeriodLabel;
    QPushButton *m_weekButton;
    QPushButton *m_monthButton;
    QPushButton *m_yearButton;

    // Details tab
    QWidget *m_detailsTab;
    QTextEdit *m_detailsText;

    // Data
    int m_totalSessions;
    int m_totalWorkTime;
    int m_totalBreakTime;
    QMap<QDate, int> m_dailyWorkTime;
    QMap<QDate, int> m_dailySessions;
    QString m_chartPeriod;
};

#endif // STATISTICSDIALOG_H

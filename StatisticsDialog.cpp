#include "StatisticsDialog.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QScrollArea>
#include <QPainter>
#include <QSettings>
#include <QDate>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QApplication>

#include "TimerState.h"

// StatisticsChart implementation
StatisticsChart::StatisticsChart(QWidget *parent)
    : QWidget(parent), m_maxValue(0)
{
    setMinimumSize(400, 200);
    setStyleSheet("background-color: white; border: 1px solid #ccc;");
}

void StatisticsChart::setData(const QMap<QDate, int> &workTimeData)
{
    m_workTimeData = workTimeData;
    m_maxValue = 0;

    for (auto it = workTimeData.begin(); it != workTimeData.end(); ++it) {
        if (it.value() > m_maxValue) {
            m_maxValue = it.value();
        }
    }

    update();
}

void StatisticsChart::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_workTimeData.isEmpty()) {
        painter.drawText(rect(), Qt::AlignCenter, "No data available");
        return;
    }

    constexpr int LABEL_SKIP_INTERVAL = 3;
    const int margin = 40;
    const int chartWidth = width() - 2 * margin;
    const int chartHeight = height() - 2 * margin;

    // Draw axes
    painter.setPen(QPen(Qt::black, 2));
    painter.drawLine(margin, height() - margin, width() - margin, height() - margin); // X-axis
    painter.drawLine(margin, margin, margin, height() - margin); // Y-axis

    if (m_maxValue == 0) return;

    // Draw bars
    const int barWidth = chartWidth / qMax(1, m_workTimeData.size());
    int x = margin;

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(70, 130, 180));

    auto sortedData = m_workTimeData;

    for (auto it = sortedData.begin(); it != sortedData.end(); ++it) {
        const int barHeight = (it.value() * chartHeight) / m_maxValue;
        const int barY = height() - margin - barHeight;

        QRect barRect(x + 2, barY, barWidth - 4, barHeight);
        painter.drawRect(barRect);

        // Draw date labels (every 3rd day to avoid crowding)
        if ((x - margin) % (barWidth * LABEL_SKIP_INTERVAL) == 0) {
            painter.setPen(Qt::black);
            QString dateStr = it.key().toString("dd/MM");
            painter.drawText(x, height() - 10, dateStr);
            painter.setPen(Qt::NoPen);
        }

        x += barWidth;
    }

    // Draw Y-axis labels
    painter.setPen(Qt::black);
    for (int i = 0; i <= 5; ++i) {
        int y = height() - margin - (i * chartHeight / 5);
        int minutes = (i * m_maxValue) / 5;
        painter.drawText(5, y + 5, QString::number(minutes) + "m");
    }
}

// StatisticsDialog implementation
StatisticsDialog::StatisticsDialog(QWidget *parent)
    : QDialog(parent)
    , m_totalSessions(0)
    , m_totalWorkTime(0)
    , m_totalBreakTime(0)
    , m_chartPeriod("week")
{
    setWindowTitle("📊 Pomodoro Statistics");
    setMinimumSize(600, 500);
    setupUI();
    loadDailyStatistics();
}

void StatisticsDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Create tab widget
    m_tabWidget = new QTabWidget();

    setupOverviewTab();
    setupChartTab();
    setupDetailsTab();

    m_tabWidget->addTab(m_overviewTab, "📈 Overview");
    m_tabWidget->addTab(m_chartTab, "📊 Chart");
    m_tabWidget->addTab(m_detailsTab, "📋 Details");

    // No additional buttons - only native window close button
    mainLayout->addWidget(m_tabWidget);

    // Connect chart period buttons
    connect(m_weekButton, &QPushButton::clicked, this, [this]() {
        m_chartPeriod = "week";
        updateChart();
    });
    connect(m_monthButton, &QPushButton::clicked, this, [this]() {
        m_chartPeriod = "month";
        updateChart();
    });
    connect(m_yearButton, &QPushButton::clicked, this, [this]() {
        m_chartPeriod = "year";
        updateChart();
    });
}

void StatisticsDialog::setupOverviewTab()
{
    m_overviewTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_overviewTab);

    // Title
    QLabel *titleLabel = new QLabel("📊 Statistics Overview");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    // Main statistics grid
    QGridLayout *statsGrid = new QGridLayout();

    // Create labels
    m_totalSessionsLabel = new QLabel();
    m_totalTimeLabel = new QLabel();
    m_workTimeLabel = new QLabel();
    m_breakTimeLabel = new QLabel();
    m_avgSessionLabel = new QLabel();
    m_efficiencyLabel = new QLabel();

    // Period statistics
    m_todayLabel = new QLabel();
    m_weekLabel = new QLabel();
    m_monthLabel = new QLabel();

    // Add to grid
    statsGrid->addWidget(new QLabel("🍅 Total Sessions:"), 0, 0);
    statsGrid->addWidget(m_totalSessionsLabel, 0, 1);

    statsGrid->addWidget(new QLabel("⏱️ Total Time:"), 1, 0);
    statsGrid->addWidget(m_totalTimeLabel, 1, 1);

    statsGrid->addWidget(new QLabel("🎯 Work Time:"), 2, 0);
    statsGrid->addWidget(m_workTimeLabel, 2, 1);

    statsGrid->addWidget(new QLabel("☕ Break Time:"), 3, 0);
    statsGrid->addWidget(m_breakTimeLabel, 3, 1);

    statsGrid->addWidget(new QLabel("📊 Avg Session:"), 4, 0);
    statsGrid->addWidget(m_avgSessionLabel, 4, 1);

    statsGrid->addWidget(new QLabel("⚡ Efficiency:"), 5, 0);
    statsGrid->addWidget(m_efficiencyLabel, 5, 1);

    // Period statistics section
    QLabel *periodTitle = new QLabel("📅 Period Statistics");
    QFont periodFont = periodTitle->font();
    periodFont.setPointSize(14);
    periodFont.setBold(true);
    periodTitle->setFont(periodFont);

    QGridLayout *periodGrid = new QGridLayout();
    periodGrid->addWidget(new QLabel("📅 Today:"), 0, 0);
    periodGrid->addWidget(m_todayLabel, 0, 1);

    periodGrid->addWidget(new QLabel("📅 This Week:"), 1, 0);
    periodGrid->addWidget(m_weekLabel, 1, 1);

    periodGrid->addWidget(new QLabel("📅 This Month:"), 2, 0);
    periodGrid->addWidget(m_monthLabel, 2, 1);

    layout->addWidget(titleLabel);
    layout->addLayout(statsGrid);
    layout->addSpacing(20);
    layout->addWidget(periodTitle);
    layout->addLayout(periodGrid);
    layout->addStretch();
}

void StatisticsDialog::setupChartTab()
{
    m_chartTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_chartTab);

    // Chart period buttons
    QHBoxLayout *periodLayout = new QHBoxLayout();
    m_chartPeriodLabel = new QLabel("📊 Work Time Chart");
    QFont chartFont = m_chartPeriodLabel->font();
    chartFont.setPointSize(14);
    chartFont.setBold(true);
    m_chartPeriodLabel->setFont(chartFont);

    m_weekButton = new QPushButton("Last 7 Days");
    m_monthButton = new QPushButton("Last 30 Days");
    m_yearButton = new QPushButton("Last Year");

    m_weekButton->setCheckable(true);
    m_monthButton->setCheckable(true);
    m_yearButton->setCheckable(true);
    m_weekButton->setChecked(true);

    periodLayout->addWidget(m_chartPeriodLabel);
    periodLayout->addStretch();
    periodLayout->addWidget(m_weekButton);
    periodLayout->addWidget(m_monthButton);
    periodLayout->addWidget(m_yearButton);

    // Chart widget
    m_chart = new StatisticsChart();

    layout->addLayout(periodLayout);
    layout->addWidget(m_chart);
}

void StatisticsDialog::setupDetailsTab()
{
    m_detailsTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_detailsTab);

    QLabel *detailsTitle = new QLabel("📋 Detailed Statistics");
    QFont detailsFont = detailsTitle->font();
    detailsFont.setPointSize(14);
    detailsFont.setBold(true);
    detailsTitle->setFont(detailsFont);

    m_detailsText = new QTextEdit();
    m_detailsText->setReadOnly(true);
    m_detailsText->setFont(QFont("Courier", 10));

    layout->addWidget(detailsTitle);
    layout->addWidget(m_detailsText);
}

void StatisticsDialog::setStatistics(int totalSessions, int totalWorkTime, int totalBreakTime)
{
    m_totalSessions = totalSessions;
    m_totalWorkTime = totalWorkTime;
    m_totalBreakTime = totalBreakTime;

    updateOverview();
    updateChart();

    // Update details tab
    QString details = QString(
        "DETAILED POMODORO STATISTICS\n"
        "=============================\n\n"
        "Overall Performance:\n"
        "- Total Sessions Completed: %1\n"
        "- Total Time Spent: %2 hours %3 minutes\n"
        "- Work Time: %4 hours %5 minutes\n"
        "- Break Time: %6 hours %7 minutes\n\n"
        "Productivity Metrics:\n"
        "- Average Session Length: %8 minutes\n"
        "- Work/Break Ratio: %9\n"
        "- Sessions per Day (avg): %10\n\n"
        "Recent Activity:\n"
        "- Today: %11 minutes\n"
        "- Yesterday: %12 minutes\n"
        "- This Week: %13 minutes\n"
        "- Last Week: %14 minutes\n"
    ).arg(m_totalSessions)
     .arg(m_totalWorkTime / 3600).arg((m_totalWorkTime % 3600) / 60)
     .arg(m_totalWorkTime / 3600).arg((m_totalWorkTime % 3600) / 60)
     .arg(m_totalBreakTime / 3600).arg((m_totalBreakTime % 3600) / 60)
     .arg(m_totalSessions > 0 ? m_totalWorkTime / (m_totalSessions * 60) : 0)
     .arg(m_totalBreakTime > 0 ? QString::number(double(m_totalWorkTime) / m_totalBreakTime, 'f', 2) : "N/A")
     .arg(m_totalSessions > 0 ? QString::number(double(m_totalSessions) / 7, 'f', 1) : "0")
     .arg(m_dailyWorkTime.value(QDate::currentDate(), 0))
     .arg(m_dailyWorkTime.value(QDate::currentDate().addDays(-1), 0))
     .arg(m_dailyWorkTime.value(QDate::currentDate(), 0)) // This week calculation can be improved
     .arg(0); // Last week calculation can be improved

    m_detailsText->setPlainText(details);
}

void StatisticsDialog::loadDailyStatistics()
{
    QSettings settings;

    // Load daily work time data for the last 30 days
    QDate today = QDate::currentDate();
    for (int i = 0; i < 30; ++i) {
        QDate date = today.addDays(-i);
        QSettings settings;
        QVariantMap dailyStats = settings.value("dailyStats").toMap();

        for (auto it = dailyStats.begin(); it != dailyStats.end(); ++it) {
            QDate date = QDate::fromString(it.key(), "yyyy-MM-dd");
            if (date.isValid()) {
                QVariantMap dayData = it.value().toMap();
                m_dailyWorkTime[date] = dayData["workTime"].toInt();
                m_dailySessions[date] = dayData["sessions"].toInt();
            }
        }

        QString sessionKey = QString("dailyStats/sessions_%1").arg(date.toString("yyyy-MM-dd"));
        int sessions = settings.value(sessionKey, 0).toInt();
        if (sessions > 0) {
            m_dailySessions[date] = sessions;
        }
    }
}

void StatisticsDialog::updateOverview()
{
    m_totalSessionsLabel->setText(QString::number(m_totalSessions));

    // Правильное форматирование времени
    m_totalTimeLabel->setText(TimerStateHelper::formatDuration(m_totalWorkTime + m_totalBreakTime));
    m_workTimeLabel->setText(TimerStateHelper::formatDuration(m_totalWorkTime));
    m_breakTimeLabel->setText(TimerStateHelper::formatDuration(m_totalBreakTime)); // исправлено

    int avgSession = m_totalSessions > 0 ? (m_totalWorkTime / m_totalSessions) : 0;
    m_avgSessionLabel->setText(TimerStateHelper::formatDuration(avgSession));

    int totalMinutes = (m_totalWorkTime + m_totalBreakTime) / 60;
    int workMinutes = m_totalWorkTime / 60;
    int efficiency = totalMinutes > 0 ? (workMinutes * 100 / totalMinutes) : 0;
    m_efficiencyLabel->setText(QString("%1%").arg(efficiency));

    // Period statistics
    QDate today = QDate::currentDate();
    int todayMinutes = m_dailyWorkTime.value(today, 0);
    m_todayLabel->setText(TimerStateHelper::formatDuration(todayMinutes * 60));

    // Calculate week total
    int weekTotal = 0;
    for (int i = 0; i < 7; ++i) {
        weekTotal += m_dailyWorkTime.value(today.addDays(-i), 0);
    }
    m_weekLabel->setText(TimerStateHelper::formatDuration(weekTotal * 60));

    // Calculate month total
    int monthTotal = 0;
    for (int i = 0; i < 30; ++i) {
        monthTotal += m_dailyWorkTime.value(today.addDays(-i), 0);
    }
    m_monthLabel->setText(TimerStateHelper::formatDuration(monthTotal * 60));
}

void StatisticsDialog::updateChart()
{
    QMap<QDate, int> chartData;
    QDate today = QDate::currentDate();

    int days = 7;
    if (m_chartPeriod == "month") days = 30;
    else if (m_chartPeriod == "year") days = 365;

    for (int i = days - 1; i >= 0; --i) {
        QDate date = today.addDays(-i);
        chartData[date] = m_dailyWorkTime.value(date, 0);
    }

    m_chart->setData(chartData);

    // Update button states
    m_weekButton->setChecked(m_chartPeriod == "week");
    m_monthButton->setChecked(m_chartPeriod == "month");
    m_yearButton->setChecked(m_chartPeriod == "year");
}

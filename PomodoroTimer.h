#ifndef POMODOROTIMER_H
#define POMODOROTIMER_H

#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QDateTime>
#include <memory>
#include "TimerState.h"

// Forward declarations
class CircularProgressBar;
class SettingsDialog;
class StatisticsDialog;
class SystemTrayManager;
class KeyboardShortcuts;
class NotificationManager;

class PomodoroTimer : public QWidget
{
    Q_OBJECT

public:
    explicit PomodoroTimer(QWidget *parent = nullptr);
    ~PomodoroTimer() override;

    // Timer configuration constants
    static constexpr int DEFAULT_WORK_DURATION = 1500;      // 25 minutes in seconds
    static constexpr int DEFAULT_SHORT_BREAK = 300;         // 5 minutes in seconds
    static constexpr int DEFAULT_LONG_BREAK = 900;          // 15 minutes in seconds
    static constexpr int SESSIONS_BEFORE_LONG_BREAK = 4;
    static constexpr int TIMER_INTERVAL_MS = 1000;
    static constexpr int AUTO_START_DELAY_MS = 3000;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    // Timer control slots
    void onStartTimer();
    void onPauseTimer();
    void onResetTimer();
    void onUpdateTimer();
    void onTimerFinished();

    // UI interaction slots
    void onShowSettings();
    void onSkipSession();
    void onShowStatistics();
    void onToggleVisibility();

private:
    // Setup methods
    void setupUI();
    void setupConnections();

    // UI creation helpers
    void createLabels();
    void createButtons();
    void createLayouts();
    void applyStyles() const;

    // Settings management
    void loadSettings();
    void saveSettings() const;

    // Timer state management
    void resetTimerState();
    void updateTimerState(TimerState newState);

    // Display update methods - optimized to avoid unnecessary updates
    void updateDisplay();
    void updateSessionCounter() const;
    void updateButtonStates() const;
    void updateWindowTitle();
    static void updateTrayTooltip();

    // Utility methods
    static QString formatTime(int seconds);
    static void needsDisplayUpdate();

    // UI elements - use raw pointers for Qt objects with parent ownership
    QTimer *m_timer;
    QLabel *m_timeLabel{nullptr};
    QLabel *m_statusLabel{nullptr};
    QLabel *m_sessionLabel{nullptr};
    QPushButton *m_startButton{nullptr};
    QPushButton *m_pauseButton{nullptr};
    QPushButton *m_resetButton{nullptr};
    QPushButton *m_settingsButton{nullptr};
    QPushButton *m_skipButton{nullptr};
    QPushButton *m_statsButton{nullptr};
    CircularProgressBar *m_circularProgress{nullptr};
    QFrame *m_mainFrame{nullptr};

    // Managers - keep as unique_ptr for complex objects
    std::unique_ptr<SystemTrayManager> m_trayManager;
    std::unique_ptr<KeyboardShortcuts> m_keyboardShortcuts;
    std::unique_ptr<NotificationManager> m_notificationManager;

    // Timer state
    int m_currentTime{0};
    int m_totalDuration{0};
    TimerState m_currentState{TimerState::Work};
    int m_completedSessions{0};
    bool m_isRunning{false};

    // Cache for reducing string operations
    mutable QString m_cachedTimeString;
    mutable int m_lastFormattedTime{-1};

    // Settings
    int m_workDuration{DEFAULT_WORK_DURATION};
    int m_shortBreakDuration{DEFAULT_SHORT_BREAK};
    int m_longBreakDuration{DEFAULT_LONG_BREAK};
    bool m_autoStartBreaks{false};
    bool m_autoStartWork{false};
    bool m_minimizeToTray{true};
    bool m_showNotifications{true};

    // Statistics
    int m_totalSessions{0};
    int m_totalWorkTime{0};
    int m_totalBreakTime{0};
    QDateTime m_sessionStartTime;

    // Update tracking to prevent unnecessary redraws
    bool m_needsButtonUpdate{true};
    bool m_needsSessionUpdate{true};
    QString m_lastWindowTitle;
};

#endif // POMODOROTIMER_H

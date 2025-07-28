#include "PomodoroTimer.h"
#include "CircularProgressBar.h"
#include "SettingsDialog.h"
#include "StatisticsDialog.h"
#include "SystemTrayManager.h"
#include "KeyboardShortcuts.h"
#include "NotificationManager.h"
#include "TimerState.h"

#include <QApplication>
#include <QDateTime>
#include <QFont>
#include <QKeyEvent>
#include <QSettings>
#include <QVBoxLayout>

namespace {
    // UI Layout constants
    constexpr int WINDOW_WIDTH = 440;
    constexpr int WINDOW_HEIGHT = 550;
    constexpr int PROGRESS_BAR_SIZE = 220;
    constexpr int MAIN_BUTTON_WIDTH = 100;
    constexpr int MAIN_BUTTON_HEIGHT = 40;
    constexpr int SMALL_BUTTON_WIDTH = 90;
    constexpr int SMALL_BUTTON_HEIGHT = 35;
    constexpr int LAYOUT_SPACING = 10;
    constexpr int LAYOUT_MARGIN = 25;

    // Font sizes
    constexpr int TIME_FONT_SIZE = 24;
    constexpr int STATUS_FONT_SIZE = 14;
    constexpr int SESSION_FONT_SIZE = 12;
    constexpr int BUTTON_FONT_SIZE = 11;

    // Progress bar positioning
    constexpr int TIME_LABEL_Y_OFFSET = 90;
}

PomodoroTimer::PomodoroTimer(QWidget *parent)
    : QWidget(parent)
    , m_timer(new QTimer(this))
    , m_trayManager(std::make_unique<SystemTrayManager>(this))
    , m_keyboardShortcuts(std::make_unique<KeyboardShortcuts>(this))
    , m_notificationManager(std::make_unique<NotificationManager>(this))
{
    loadSettings();
    setupUI();
    setupConnections();
    resetTimerState();
    updateDisplay();

    // Setup manager connections
    m_notificationManager->setSystemTrayManager(m_trayManager.get());
    m_notificationManager->setNotificationsEnabled(m_showNotifications);
    m_trayManager->show();
}

PomodoroTimer::~PomodoroTimer()
{
    saveSettings();
}

void PomodoroTimer::setupUI()
{
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    m_mainFrame = new QFrame(this);
    m_circularProgress = new CircularProgressBar(m_mainFrame);
    m_circularProgress->setFixedSize(PROGRESS_BAR_SIZE, PROGRESS_BAR_SIZE);

    createLabels();
    createButtons();
    createLayouts();
    applyStyles();
    updateButtonStates();
}

void PomodoroTimer::createLabels()
{
    // Time display label
    m_timeLabel = new QLabel("25:00", m_circularProgress);
    m_timeLabel->setAlignment(Qt::AlignCenter);
    m_timeLabel->resize(PROGRESS_BAR_SIZE, 40);
    m_timeLabel->move(0, TIME_LABEL_Y_OFFSET);

    // Status indicator label
    m_statusLabel = new QLabel("Ready to Focus", m_mainFrame);
    m_statusLabel->setAlignment(Qt::AlignCenter);

    // Session progress label
    m_sessionLabel = new QLabel("Session 1 of 4", m_mainFrame);
    m_sessionLabel->setAlignment(Qt::AlignCenter);
}

void PomodoroTimer::createButtons()
{
    m_startButton = new QPushButton("Start", m_mainFrame);
    m_pauseButton = new QPushButton("Pause", m_mainFrame);
    m_resetButton = new QPushButton("Reset", m_mainFrame);
    m_settingsButton = new QPushButton("Settings", m_mainFrame);
    m_skipButton = new QPushButton("Skip", m_mainFrame);
    m_statsButton = new QPushButton("Statistics", m_mainFrame);

    // Set button sizes
    constexpr QSize mainButtonSize(MAIN_BUTTON_WIDTH, MAIN_BUTTON_HEIGHT);
    constexpr QSize smallButtonSize(SMALL_BUTTON_WIDTH, SMALL_BUTTON_HEIGHT);

    m_startButton->setFixedSize(mainButtonSize);
    m_pauseButton->setFixedSize(mainButtonSize);
    m_resetButton->setFixedSize(mainButtonSize);
    m_settingsButton->setFixedSize(mainButtonSize);
    m_skipButton->setFixedSize(smallButtonSize);
    m_statsButton->setFixedSize(smallButtonSize);
}

void PomodoroTimer::createLayouts()
{
    // Progress bar layout
    auto progressLayout = new QVBoxLayout();
    progressLayout->addWidget(m_circularProgress, 0, Qt::AlignCenter);

    // Main control buttons layout
    auto buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(LAYOUT_SPACING);
    buttonLayout->addWidget(m_startButton);
    buttonLayout->addWidget(m_pauseButton);
    buttonLayout->addWidget(m_resetButton);

    // Header layout with settings button
    auto headerLayout = new QHBoxLayout();
    headerLayout->addStretch();
    headerLayout->addWidget(m_settingsButton);

    // Extra buttons layout
    auto extraButtonLayout = new QHBoxLayout();
    extraButtonLayout->setSpacing(LAYOUT_SPACING);
    extraButtonLayout->addStretch();
    extraButtonLayout->addWidget(m_skipButton);
    extraButtonLayout->addWidget(m_statsButton);
    extraButtonLayout->addStretch();

    // Main layout assembly
    auto mainLayout = new QVBoxLayout();
    mainLayout->addLayout(headerLayout);
    mainLayout->addSpacing(5);
    mainLayout->addWidget(m_statusLabel);
    mainLayout->addSpacing(15);
    mainLayout->addLayout(progressLayout);
    mainLayout->addSpacing(15);
    mainLayout->addWidget(m_sessionLabel);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addSpacing(15);
    mainLayout->addLayout(extraButtonLayout);
    mainLayout->setContentsMargins(LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN);

    m_mainFrame->setLayout(mainLayout);

    // Frame layout
    auto frameLayout = new QVBoxLayout();
    frameLayout->addWidget(m_mainFrame);
    frameLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(frameLayout);
}

void PomodoroTimer::applyStyles() const {
    QFont timeFont = m_timeLabel->font();
    timeFont.setPointSize(TIME_FONT_SIZE);
    timeFont.setBold(true);
    m_timeLabel->setFont(timeFont);

    QFont statusFont = m_statusLabel->font();
    statusFont.setPointSize(STATUS_FONT_SIZE);
    m_statusLabel->setFont(statusFont);

    QFont sessionFont = m_sessionLabel->font();
    sessionFont.setPointSize(SESSION_FONT_SIZE);
    m_sessionLabel->setFont(sessionFont);

    QFont buttonFont;
    buttonFont.setPointSize(BUTTON_FONT_SIZE);

    const auto buttons = {m_startButton, m_pauseButton, m_resetButton,
                         m_settingsButton, m_skipButton, m_statsButton};
    for (auto* button : buttons) {
        if (button) button->setFont(buttonFont);
    }
}

void PomodoroTimer::setupConnections()
{
    // Timer connections
    connect(m_timer, &QTimer::timeout, this, &PomodoroTimer::onUpdateTimer);

    // Button connections
    connect(m_startButton, &QPushButton::clicked, this, &PomodoroTimer::onStartTimer);
    connect(m_pauseButton, &QPushButton::clicked, this, &PomodoroTimer::onPauseTimer);
    connect(m_resetButton, &QPushButton::clicked, this, &PomodoroTimer::onResetTimer);
    connect(m_settingsButton, &QPushButton::clicked, this, &PomodoroTimer::onShowSettings);
    connect(m_skipButton, &QPushButton::clicked, this, &PomodoroTimer::onSkipSession);
    connect(m_statsButton, &QPushButton::clicked, this, &PomodoroTimer::onShowStatistics);

    // System tray connections
    connect(m_trayManager.get(), &SystemTrayManager::showMainWindow, this, &PomodoroTimer::onToggleVisibility);
    connect(m_trayManager.get(), &SystemTrayManager::startPauseRequested, this, [this]() {
        m_isRunning ? onPauseTimer() : onStartTimer();
    });
    connect(m_trayManager.get(), &SystemTrayManager::resetRequested, this, &PomodoroTimer::onResetTimer);
    connect(m_trayManager.get(), &SystemTrayManager::skipRequested, this, &PomodoroTimer::onSkipSession);
    connect(m_trayManager.get(), &SystemTrayManager::settingsRequested, this, &PomodoroTimer::onShowSettings);
    connect(m_trayManager.get(), &SystemTrayManager::statisticsRequested, this, &PomodoroTimer::onShowStatistics);
    connect(m_trayManager.get(), &SystemTrayManager::quitRequested, qApp, &QCoreApplication::quit);

    // Keyboard shortcuts connections
    connect(m_keyboardShortcuts.get(), &KeyboardShortcuts::startPauseRequested, this, [this]() {
        m_isRunning ? onPauseTimer() : onStartTimer();
    });
    connect(m_keyboardShortcuts.get(), &KeyboardShortcuts::pauseRequested, this, &PomodoroTimer::onPauseTimer);
    connect(m_keyboardShortcuts.get(), &KeyboardShortcuts::resetRequested, this, &PomodoroTimer::onResetTimer);
    connect(m_keyboardShortcuts.get(), &KeyboardShortcuts::settingsRequested, this, &PomodoroTimer::onShowSettings);
    connect(m_keyboardShortcuts.get(), &KeyboardShortcuts::skipRequested, this, &PomodoroTimer::onSkipSession);
}

// Timer control methods
void PomodoroTimer::onStartTimer()
{
    if (m_isRunning) return;

    m_isRunning = true;
    m_sessionStartTime = QDateTime::currentDateTime();
    m_timer->start(TIMER_INTERVAL_MS);

    m_statusLabel->setText(TimerStateHelper::getStatusMessage(m_currentState, true));
    updateButtonStates();
    updateDisplay();
}

void PomodoroTimer::onPauseTimer()
{
    if (!m_isRunning) return;

    m_timer->stop();
    m_isRunning = false;
    m_statusLabel->setText("⏸ Paused");
    updateButtonStates();
    updateDisplay();
}

void PomodoroTimer::onResetTimer()
{
    m_timer->stop();
    m_isRunning = false;
    resetTimerState();
    updateDisplay();
    updateButtonStates();
}

void PomodoroTimer::onUpdateTimer()
{
    --m_currentTime;
    updateDisplay();

    if (m_currentTime <= 0) {
        onTimerFinished();
    }
}

void PomodoroTimer::onTimerFinished()
{
    m_timer->stop();
    m_isRunning = false;

    // Update statistics
    const QDateTime now = QDateTime::currentDateTime();
    const int sessionDuration = static_cast<int>(m_sessionStartTime.secsTo(now));

    if (m_currentState == TimerState::Work) {
        m_totalWorkTime += sessionDuration;
        m_totalSessions++;
        m_completedSessions++;
    } else {
        m_totalBreakTime += sessionDuration;
    }

    // Determine next state and show notifications
    QString message, nextAction;
    if (m_currentState == TimerState::Work) {
        message = "🎉 Focus session completed!";

        const bool isLongBreakTime = (m_completedSessions % SESSIONS_BEFORE_LONG_BREAK == 0);
        updateTimerState(isLongBreakTime ? TimerState::LongBreak : TimerState::ShortBreak);
        nextAction = isLongBreakTime ? "Time for a long break! 🧘‍♀️" : "Take a short break! ☕";
    } else {
        message = "Break time finished!";
        nextAction = "Ready to focus again? 🎯";
        updateTimerState(TimerState::Work);
    }

    m_notificationManager->showNotification(message + " " + nextAction);
    onResetTimer();

    // Auto-start if enabled
    const bool shouldAutoStart = (m_currentState != TimerState::Work && m_autoStartBreaks) ||
                                (m_currentState == TimerState::Work && m_autoStartWork);
    if (shouldAutoStart) {
        QTimer::singleShot(AUTO_START_DELAY_MS, this, &PomodoroTimer::onStartTimer);
    }

    saveSettings();
}

// Helper methods
void PomodoroTimer::resetTimerState()
{
    m_currentTime = TimerStateHelper::getDurationForState(m_currentState, m_workDuration, m_shortBreakDuration, m_longBreakDuration);
    m_totalDuration = m_currentTime;
    m_statusLabel->setText(TimerStateHelper::getStatusMessage(m_currentState, false));
}

void PomodoroTimer::updateTimerState(TimerState newState)
{
    m_currentState = newState;
}

QString PomodoroTimer::formatTime(int seconds) {
  const int minutes = seconds / 60;
  const int secs = seconds % 60;
  return QString("%1:%2")
      .arg(minutes, 2, 10, QChar('0'))
      .arg(secs, 2, 10, QChar('0'));
}
void PomodoroTimer::needsDisplayUpdate() {}

void PomodoroTimer::updateDisplay()
{
    m_timeLabel->setText(formatTime(m_currentTime));

    const int progress = m_totalDuration > 0 ?
        ((m_totalDuration - m_currentTime) * 100) / m_totalDuration : 0;
    m_circularProgress->setValue(progress);

    updateSessionCounter();
    updateWindowTitle();

    // Update system tray
    const int currentSession = (m_completedSessions % SESSIONS_BEFORE_LONG_BREAK) + 1;
    m_trayManager->updateTooltip(m_currentState, m_isRunning, formatTime(m_currentTime),
                                currentSession, SESSIONS_BEFORE_LONG_BREAK);
}

void PomodoroTimer::updateSessionCounter() const {
    const int currentSession = (m_completedSessions % SESSIONS_BEFORE_LONG_BREAK) + 1;
    m_sessionLabel->setText(QString("Session %1 of %2")
                           .arg(currentSession)
                           .arg(SESSIONS_BEFORE_LONG_BREAK));
}

void PomodoroTimer::updateButtonStates() const {
    m_startButton->setEnabled(!m_isRunning);
    m_pauseButton->setEnabled(m_isRunning);

    const bool shouldShowReset = m_isRunning || m_currentTime < m_totalDuration;
    m_resetButton->setVisible(shouldShowReset);
    m_resetButton->setEnabled(true);

    m_skipButton->setEnabled(m_isRunning);
    m_skipButton->setVisible(m_isRunning);

    // Force layout update
    if (m_mainFrame && m_mainFrame->layout()) {
        m_mainFrame->layout()->update();
        m_mainFrame->updateGeometry();
    }
}

void PomodoroTimer::updateWindowTitle() {
  const QString timeRemaining = formatTime(m_currentTime);
  const QString emoji = TimerStateHelper::getStateEmoji(m_currentState);

  QString title;
  if (m_isRunning) {
    const QString stateText =
        (m_currentState == TimerState::Work) ? "Focus" : "Break";
    title = QString("%1 %2: %3").arg(emoji, stateText, timeRemaining);
  } else {
    title = "🍅 Pomodoro Timer";
  }

  setWindowTitle(title);
}
void PomodoroTimer::updateTrayTooltip() {}

void PomodoroTimer::onShowSettings()
{
    SettingsDialog dialog(this);

    dialog.setWorkDuration(m_workDuration / 60);
    dialog.setShortBreakDuration(m_shortBreakDuration / 60);
    dialog.setLongBreakDuration(m_longBreakDuration / 60);
    dialog.setAutoStartBreaks(m_autoStartBreaks);
    dialog.setAutoStartWork(m_autoStartWork);
    dialog.setMinimizeToTray(m_minimizeToTray);
    dialog.setShowNotifications(m_showNotifications);

    if (dialog.exec() == QDialog::Accepted) {
        m_workDuration = dialog.workDuration() * 60;
        m_shortBreakDuration = dialog.shortBreakDuration() * 60;
        m_longBreakDuration = dialog.longBreakDuration() * 60;
        m_autoStartBreaks = dialog.autoStartBreaks();
        m_autoStartWork = dialog.autoStartWork();
        m_minimizeToTray = dialog.minimizeToTray();
        m_showNotifications = dialog.showNotifications();

        saveSettings();
        onResetTimer();
    }
}

void PomodoroTimer::onSkipSession()
{
    if (m_isRunning) {
        onTimerFinished();
    }
}

void PomodoroTimer::onShowStatistics()
{
    StatisticsDialog dialog(this);
    dialog.setStatistics(m_totalSessions, m_totalWorkTime, m_totalBreakTime);
    dialog.exec();
}

void PomodoroTimer::onToggleVisibility()
{
    if (isVisible()) {
        hide();
    } else {
        show();
        raise();
        activateWindow();
    }
}

void PomodoroTimer::loadSettings()
{
    QSettings settings;
    m_workDuration = settings.value("workDuration", DEFAULT_WORK_DURATION).toInt();
    m_shortBreakDuration = settings.value("shortBreakDuration", DEFAULT_SHORT_BREAK).toInt();
    m_longBreakDuration = settings.value("longBreakDuration", DEFAULT_LONG_BREAK).toInt();
    m_autoStartBreaks = settings.value("autoStartBreaks", false).toBool();
    m_autoStartWork = settings.value("autoStartWork", false).toBool();
    m_minimizeToTray = settings.value("minimizeToTray", true).toBool();
    m_showNotifications = settings.value("showNotifications", true).toBool();
    m_totalSessions = settings.value("totalSessions", 0).toInt();
    m_totalWorkTime = settings.value("totalWorkTime", 0).toInt();
    m_totalBreakTime = settings.value("totalBreakTime", 0).toInt();
}

void PomodoroTimer::saveSettings() const {
    QSettings settings;
    settings.setValue("workDuration", m_workDuration);
    settings.setValue("shortBreakDuration", m_shortBreakDuration);
    settings.setValue("longBreakDuration", m_longBreakDuration);
    settings.setValue("autoStartBreaks", m_autoStartBreaks);
    settings.setValue("autoStartWork", m_autoStartWork);
    settings.setValue("minimizeToTray", m_minimizeToTray);
    settings.setValue("showNotifications", m_showNotifications);
    settings.setValue("totalSessions", m_totalSessions);
    settings.setValue("totalWorkTime", m_totalWorkTime);
    settings.setValue("totalBreakTime", m_totalBreakTime);
}

void PomodoroTimer::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        hide();
    }
    QWidget::keyPressEvent(event);
}

void PomodoroTimer::closeEvent(QCloseEvent *event)
{
    if (m_trayManager && m_trayManager->isVisible()) {
        hide();
        event->ignore();

        static bool firstHide = true;
        if (firstHide) {
            m_notificationManager->showNotification("Application was minimized to tray. Click the tray icon to show again.");
            firstHide = false;
        }
    } else {
        saveSettings();
        event->accept();
    }
}

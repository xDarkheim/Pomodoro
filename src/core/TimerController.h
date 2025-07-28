#ifndef TIMERCONTROLLER_H
#define TIMERCONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QDateTime>
#include <memory>

enum class SessionType {
    Work,
    ShortBreak,
    LongBreak
};

enum class TimerStatus {
    Stopped,
    Running,
    Paused
};

class TimerController : public QObject
{
    Q_OBJECT

public:
    explicit TimerController(QObject *parent = nullptr);
    ~TimerController() override = default;

    // Timer control
    void start();
    void pause();
    void reset();
    void skip();

    // Getters
    [[nodiscard]] int remainingSeconds() const { return m_remainingSeconds; }
    [[nodiscard]] int totalSeconds() const { return m_totalSeconds; }
    [[nodiscard]] SessionType currentSessionType() const { return m_currentSessionType; }
    [[nodiscard]] TimerStatus state() const { return m_state; }
    [[nodiscard]] int completedSessions() const { return m_completedSessions; }
    [[nodiscard]] double progressPercentage() const;

    // Configuration
    void setWorkDuration(int seconds) { m_workDuration = seconds; }
    void setShortBreakDuration(int seconds) { m_shortBreakDuration = seconds; }
    void setLongBreakDuration(int seconds) { m_longBreakDuration = seconds; }

signals:
    void timeChanged(int remainingSeconds);
    void sessionChanged(SessionType type);
    void timerFinished();
    void stateChanged(TimerStatus state);

private slots:
    void onTimerTick();

private:
    void startNextSession();
    void updateTotalSeconds();

    // Timer objects
    std::unique_ptr<QTimer> m_timer;

    // State
    TimerStatus m_state = TimerStatus::Stopped;
    SessionType m_currentSessionType = SessionType::Work;

    // Time tracking
    int m_remainingSeconds = 0;
    int m_totalSeconds = 0;
    int m_completedSessions = 0;

    // Configuration
    int m_workDuration = 1500;      // 25 minutes
    int m_shortBreakDuration = 300; // 5 minutes
    int m_longBreakDuration = 900;  // 15 minutes

    static constexpr int SESSIONS_BEFORE_LONG_BREAK = 4;
};

#endif // TIMERCONTROLLER_H

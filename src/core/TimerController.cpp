#include "TimerController.h"
#include <QDebug>

TimerController::TimerController(QObject *parent)
    : QObject(parent)
    , m_timer(std::make_unique<QTimer>(this))
{
    connect(m_timer.get(), &QTimer::timeout, this, &TimerController::onTimerTick);
    m_timer->setInterval(1000); // 1 second
    reset();
}

void TimerController::start()
{
    if (m_state == TimerStatus::Stopped) {
        updateTotalSeconds();
        m_remainingSeconds = m_totalSeconds;
    }

    m_state = TimerStatus::Running;
    m_timer->start();
    emit stateChanged(m_state);
}

void TimerController::pause()
{
    if (m_state == TimerStatus::Running) {
        m_state = TimerStatus::Paused;
        m_timer->stop();
        emit stateChanged(m_state);
    }
}

void TimerController::reset()
{
    m_timer->stop();
    m_state = TimerStatus::Stopped;
    m_currentSessionType = SessionType::Work;
    updateTotalSeconds();
    m_remainingSeconds = m_totalSeconds;

    emit stateChanged(m_state);
    emit sessionChanged(m_currentSessionType);
    emit timeChanged(m_remainingSeconds);
}

void TimerController::skip()
{
    m_timer->stop();
    startNextSession();
}

double TimerController::progressPercentage() const
{
    if (m_totalSeconds == 0) return 0.0;
    return (static_cast<double>(m_totalSeconds - m_remainingSeconds) / m_totalSeconds) * 100.0;
}

void TimerController::onTimerTick()
{
    if (m_remainingSeconds > 0) {
        --m_remainingSeconds;
        emit timeChanged(m_remainingSeconds);
    } else {
        m_timer->stop();
        emit timerFinished();
        startNextSession();
    }
}

void TimerController::startNextSession()
{
    switch (m_currentSessionType) {
        case SessionType::Work:
            ++m_completedSessions;
            if (m_completedSessions % SESSIONS_BEFORE_LONG_BREAK == 0) {
                m_currentSessionType = SessionType::LongBreak;
            } else {
                m_currentSessionType = SessionType::ShortBreak;
            }
            break;

        case SessionType::ShortBreak:
        case SessionType::LongBreak:
            m_currentSessionType = SessionType::Work;
            break;
    }

    m_state = TimerStatus::Stopped;
    updateTotalSeconds();
    m_remainingSeconds = m_totalSeconds;

    emit sessionChanged(m_currentSessionType);
    emit stateChanged(m_state);
    emit timeChanged(m_remainingSeconds);
}

void TimerController::updateTotalSeconds()
{
    switch (m_currentSessionType) {
        case SessionType::Work:
            m_totalSeconds = m_workDuration;
            break;
        case SessionType::ShortBreak:
            m_totalSeconds = m_shortBreakDuration;
            break;
        case SessionType::LongBreak:
            m_totalSeconds = m_longBreakDuration;
            break;
    }
}

#ifndef POMODOROCONFIG_H
#define POMODOROCONFIG_H

#include <QSettings>
#include <QString>
#include <memory>

class PomodoroConfig
{
public:
    static PomodoroConfig& instance();

    // Timer durations (in seconds)
    int workDuration() const { return m_workDuration; }
    int shortBreakDuration() const { return m_shortBreakDuration; }
    int longBreakDuration() const { return m_longBreakDuration; }

    void setWorkDuration(int seconds);
    void setShortBreakDuration(int seconds);
    void setLongBreakDuration(int seconds);

    // UI settings
    bool showNotifications() const { return m_showNotifications; }
    void setShowNotifications(bool enabled);

    bool autoStartBreaks() const { return m_autoStartBreaks; }
    void setAutoStartBreaks(bool enabled);

    // Sound settings
    QString workEndSound() const { return m_workEndSound; }
    QString breakEndSound() const { return m_breakEndSound; }
    void setWorkEndSound(const QString& soundPath);
    void setBreakEndSound(const QString& soundPath);

    // Constants
    static constexpr int DEFAULT_WORK_DURATION = 1500;      // 25 minutes
    static constexpr int DEFAULT_SHORT_BREAK = 300;         // 5 minutes
    static constexpr int DEFAULT_LONG_BREAK = 900;          // 15 minutes
    static constexpr int SESSIONS_BEFORE_LONG_BREAK = 4;

    void saveSettings();
    void loadSettings();

private:
    PomodoroConfig();
    ~PomodoroConfig() = default;

    // Disable copy/move
    PomodoroConfig(const PomodoroConfig&) = delete;
    PomodoroConfig& operator=(const PomodoroConfig&) = delete;
    PomodoroConfig(PomodoroConfig&&) = delete;
    PomodoroConfig& operator=(PomodoroConfig&&) = delete;

    std::unique_ptr<QSettings> m_settings;

    // Timer settings
    int m_workDuration = DEFAULT_WORK_DURATION;
    int m_shortBreakDuration = DEFAULT_SHORT_BREAK;
    int m_longBreakDuration = DEFAULT_LONG_BREAK;

    // UI settings
    bool m_showNotifications = true;
    bool m_autoStartBreaks = false;

    // Sound settings
    QString m_workEndSound;
    QString m_breakEndSound;
};

#endif // POMODOROCONFIG_H

#include "PomodoroConfig.h"
#include <QStandardPaths>
#include <QDir>

PomodoroConfig& PomodoroConfig::instance()
{
    static PomodoroConfig config;
    return config;
}

PomodoroConfig::PomodoroConfig()
{
    const QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    const QString appConfigDir = configPath + "/PomodoroTimer";

    QDir dir;
    dir.mkpath(appConfigDir);

    m_settings = std::make_unique<QSettings>(
        appConfigDir + "/config.ini",
        QSettings::IniFormat
    );

    loadSettings();
}

void PomodoroConfig::setWorkDuration(int seconds)
{
    if (seconds > 0 && seconds != m_workDuration) {
        m_workDuration = seconds;
        saveSettings();
    }
}

void PomodoroConfig::setShortBreakDuration(int seconds)
{
    if (seconds > 0 && seconds != m_shortBreakDuration) {
        m_shortBreakDuration = seconds;
        saveSettings();
    }
}

void PomodoroConfig::setLongBreakDuration(int seconds)
{
    if (seconds > 0 && seconds != m_longBreakDuration) {
        m_longBreakDuration = seconds;
        saveSettings();
    }
}

void PomodoroConfig::setShowNotifications(bool enabled)
{
    if (enabled != m_showNotifications) {
        m_showNotifications = enabled;
        saveSettings();
    }
}

void PomodoroConfig::setAutoStartBreaks(bool enabled)
{
    if (enabled != m_autoStartBreaks) {
        m_autoStartBreaks = enabled;
        saveSettings();
    }
}

void PomodoroConfig::setWorkEndSound(const QString& soundPath)
{
    if (soundPath != m_workEndSound) {
        m_workEndSound = soundPath;
        saveSettings();
    }
}

void PomodoroConfig::setBreakEndSound(const QString& soundPath)
{
    if (soundPath != m_breakEndSound) {
        m_breakEndSound = soundPath;
        saveSettings();
    }
}

void PomodoroConfig::saveSettings()
{
    if (!m_settings) return;

    m_settings->beginGroup("Timer");
    m_settings->setValue("workDuration", m_workDuration);
    m_settings->setValue("shortBreakDuration", m_shortBreakDuration);
    m_settings->setValue("longBreakDuration", m_longBreakDuration);
    m_settings->endGroup();

    m_settings->beginGroup("UI");
    m_settings->setValue("showNotifications", m_showNotifications);
    m_settings->setValue("autoStartBreaks", m_autoStartBreaks);
    m_settings->endGroup();

    m_settings->beginGroup("Sound");
    m_settings->setValue("workEndSound", m_workEndSound);
    m_settings->setValue("breakEndSound", m_breakEndSound);
    m_settings->endGroup();

    m_settings->sync();
}

void PomodoroConfig::loadSettings()
{
    if (!m_settings) return;

    m_settings->beginGroup("Timer");
    m_workDuration = m_settings->value("workDuration", DEFAULT_WORK_DURATION).toInt();
    m_shortBreakDuration = m_settings->value("shortBreakDuration", DEFAULT_SHORT_BREAK).toInt();
    m_longBreakDuration = m_settings->value("longBreakDuration", DEFAULT_LONG_BREAK).toInt();
    m_settings->endGroup();

    m_settings->beginGroup("UI");
    m_showNotifications = m_settings->value("showNotifications", true).toBool();
    m_autoStartBreaks = m_settings->value("autoStartBreaks", false).toBool();
    m_settings->endGroup();

    m_settings->beginGroup("Sound");
    m_workEndSound = m_settings->value("workEndSound", QString()).toString();
    m_breakEndSound = m_settings->value("breakEndSound", QString()).toString();
    m_settings->endGroup();
}

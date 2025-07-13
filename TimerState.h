#ifndef TIMERSTATE_H
#define TIMERSTATE_H

#include <QString>

enum class TimerState : quint8 {  // Use smaller enum type
    Work,
    ShortBreak,
    LongBreak
};

class TimerStateHelper
{
public:
    // Use constexpr and string literals for better performance
    static QString getStateText(TimerState state) noexcept {
        switch (state) {
            case TimerState::Work:
                return "Work";
            case TimerState::ShortBreak:
                return "Short Break";
            case TimerState::LongBreak:
                return "Long Break";
        }
        return "Unknown";
    }

    static QString getStateEmoji(TimerState state) noexcept {
        switch (state) {
            case TimerState::Work:
                return "👨‍💻";
            case TimerState::ShortBreak:
                return "☕️";
            case TimerState::LongBreak:
                return "😴";
        }
        return "❓";
    }

    static QString getStatusMessage(TimerState state, bool isRunning) noexcept {
        QString text = getStateText(state);
        if (!isRunning) {
            text += " (Paused)";
        }
        return text;
    }

    static int getDurationForState(TimerState state, int workDuration, int shortBreak, int longBreak) noexcept {
        switch (state) {
            case TimerState::Work:
                return workDuration;
            case TimerState::ShortBreak:
                return shortBreak;
            case TimerState::LongBreak:
                return longBreak;
        }
        return 0;
    }

    static QString formatDuration(int seconds) noexcept
    {
        const int hours = seconds / 3600;
        const int minutes = (seconds % 3600) / 60;
        if (hours > 0)
        {
            return QString("%1h %2m").arg(hours).arg(minutes);
        }
        return QString("%1m").arg(minutes);
    }
};

#endif // TIMERSTATE_H

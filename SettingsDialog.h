#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

// Forward declarations
class QSpinBox;
class QCheckBox;
class QPushButton;
class QDialogButtonBox;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    // Getters
    int workDuration() const;
    int shortBreakDuration() const;
    int longBreakDuration() const;
    bool autoStartBreaks() const;
    bool autoStartWork() const;
    bool minimizeToTray() const;
    bool showNotifications() const;

    // Setters
    void setWorkDuration(int minutes);
    void setShortBreakDuration(int minutes);
    void setLongBreakDuration(int minutes);
    void setAutoStartBreaks(bool enabled);
    void setAutoStartWork(bool enabled);
    void setMinimizeToTray(bool enabled);
    void setShowNotifications(bool enabled);

signals:
    void settingsChanged();

private slots:
    void resetToDefaults();

private:
    void setupUI();

    // UI elements
    QSpinBox *m_workDurationSpin;
    QSpinBox *m_shortBreakSpin;
    QSpinBox *m_longBreakSpin;
    QCheckBox *m_autoStartBreaksCheck;
    QCheckBox *m_autoStartWorkCheck;
    QCheckBox *m_minimizeToTrayCheck;
    QCheckBox *m_showNotificationsCheck;
    QDialogButtonBox *m_buttonBox;
};

#endif // SETTINGSDIALOG_H

#include "SettingsDialog.h"
#include <QApplication>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Pomodoro Settings");
    setFixedSize(350, 300);  // Reduced from 400 since we removed sound settings
    // Remove all custom styling
    setStyleSheet("");
    setupUI();
}

void SettingsDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Timer Settings Group
    QGroupBox *timerGroup = new QGroupBox("Timer Settings");
    QFormLayout *timerLayout = new QFormLayout(timerGroup);

    m_workDurationSpin = new QSpinBox();
    m_workDurationSpin->setRange(1, 60);
    m_workDurationSpin->setSuffix(" minutes");
    timerLayout->addRow("Work Duration:", m_workDurationSpin);

    m_shortBreakSpin = new QSpinBox();
    m_shortBreakSpin->setRange(1, 30);
    m_shortBreakSpin->setSuffix(" minutes");
    timerLayout->addRow("Short Break:", m_shortBreakSpin);

    m_longBreakSpin = new QSpinBox();
    m_longBreakSpin->setRange(5, 60);
    m_longBreakSpin->setSuffix(" minutes");
    timerLayout->addRow("Long Break:", m_longBreakSpin);

    // Behavior Settings Group
    QGroupBox *behaviorGroup = new QGroupBox("Behavior");
    QVBoxLayout *behaviorLayout = new QVBoxLayout(behaviorGroup);

    m_autoStartBreaksCheck = new QCheckBox("Auto-start breaks");
    m_autoStartWorkCheck = new QCheckBox("Auto-start work sessions");
    m_minimizeToTrayCheck = new QCheckBox("Minimize to system tray");
    m_showNotificationsCheck = new QCheckBox("Show notifications");

    behaviorLayout->addWidget(m_autoStartBreaksCheck);
    behaviorLayout->addWidget(m_autoStartWorkCheck);
    behaviorLayout->addWidget(m_minimizeToTrayCheck);
    behaviorLayout->addWidget(m_showNotificationsCheck);

    // Dialog Buttons
    m_buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::RestoreDefaults);

    // Layout
    mainLayout->addWidget(timerGroup);
    mainLayout->addWidget(behaviorGroup);
    mainLayout->addStretch();
    mainLayout->addWidget(m_buttonBox);

    // Connections
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_buttonBox->button(QDialogButtonBox::RestoreDefaults),
            &QPushButton::clicked, this, &SettingsDialog::resetToDefaults);
}

// Getters
int SettingsDialog::workDuration() const { return m_workDurationSpin->value(); }
int SettingsDialog::shortBreakDuration() const { return m_shortBreakSpin->value(); }
int SettingsDialog::longBreakDuration() const { return m_longBreakSpin->value(); }
bool SettingsDialog::autoStartBreaks() const { return m_autoStartBreaksCheck->isChecked(); }
bool SettingsDialog::autoStartWork() const { return m_autoStartWorkCheck->isChecked(); }
bool SettingsDialog::minimizeToTray() const { return m_minimizeToTrayCheck->isChecked(); }
bool SettingsDialog::showNotifications() const { return m_showNotificationsCheck->isChecked(); }

// Setters
void SettingsDialog::setWorkDuration(int minutes) { m_workDurationSpin->setValue(minutes); }
void SettingsDialog::setShortBreakDuration(int minutes) { m_shortBreakSpin->setValue(minutes); }
void SettingsDialog::setLongBreakDuration(int minutes) { m_longBreakSpin->setValue(minutes); }
void SettingsDialog::setAutoStartBreaks(bool enabled) { m_autoStartBreaksCheck->setChecked(enabled); }
void SettingsDialog::setAutoStartWork(bool enabled) { m_autoStartWorkCheck->setChecked(enabled); }
void SettingsDialog::setMinimizeToTray(bool enabled) { m_minimizeToTrayCheck->setChecked(enabled); }
void SettingsDialog::setShowNotifications(bool enabled) { m_showNotificationsCheck->setChecked(enabled); }

void SettingsDialog::resetToDefaults()
{
    setWorkDuration(25);
    setShortBreakDuration(5);
    setLongBreakDuration(15);
    setAutoStartBreaks(false);
    setAutoStartWork(false);
    setMinimizeToTray(true);
    setShowNotifications(true);
}

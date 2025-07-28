#include "SystemTrayManager.h"
#include <QApplication>
#include <QAction>

SystemTrayManager::SystemTrayManager(QObject *parent)
    : QObject(parent), m_trayIcon(nullptr), m_trayMenu(nullptr) {
  if (!QSystemTrayIcon::isSystemTrayAvailable()) {
    qWarning("System tray is not available on this system.");
    return;
  }

  m_trayIcon = new QSystemTrayIcon(this);
  m_trayIcon->setIcon(qApp->windowIcon());

  setupTrayMenu();

  connect(m_trayIcon,
          QOverload<QSystemTrayIcon::ActivationReason>::of(
              &QSystemTrayIcon::activated),
          this, &SystemTrayManager::onTrayIconActivated);
}
SystemTrayManager::~SystemTrayManager() {}

void SystemTrayManager::setupTrayMenu()
{
    m_trayMenu = new QMenu();

    // Use QStringLiteral for string literals to avoid runtime allocations
    m_trayMenu->addAction(QStringLiteral("Show Timer"), this, &SystemTrayManager::showMainWindow);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(QStringLiteral("Start/Pause"), this, &SystemTrayManager::startPauseRequested);
    m_trayMenu->addAction(QStringLiteral("Reset"), this, &SystemTrayManager::resetRequested);
    m_trayMenu->addAction(QStringLiteral("Skip Session"), this, &SystemTrayManager::skipRequested);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(QStringLiteral("Settings"), this, &SystemTrayManager::settingsRequested);
    m_trayMenu->addAction(QStringLiteral("Statistics"), this, &SystemTrayManager::statisticsRequested);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(QStringLiteral("Quit"), this, &SystemTrayManager::quitRequested);

    m_trayIcon->setContextMenu(m_trayMenu);
}

void SystemTrayManager::show()
{
    if (m_trayIcon) {
        m_trayIcon->show();
    }
}

void SystemTrayManager::hide()
{
    if (m_trayIcon) {
        m_trayIcon->hide();
    }
}

bool SystemTrayManager::isVisible() const
{
    return m_trayIcon && m_trayIcon->isVisible();
}

void SystemTrayManager::updateTooltip(TimerState state, bool isRunning, const QString &timeRemaining, int currentSession, int totalSessions)
{
    if (!m_trayIcon) return;

    // Build tooltip more efficiently
    QString tooltip;
    tooltip.reserve(100); // Reserve space to avoid reallocations

    QString emoji = TimerStateHelper::getStateEmoji(state);
    QString stateText = TimerStateHelper::getStateText(state);

    if (isRunning) {
        tooltip = QStringLiteral("%1 %2 - %3 remaining").arg(
            emoji,
            stateText,
            timeRemaining);
    } else {
        stateText += " (Paused)";
        tooltip = QStringLiteral("%1 %2 Ready (%3)").arg(
            emoji,
            stateText,
            timeRemaining);
    }

    tooltip += QStringLiteral("\nSession %1 of %2").arg(currentSession).arg(totalSessions);

    // Only update if tooltip changed
    if (tooltip != m_lastTooltip) {
        m_trayIcon->setToolTip(tooltip);
        m_lastTooltip = std::move(tooltip);
    }
}

void SystemTrayManager::showMessage(const QString &title, const QString &message)
{
    if (m_trayIcon && m_trayIcon->isVisible()) {
        m_trayIcon->showMessage(title, message, QSystemTrayIcon::Information, 5000);
    }
}

void SystemTrayManager::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        emit showMainWindow();
    }
}

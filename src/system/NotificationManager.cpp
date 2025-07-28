#include "NotificationManager.h"
#include "SystemTrayManager.h"
#include <QWidget>
#include <QMessageBox>

NotificationManager::NotificationManager(QWidget *parent)
    : QObject(parent)
    , m_parent(parent)
    , m_trayManager(nullptr)
    , m_notificationsEnabled(true)
{
}

NotificationManager::~NotificationManager() = default;

void NotificationManager::setSystemTrayManager(SystemTrayManager *trayManager)
{
    if (!trayManager) {
        qWarning() << "NotificationManager: Attempting to set null tray manager";
        return;
    }
    m_trayManager = trayManager;
}

void NotificationManager::setNotificationsEnabled(bool enabled)
{
    m_notificationsEnabled = enabled;
}

void NotificationManager::showNotification(const QString &message) const
{
    if (!m_notificationsEnabled || message.isEmpty()) {
        return;
    }

    if (m_trayManager && m_trayManager->isVisible()) {
        m_trayManager->showMessage(QStringLiteral("Pomodoro Timer"), message);
    } else if (m_parent) {
        QMessageBox::information(m_parent, QStringLiteral("Pomodoro Timer"), message);
    }
}

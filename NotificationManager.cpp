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

void NotificationManager::setSystemTrayManager(SystemTrayManager *trayManager)
{
    m_trayManager = trayManager;
}

void NotificationManager::setNotificationsEnabled(bool enabled)
{
    m_notificationsEnabled = enabled;
}

void NotificationManager::showNotification(const QString &message)
{
    if (!m_notificationsEnabled) return;

    if (m_trayManager && m_trayManager->isVisible()) {
        m_trayManager->showMessage("Pomodoro Timer", message);
    } else if (m_parent) {
        QMessageBox::information(m_parent, "Pomodoro Timer", message);
    }
}


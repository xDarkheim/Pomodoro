#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include <QString>

class QWidget;
class SystemTrayManager;

class NotificationManager : public QObject
{
    Q_OBJECT

public:
    explicit NotificationManager(QWidget *parent = nullptr);
    ~NotificationManager() = default;

    void setSystemTrayManager(SystemTrayManager *trayManager);
    void setNotificationsEnabled(bool enabled);
    void showNotification(const QString &message);

private:
    QWidget *m_parent;
    SystemTrayManager *m_trayManager;
    bool m_notificationsEnabled;
};

#endif // NOTIFICATIONMANAGER_H


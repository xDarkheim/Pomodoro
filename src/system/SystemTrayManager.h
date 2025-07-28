#ifndef SYSTEMTRAYMANAGER_H
#define SYSTEMTRAYMANAGER_H

#include <QSystemTrayIcon>
#include <QMenu>
#include "TimerState.h"

class SystemTrayManager : public QObject
{
    Q_OBJECT

public:
    explicit SystemTrayManager(QObject *parent = nullptr);
    ~SystemTrayManager() override;

    void show();
    void hide();
    bool isVisible() const;
    void updateTooltip(TimerState state, bool isRunning, const QString &timeRemaining, int currentSession, int totalSessions);
    void showMessage(const QString &title, const QString &message);

signals:
    void showMainWindow();
    void startPauseRequested();
    void resetRequested();
    void skipRequested();
    void settingsRequested();
    void statisticsRequested();
    void quitRequested();

private slots:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void setupTrayMenu();

    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;

    // Cache to avoid unnecessary tooltip updates
    QString m_lastTooltip;
};

#endif // SYSTEMTRAYMANAGER_H

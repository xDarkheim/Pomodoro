#ifndef KEYBOARDSHORTCUTS_H
#define KEYBOARDSHORTCUTS_H

#include <QShortcut>

class KeyboardShortcuts : public QObject
{
    Q_OBJECT

public:
    explicit KeyboardShortcuts(QWidget *parent = nullptr);
    ~KeyboardShortcuts() override;

signals:
    void startPauseRequested();
    void pauseRequested();
    void resetRequested();
    void settingsRequested();
    void skipRequested();

private:
    void setupShortcuts(QWidget *parent);

    QShortcut *m_startShortcut{};
    QShortcut *m_pauseShortcut{};
    QShortcut *m_resetShortcut{};
    QShortcut *m_settingsShortcut{};
    QShortcut *m_skipShortcut{};
};

#endif // KEYBOARDSHORTCUTS_H


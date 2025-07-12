#include "KeyboardShortcuts.h"
#include <QWidget>

KeyboardShortcuts::KeyboardShortcuts(QWidget *parent)
    : QObject(parent)
{
    setupShortcuts(parent);
}

void KeyboardShortcuts::setupShortcuts(QWidget *parent)
{
    m_startShortcut = new QShortcut(QKeySequence(Qt::Key_Space), parent);
    m_pauseShortcut = new QShortcut(QKeySequence(Qt::Key_P), parent);
    m_resetShortcut = new QShortcut(QKeySequence(Qt::Key_R), parent);
    m_settingsShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Comma), parent);
    m_skipShortcut = new QShortcut(QKeySequence(Qt::Key_S), parent);

    connect(m_startShortcut, &QShortcut::activated, this, &KeyboardShortcuts::startPauseRequested);
    connect(m_pauseShortcut, &QShortcut::activated, this, &KeyboardShortcuts::pauseRequested);
    connect(m_resetShortcut, &QShortcut::activated, this, &KeyboardShortcuts::resetRequested);
    connect(m_settingsShortcut, &QShortcut::activated, this, &KeyboardShortcuts::settingsRequested);
    connect(m_skipShortcut, &QShortcut::activated, this, &KeyboardShortcuts::skipRequested);
}


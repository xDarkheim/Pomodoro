#include <QApplication>
#include <QScreen>
#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include <QRadialGradient>
#include <QPen>
#include <QBrush>
#include "PomodoroTimer.h"

namespace {
    // Application constants - use QStringLiteral for compile-time optimization
    const QString APP_NAME = QStringLiteral("Pomodoro Timer");
    const QString APP_VERSION = QStringLiteral("0.1.0");
    const QString ORGANIZATION = QStringLiteral("PomodoroApp");

    // Icon generation constants
    constexpr double TOMATO_SIZE_RATIO = 0.8;
    constexpr double STEM_SIZE_RATIO = 0.15;
    constexpr double TOMATO_FLATTEN_RATIO = 0.9;
    constexpr int PEN_WIDTH = 1;
    constexpr int HIGHLIGHT_ALPHA = 100;

    // Cache for generated icons to avoid regeneration
    QIcon g_cachedAppIcon;

    QIcon createTomatoIcon(int size = 32)
    {
        QPixmap pixmap(size, size);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);

        // Calculate proportions
        const int tomatoSize = static_cast<int>(size * TOMATO_SIZE_RATIO);
        const int stemSize = static_cast<int>(size * STEM_SIZE_RATIO);
        const int margin = (size - tomatoSize) / 2;

        // Draw tomato body with gradient
        QRadialGradient tomatoGradient(size/2, size/2 + stemSize, tomatoSize/2);
        tomatoGradient.setColorAt(0.0, QColor(255, 100, 100));
        tomatoGradient.setColorAt(0.7, QColor(220, 50, 50));
        tomatoGradient.setColorAt(1.0, QColor(180, 30, 30));

        painter.setBrush(QBrush(tomatoGradient));
        painter.setPen(QPen(QColor(150, 20, 20), PEN_WIDTH));

        const QRect tomatoRect(margin, margin + stemSize, tomatoSize,
                              static_cast<int>(tomatoSize * TOMATO_FLATTEN_RATIO));
        painter.drawEllipse(tomatoRect);

        // Draw stem and leaves
        painter.setBrush(QBrush(QColor(34, 139, 34)));
        painter.setPen(QPen(QColor(20, 100, 20), PEN_WIDTH));

        // Main stem
        const QRect stemRect(size/2 - stemSize/3, margin, stemSize/1.5, stemSize);
        painter.drawRect(stemRect);

        // Draw leaves using polygons - optimize polygon creation
        QPolygon leftLeaf, rightLeaf;
        leftLeaf.reserve(3);
        rightLeaf.reserve(3);

        leftLeaf << QPoint(size/2 - stemSize/2, margin + stemSize/3)
                 << QPoint(size/2 - stemSize, margin)
                 << QPoint(size/2 - stemSize/3, margin + stemSize/2);

        rightLeaf << QPoint(size/2 + stemSize/2, margin + stemSize/3)
                  << QPoint(size/2 + stemSize, margin)
                  << QPoint(size/2 + stemSize/3, margin + stemSize/2);

        painter.drawPolygon(leftLeaf);
        painter.drawPolygon(rightLeaf);

        // Add 3D highlight effect
        painter.setBrush(QBrush(QColor(255, 150, 150, HIGHLIGHT_ALPHA)));
        painter.setPen(Qt::NoPen);
        const QRect highlightRect(margin + tomatoSize/4, margin + stemSize + tomatoSize/4,
                                 tomatoSize/3, tomatoSize/4);
        painter.drawEllipse(highlightRect);

        return QIcon(pixmap);
    }

    const QIcon& createApplicationIcon()
    {
        if (g_cachedAppIcon.isNull()) {
            constexpr int iconSizes[] = {16, 24, 32, 48, 64};

            for (int size : iconSizes) {
                g_cachedAppIcon.addPixmap(createTomatoIcon(size).pixmap(size, size));
            }
        }
        return g_cachedAppIcon;
    }

    void centerWindow(QWidget* window)
    {
        const QScreen* screen = QApplication::primaryScreen();
        if (!screen) return;

        const QRect screenGeometry = screen->availableGeometry();
        const int x = (screenGeometry.width() - window->width()) / 2;
        const int y = (screenGeometry.height() - window->height()) / 2;
        window->move(x, y);
    }

    void setupApplicationProperties(QApplication& app)
    {
        app.setApplicationName(APP_NAME);
        app.setApplicationVersion(APP_VERSION);
        app.setOrganizationName(ORGANIZATION);
        app.setWindowIcon(createApplicationIcon());
        app.setQuitOnLastWindowClosed(false);
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    setupApplicationProperties(app);

    PomodoroTimer timer;
    timer.show();
    centerWindow(&timer);

    return app.exec();
}
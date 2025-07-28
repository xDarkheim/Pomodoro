#include "CircularProgressBar.h"
#include <QPainter>
#include <QResizeEvent>

CircularProgressBar::CircularProgressBar(QWidget *parent)
    : QWidget(parent)
    , m_value(0)
    , m_maximum(100)
{
    setFixedSize(220, 220);
}

void CircularProgressBar::setValue(const int value) {
  if (const int clampedValue = qBound(0, value, m_maximum); clampedValue != m_value) {
        m_value = clampedValue;
        update();
    }
}

void CircularProgressBar::setMaximum(int maximum){
    m_maximum = maximum;
    update();
}

void CircularProgressBar::paintEvent(QPaintEvent *event){
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int side = qMin(width(), height());
    const QRect rect((width() - side) / 2 + 20, (height() - side) / 2 + 20,
               side - 40, side - 40);

    // Background circle
    QPen backgroundPen(QColor(200, 200, 200), 12);
    backgroundPen.setCapStyle(Qt::RoundCap);
    painter.setPen(backgroundPen);
    painter.drawEllipse(rect);

    // Progress arc
    if (m_value > 0) {
        QPen progressPen(QColor(70, 130, 180), 12);
        progressPen.setCapStyle(Qt::RoundCap);
        painter.setPen(progressPen);

        const int angle = (360 * m_value) / m_maximum;
        painter.drawArc(rect, 90 * 16, -angle * 16);
    }
}

void CircularProgressBar::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);
    updateChildPositions();
}

void CircularProgressBar::updateChildPositions() const {
    const auto children = findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
    for (QWidget *child : children) {
        child->resize(width(), 40);
        child->move(0, (height() - 40) / 2);
    }
}

#ifndef CIRCULARPROGRESSBAR_H
#define CIRCULARPROGRESSBAR_H

#include <QWidget>

class CircularProgressBar : public QWidget
{
    Q_OBJECT

public:
    explicit CircularProgressBar(QWidget *parent = nullptr);

    void setValue(int value);
    void setMaximum(int maximum);
    int value() const { return m_value; }
    int maximum() const { return m_maximum; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    int m_value;
    int m_maximum;

    void updateChildPositions();
};

#endif // CIRCULARPROGRESSBAR_H

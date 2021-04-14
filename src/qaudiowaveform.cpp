#include <QPainter>
#include "qaudiowaveform.h"
#include <QTime>

#include <QDebug>

QAudioWaveform::QAudioWaveform(QWidget *parent, int index) :
    QWidget(parent)
{
      setMinimumHeight(30);
      waveformNumber = index;
}

void QAudioWaveform::clearLevels()
{
    while (!m_levels.isEmpty())
        m_levels.removeFirst();
}

void QAudioWaveform::updateLevel(qreal level)
{
    while (m_levels.count() >= width())
        m_levels.removeFirst();
    m_levels.append(level);
    update();
}


void QAudioWaveform::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    QColor paint;
    qreal level;
    qreal fractionalLevel;
    for (int i = 0; i < m_levels.count(); i++) {
        fractionalLevel = m_levels.at(i);
        if (fractionalLevel < .5) {
            paint = QColor(0, 150, 35);     // Green
        } else if (fractionalLevel < .6) {
            paint = QColor(255, 212, 0);    // Yellow
        } else if (fractionalLevel < .7) {
            paint = QColor(255, 144, 0);    // Orange
        } else {
            paint = QColor(255, 0, 0);      // Red
        }

        painter.setPen(QPen(paint, 1, Qt::SolidLine, Qt::RoundCap));
        level = fractionalLevel * height();
        if (waveformNumber % 2 == 0)
            painter.drawLine(i, height(), i, height() - level);
        else
            painter.drawLine(i, level, i, 0);
    }
}

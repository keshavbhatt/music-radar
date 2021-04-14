#ifndef QAUDIOWAVEFORM_H
#define QAUDIOWAVEFORM_H

#include <QWidget>

namespace Ui {
class QAudioWaveform;
}

class QAudioWaveform : public QWidget
{
    Q_OBJECT

public:
    explicit QAudioWaveform(QWidget *parent, int index);
    void updateLevel(qreal level);
    void clearLevels();

protected:
    void paintEvent(QPaintEvent *event);

private:
    int waveformNumber;
    QList<qreal> m_levels;
};

#endif // QAUDIOWAVEFORM_H

#ifndef HOME_H
#define HOME_H

#include <QAudioDeviceInfo>
#include <QWidget>
#include <QSettings>

#include "pulseutils.h"

namespace Ui {
class Home;
}

class Home : public QWidget
{
    Q_OBJECT

public:
    explicit Home(const QList<QAudioDeviceInfo> &availableInputDevices,
                  QWidget *parent = nullptr);
    ~Home();

public slots:
    void animate();

signals:
    void startRecording(QString selectedDevice);
    void showHistory();

private slots:
    void on_fromMicrophone_toggled(bool checked);


    void on_fromSpeaker_toggled(bool checked);

    void updateDevices();
    void on_recordToolButton_clicked();
    QString getSelectedDevice();


private:
    Ui::Home *ui;

    PulseUtils * pulseUtils = nullptr;

    QString defaultSink;
    QString defaultSource;

    QSettings settings;

};

#endif // HOME_H

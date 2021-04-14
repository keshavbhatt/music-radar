#include "home.h"
#include "ui_home.h"

#include <QAudioDeviceInfo>
#include <QGraphicsOpacityEffect>
#include <QMessageBox>
#include <QPropertyAnimation>


Home::Home(
    const QList<QAudioDeviceInfo> &availableInputDevices,
    QWidget *parent)
    : QWidget(parent),
    ui(new Ui::Home)
{
    ui->setupUi(this);

    ui->welcome_lable->setText(QString("<p><span style='font-size:16pt;'>Welcome to %1</span></p>")
                               .arg(QApplication::applicationName()));

    pulseUtils = new PulseUtils(this);


//    QAudioDeviceInfo device;
//    foreach (device, availableInputDevices)
//        ui->devicesComboBox->addItem(device.deviceName(),
//                                       QVariant::fromValue(device));

    animate();
}

void Home::animate()
{
    this->hide();
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(eff);

    QPropertyAnimation *a = new QPropertyAnimation(this->graphicsEffect(),"opacity");
    a->setDuration(400);
    a->setStartValue(0);
    a->setEndValue(1);
    a->setEasingCurve(QEasingCurve::InCurve);
    a->start(QPropertyAnimation::DeleteWhenStopped);
    connect(a,&QPropertyAnimation::finished,[this,eff](){
       eff->deleteLater();
    });
    this->show();
}

Home::~Home()
{
    pulseUtils->deleteLater();
    delete ui;
}

void Home::updateDevices()
{
    defaultSink     = pulseUtils->getDefaultSink();
    defaultSource   = pulseUtils->getDefaultSource();

    if(defaultSink.trimmed().isEmpty()){
        QMessageBox::critical(this,"Error","Cannot get the default Sink.");
    }
    if(defaultSource.trimmed().isEmpty()){
        QMessageBox::critical(this,"Error","Cannot get the default Source.");
    }
}

void Home::on_fromMicrophone_toggled(bool checked)
{
    if(checked)
        updateDevices();

}

void Home::on_fromSpeaker_toggled(bool checked)
{
    if(checked)
        updateDevices();
}

void Home::on_recordToolButton_clicked()
{
    emit startRecording(getSelectedDevice());
}

QString Home::getSelectedDevice()
{
    return ui->fromMicrophone->isChecked() ? defaultSource : defaultSink;
}

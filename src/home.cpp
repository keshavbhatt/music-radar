#include "home.h"
#include "ui_home.h"

#include <QAudioDeviceInfo>
#include <QGraphicsOpacityEffect>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QDebug>

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


//    if(settings.value("source","speaker").toString() == "speaker"){
//        ui->fromMicrophone->blockSignals(true);
//        ui->fromMicrophone->setChecked(true);
//        ui->fromMicrophone->blockSignals(false);
//    }else{
//        ui->fromSpeaker->blockSignals(true);
//        ui->fromSpeaker->setChecked(true);
//        ui->fromSpeaker->blockSignals(false);
//    }

    bool speaker = (settings.value("source","speaker").toString() == "speaker");
    qDebug()<< speaker;

    settings.value("source","speaker").toString() == "speaker"
            ? ui->fromSpeaker->setChecked(true)
            : ui->fromMicrophone->setChecked(true);
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
    if(checked){
        updateDevices();
        settings.setValue("source","microphone");
    }
}

void Home::on_fromSpeaker_toggled(bool checked)
{
    if(checked){
        updateDevices();
        settings.setValue("source","speaker");
    }
}

void Home::on_recordToolButton_clicked()
{
    emit startRecording(getSelectedDevice());
}

QString Home::getSelectedDevice()
{
    updateDevices();
    return ui->fromMicrophone->isChecked() ? defaultSource : defaultSink;
}

#include "settingswidget.h"
#include "ui_settingswidget.h"
#include <QDebug>
#include <lib/QtStyler/qtstyler.h>

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);

    ui->infoLabel->setText(ui->infoLabel->text().arg(QApplication::applicationName()));

    QString t = "";

    if(settings.value("yek").isNull()){
        settings.setValue("yek",t.toUtf8().toBase64());
        ui->keyLineEdit->setText(t);
    }else{
        QByteArray base64EncodedKey = settings.value("yek",t.toUtf8().toBase64()).toByteArray();
        ui->keyLineEdit->setText(QString(QByteArray::fromBase64(base64EncodedKey)));
    }

    ui->saveKeys->setEnabled(false);

    QtStyler *styler = new QtStyler(this);
    ui->themeCombo->blockSignals(true);
    ui->themeCombo->addItems(styler->availableStyles());
    ui->themeCombo->blockSignals(false);
    //ui->themeCombo->setCurrentText(styler->currentStyle());
    connect(ui->themeCombo, &QComboBox::currentTextChanged, styler, &QtStyler::setStyle);
    connect(styler, &QtStyler::styleChanged, ui->themeCombo, &QComboBox::setCurrentText);

    ui->themeCombo->setCurrentIndex(settings.value("theme",0).toInt());
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}

void SettingsWidget::closeEvent(QCloseEvent *event)
{
    settings.setValue("settingsGeometry",this->geometry());
    QWidget::closeEvent(event);
}

void SettingsWidget::on_infoLabel_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}


void SettingsWidget::on_saveKeys_clicked()
{
    QString key = ui->keyLineEdit->text().trimmed();
    if(key.isEmpty()){
        key = "";
    }
    settings.setValue("yek",key.toUtf8().toBase64());

    //load
    QByteArray base64EncodedKey = settings.value("yek",key.toUtf8().toBase64()).toByteArray();
    ui->keyLineEdit->setText(QString(QByteArray::fromBase64(base64EncodedKey)));

    ui->saveKeys->setEnabled(false);
    ui->saveKeys->setText("Saved");
}

void SettingsWidget::on_keyLineEdit_textChanged(const QString &arg1)
{
    QString key = "";
    QByteArray base64EncodedKey = settings.value("yek",key.toUtf8().toBase64()).toByteArray();
    if(arg1 != QString(QByteArray::fromBase64(base64EncodedKey))){
        ui->saveKeys->setEnabled(true);
        ui->saveKeys->setText("Save");
    }else{
        ui->saveKeys->setEnabled(false);
        ui->saveKeys->setText("Save");
    }
}

void SettingsWidget::on_themeCombo_currentIndexChanged(int index)
{
    settings.setValue("theme",index);
}

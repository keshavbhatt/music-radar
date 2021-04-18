#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QUrl>
#include <QDesktopServices>
#include <QSettings>


namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    ~SettingsWidget();

protected slots:
    void closeEvent(QCloseEvent *event);
private slots:
    void on_infoLabel_linkActivated(const QString &link);

    void on_saveKeys_clicked();

    void on_keyLineEdit_textChanged(const QString &arg1);

    void on_themeCombo_currentIndexChanged(int index);

    void on_autoFinishDurationSpinBox_valueChanged(int arg1);

private:
    Ui::SettingsWidget *ui;
    QSettings settings;
};

#endif // SETTINGSWIDGET_H

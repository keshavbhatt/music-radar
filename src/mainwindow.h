#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QMainWindow>
#include <QSettings>
#include <QDebug>
#include <QPushButton>


#include "about.h"

#include "home.h"
#include "recordingpage.h"
#include "settingswidget.h"
#include "iteminfowidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected slots:
    void closeEvent(QCloseEvent *event);
private slots:
    void createActions();
    void showAbout();

    void showSettings();
    void init_settings();

private:
    Ui::MainWindow *ui;

    QAction *aboutAction;
    QAction *backAction;
    QAction *historyAction;
    QAction *itemDeleteAction, *itemInfoAction;
    QAction *settingsAction;

    Home *homeWidget = nullptr;
    RecordingPage *recordingPageWidget = nullptr;
    QSettings settings;

    SettingsWidget *settingsWidget = nullptr;
};

#endif // MAINWINDOW_H

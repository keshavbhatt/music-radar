#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCloseEvent>
#include <QMainWindow>
#include <QSettings>

#include "about.h"

#include "home.h"
#include "recordingpage.h"

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

private:
    Ui::MainWindow *ui;

    QAction *aboutAction;
    QAction *backAction;
    QAction *historyAction;
    QAction *itemDeleteAction;

    Home *homeWidget = nullptr;
    RecordingPage *recordingPageWidget = nullptr;
    QSettings settings;
};

#endif // MAINWINDOW_H

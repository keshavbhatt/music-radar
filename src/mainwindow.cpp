#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    setWindowTitle(QApplication::applicationName());
    setWindowIcon(QIcon(":/icons/app/icon-256.png"));
    setMinimumWidth(600);
    setMinimumHeight(450);

    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());


    homeWidget          = new Home(QAudioDeviceInfo::availableDevices(QAudio::AudioInput),this);
    recordingPageWidget = new RecordingPage(this);

    connect(recordingPageWidget,&RecordingPage::enableItemActions,[=](bool enable){
       itemDeleteAction->setEnabled(enable);
    });

    connect(recordingPageWidget,&RecordingPage::back,[=](){
       backAction->trigger();
    });

    connect(homeWidget,&Home::startRecording,[=](QString selectedDevice){
       backAction->setEnabled(true);
       ui->stackedWidget->setCurrentWidget(recordingPageWidget);
       recordingPageWidget->animate();
       recordingPageWidget->stopRecording();
       recordingPageWidget->startRecording(selectedDevice);
    });

    ui->stackedWidget->addWidget(homeWidget);
    ui->stackedWidget->addWidget(recordingPageWidget);

    createActions();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    QMainWindow::closeEvent(event);
}

void MainWindow::createActions()
{
    aboutAction = new QAction(tr("&About"), this);
    aboutAction->setIcon(QIcon(":/icons/app/icon-32.png"));
    aboutAction->setIconText("About");

    backAction = new QAction(tr("&Back"), this);
    backAction->setIcon(QIcon(":/icons/arrow-left-circle-line.png"));
    backAction->setIconText("Back");

    historyAction = new QAction(tr("&History"), this);
    historyAction->setIcon(QIcon(":/icons/time-line.png"));
    historyAction->setIconText("History");

    itemDeleteAction = new QAction(QIcon(":/icons/delete-bin-5-line.png"),"Delete",this);

    connect(itemDeleteAction, &QAction::triggered,recordingPageWidget,&RecordingPage::deleteSelected);

    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);

    connect(backAction, &QAction::triggered,[=]()
    {
        backAction->setEnabled(false);
        recordingPageWidget->clearDebug();
        recordingPageWidget->stopRecording();
        recordingPageWidget->stopAllPlayers();
        ui->stackedWidget->setCurrentWidget(homeWidget);
        homeWidget->animate();
    });

    connect(historyAction, &QAction::triggered,[=]()
    {
        backAction->setEnabled(true);
        ui->stackedWidget->setCurrentWidget(recordingPageWidget);
        recordingPageWidget->showHistory();
    });


    ui->mainToolBar->addAction(backAction);
    backAction->setEnabled(false);

    ui->mainToolBar->addSeparator();


    ui->mainToolBar->addAction(historyAction);
    historyAction->setEnabled(true);

    ui->mainToolBar->addSeparator();

    ui->mainToolBar->addAction(itemDeleteAction);
    itemDeleteAction->setEnabled(false);

    QWidget* hSpacer= new QWidget(this);
    hSpacer->setStyleSheet("background-color:transparent");
    hSpacer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    ui->mainToolBar->addWidget(hSpacer);

    ui->mainToolBar->addAction(aboutAction);

}

void MainWindow::showAbout()
{
    About *about = new About(this);
    about->setWindowFlags(about->windowFlags()| Qt::Dialog);
    about->setMinimumSize(about->sizeHint());
    about->adjustSize();
    about->setAttribute(Qt::WA_DeleteOnClose);
    about->show();
}


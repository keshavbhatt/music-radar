#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QMargins m = ui->mainToolBar->layout()->contentsMargins();
    ui->mainToolBar->setContentsMargins(m.left(),m.top(),m.right(),m.top());

    setWindowTitle(QApplication::applicationName());
    setWindowIcon(QIcon(":/icons/app/icon-256.png"));
    setMinimumWidth(600);
    setMinimumHeight(450);

    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());


    homeWidget          = new Home(QAudioDeviceInfo::availableDevices(QAudio::AudioInput),this);
    recordingPageWidget = new RecordingPage(this);

    connect(recordingPageWidget,&RecordingPage::showJson,[=](QString jsonPath){
        ItemInfoWidget * info = new ItemInfoWidget(this);
        info->setWindowTitle(QApplication::applicationName()+" | "+tr("Item info"));
        info->setWindowFlag(Qt::Dialog);
        info->load(jsonPath);
        info->setAttribute(Qt::WA_DeleteOnClose);
        info->show();
    });

    connect(recordingPageWidget,&RecordingPage::showFixKeyError,[=](QString error){
        QMessageBox msgBox;
        msgBox.setText(QApplication::applicationName()+" | Incorrect API Token");
        msgBox.setText(error);
        msgBox.setInformativeText("Do you want to correct API Token now?");

        QPushButton *settingsButton = msgBox.addButton(tr("Yes, correct API Token"), QMessageBox::ActionRole);
        QPushButton *abortButton = msgBox.addButton(QMessageBox::No);
        msgBox.exec();
        if (msgBox.clickedButton() == settingsButton) {
          settingsAction->trigger();
        } else if (msgBox.clickedButton() == abortButton) {
         msgBox.close();
        }
    });

    connect(recordingPageWidget,&RecordingPage::showPutKeyError,[=](QString error){
        QMessageBox msgBox;
        msgBox.setText(QApplication::applicationName()+" | Need API Token");
        msgBox.setText(error);
        msgBox.setInformativeText("Do you want to set API Token now?");

        QPushButton *settingsButton = msgBox.addButton(tr("Yes, set API Token"), QMessageBox::ActionRole);
        QPushButton *abortButton = msgBox.addButton(QMessageBox::No);
        msgBox.exec();
        if (msgBox.clickedButton() == settingsButton) {
          settingsAction->trigger();
        } else if (msgBox.clickedButton() == abortButton) {
         msgBox.close();
        }
    });

    connect(recordingPageWidget,&RecordingPage::enableItemActions,[=](bool enable){
       itemDeleteAction->setEnabled(enable);
       itemInfoAction->setEnabled(enable);
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

    init_settings();

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
    aboutAction      = new QAction(QIcon(":/icons/app/icon-32.png"),tr("&About"), this);
    backAction       = new QAction(QIcon(":/icons/arrow-left-circle-line.png"),tr("&Back"), this);
    historyAction    = new QAction(QIcon(":/icons/time-line.png"),tr("&History"), this);
    itemDeleteAction = new QAction(QIcon(":/icons/delete-bin-5-line.png"),tr("Delete"),this);
    itemInfoAction   = new QAction(QIcon(":/icons/information-line.png"),tr("Info"),this);
    settingsAction   = new QAction(QIcon(":/icons/setting-line.png"),tr("Settings"),this);

    connect(settingsAction, &QAction::triggered,this,&MainWindow::showSettings);

    connect(itemDeleteAction, &QAction::triggered,recordingPageWidget,&RecordingPage::deleteSelected);

    connect(itemInfoAction, &QAction::triggered,recordingPageWidget,&RecordingPage::showItemInfo);

    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);

    connect(backAction, &QAction::triggered,[=]()
    {
        backAction->setEnabled(false);
        emit recordingPageWidget->enableItemActions(false);
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

    ui->mainToolBar->addAction(itemInfoAction);
    itemInfoAction->setEnabled(false);

    QWidget* hSpacer= new QWidget(this);
    hSpacer->setStyleSheet("background-color:transparent");
    hSpacer->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

    ui->mainToolBar->addWidget(hSpacer);

    ui->mainToolBar->addAction(settingsAction);

    ui->mainToolBar->addAction(aboutAction);

}

void MainWindow::init_settings()
{
    settingsWidget = new SettingsWidget(this);
    settingsWidget->setWindowTitle(QApplication::applicationName()+"| "+tr("Settings"));
    settingsWidget->setWindowFlag(Qt::Dialog);
    settingsWidget->setWindowModality(Qt::WindowModal);
    settingsWidget->restoreGeometry(settings.value("settingsGeometry").toByteArray());
}

void MainWindow::showSettings()
{
    if(!settingsWidget->isVisible()){
        settingsWidget->show();
    }else{
        settingsWidget->raise();
        settingsWidget->activateWindow();
    }
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


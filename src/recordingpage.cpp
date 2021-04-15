#include "recordingpage.h"
#include "ui_recordingpage.h"

#include <QStandardPaths>
#include "utils.h"
#include <QFileInfo>
#include <QtNetwork>
#include <QTextEdit>
#include <QGraphicsOpacityEffect>

RecordingPage::RecordingPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecordingPage)
{
    ui->setupUi(this);

    networkManager = new QNetworkAccessManager(this);

    recordingFileName = utils::returnPath("temp")+"temp.mp3";
    historyPath       = utils::returnPath("history");


    ui->waveformWidget->hide();

    // loader is the child of results
    _loader = new WaitingSpinnerWidget(ui->resultListWidget,true,false);
    _loader->setRoundness(70.0);
    _loader->setMinimumTrailOpacity(15.0);
    _loader->setTrailFadePercentage(70.0);
    _loader->setNumberOfLines(10);
    _loader->setLineLength(8);
    _loader->setLineWidth(2);
    _loader->setInnerRadius(2);
    _loader->setRevolutionsPerSecond(3);
    _loader->setColor(QColor("#1e90ff"));

    animate();
}

void RecordingPage::animate()
{
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
    qApp->processEvents();
}


void RecordingPage::loadHistory()
{
    ui->resultListWidget->clear();
    QDir savedDir(historyPath);
    savedDir.setFilter(QDir::Files|QDir::NoSymLinks|QDir::NoDotAndDotDot);
    savedDir.setNameFilters(QStringList()<<"*.json");
    foreach(QFileInfo info, savedDir.entryInfoList()){
            uploadFinished(true,info.filePath());
    }
    if(ui->resultListWidget->currentItem() == nullptr){
        emit enableItemActions(false);
    }
}

void RecordingPage::createRecodingFile()
{
    QFile recordingFile(recordingFileName);
    if(!recordingFile.open(QIODevice::WriteOnly)){
        QMessageBox::critical(this,"Error","Unable to create RecordingFile.");
        stopRecording();
        emit back();
    }
    recordingFile.close();
}

RecordingPage::~RecordingPage()
{
    stopRecording();
    delete ui;
}


void RecordingPage::startRecording(QString selectedDevice)
{
    ui->resultListWidget->clear();

    debug.clear();
    createRecodingFile();

    ui->debugButton->show();
    ui->statusLabel->show();
    ui->label->show();
    ui->cancelButton->show();

    QProcess *recordingProcess = new QProcess(this);
    recordingProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(recordingProcess,&QProcess::readyRead,[=]()
    {
        QString bytes = recordingProcess->readAll();
        if(QString(bytes).contains("Time",Qt::CaseInsensitive))
        {
            ui->statusLabel->setText(QString(bytes).split(";").first().trimmed().simplified());
            if(ui->statusLabel->text().contains(" 20.")) //dirty
                on_cancelButton_clicked();
        }else{
            debug.append(bytes);
        }
    });
    connect(recordingProcess, static_cast<void (QProcess::*)
                (int,QProcess::ExitStatus)>(&QProcess::finished),[this]
                (int exitCode,QProcess::ExitStatus exitStatus){
            Q_UNUSED(exitStatus);
            Q_UNUSED(exitCode);
            ui->cancelButton->hide();
            QFileInfo info(recordingFileName);
            if(info.size() < 800)
            {
                QMessageBox::critical(this,"Error","Recorded file is empty.\nIf you installed using snapd make sure you enable audio-record permission.");
                stopRecording();
                emit back();
            }
    });

    debug.append("Used Device: "+selectedDevice+"\n\n");
    recordingProcess->start("bash",QStringList()<<"-c"<<QString("parec -v -d %1 | lame -r -V0 - %2")
                            .arg(selectedDevice,QString(recordingFileName).replace(" ","\\ ")));
    processIdList.append(recordingProcess->processId());

    if(!recordingProcess->waitForStarted(1000))
    {
        ui->cancelButton->hide();
        QMessageBox::critical(this,"Error","Failed to initialize recording process.\nPlease report to developer.");
        stopRecording();
        emit back();
    }else{
        ui->cancelButton->show();
    }
}

void RecordingPage::showHistory()
{
    ui->debugButton->hide();
    ui->statusLabel->hide();
    ui->label->hide();
    ui->cancelButton->hide();

    loadHistory();
}

void RecordingPage::clearDebug()
{
    debug.clear();
}

void RecordingPage::stopRecording()
{
    foreach(QProcess* process, this->findChildren<QProcess*>())
    {
        //kill piped process
        for(int i=0;i<processIdList.count();i++){
            QProcess::execute("pkill",QStringList()<<"-P"<<QString::number(processIdList.at(i)));
            processIdList.removeAt(i);
        }
        if(process)
        {
            process->close();
            process->deleteLater();
        }
    }
}

void RecordingPage::on_cancelButton_clicked()
{
    stopRecording();

    ui->cancelButton->hide();

    QFileInfo info(recordingFileName);
    if(info.size() > 800)
    {
        processRecording();
    }else{
        QMessageBox::critical(this,"Error","Recorded file is too small, try recording again.");
        stopRecording();
        emit back();
    }

}


QString RecordingPage::withUnit(qreal bytes)
{
    if (bytes < (1 << 10))
        return tr("%L1 B").arg(bytes);
    else if (bytes < (1 << 20))
        return tr("%L1 KiB").arg(bytes / (1 << 10), 0, 'f', 2);
    else if (bytes < (1 << 30))
        return tr("%L1 MiB").arg(bytes / (1 << 20), 0, 'f', 2);
    else
        return tr("%L1 GiB").arg(bytes / (1 << 30), 0, 'f', 2);
}

void RecordingPage::processRecording()
{
    ui->statusLabel->setText("Please wait...");
    _loader->start();

    QString t = "";
    QByteArray base64EncodedKey = settings.value("yek",t.toUtf8().toBase64()).toByteArray();

    QString bound="margin";
    QByteArray data(QString("--" + bound + "\r\n").toLatin1());
    data.append("Content-Disposition: form-data; name=\"api_token\"\r\n\r\n");
    data.append(QString(QByteArray::fromBase64(base64EncodedKey))+"\r\n");
    data.append("--" + bound + "\r\n");
    data.append("Content-Disposition: form-data; name=\"return\"\r\n\r\n");
    data.append("spotify\r\n");
    data.append("--" + bound + "\r\n");
    data.append("Content-Disposition: form-data; name=\"file\"; filename=\"test.mp3\"\r\n");
    data.append("Content-Type: audio/mpeg\r\n\r\n");
    QFile file(recordingFileName);
    if (!file.open(QIODevice::ReadOnly))
        return;
    data.append(file.readAll());
    data.append("\r\n");
    data.append("--" + bound + "--\r\n");

    QUrl url("https://api.audd.io/");
    QNetworkRequest request(url);

    request.setRawHeader(QString("Content-Type").toLatin1(),QString("multipart/form-data; boundary=" + bound).toLatin1());
    request.setRawHeader(QString("Content-Length").toLatin1(), QString::number(data.length()).toLatin1());

    networkReply = networkManager->post(request, data);
    m_timeAdded = QTime::currentTime();

    connect(networkReply, SIGNAL(finished()),this, SLOT(uploadFinished()));

    connect(networkReply, SIGNAL(uploadProgress(qint64, qint64)),this, SLOT  (uploadProgress(qint64, qint64)));
}

void RecordingPage::uploadProgress(qint64 byteSent, qint64 bytesTotal)
{
    qreal totalBytes = bytesTotal;
    qreal receivedBytes = byteSent;
    qreal bytesPerSecond = receivedBytes / m_timeAdded.elapsed() * 1000;

    if (totalBytes >= 0) {
        ui->statusLabel->setText(tr("%1 of %2 uploaded - %3/s")
                  .arg(withUnit(receivedBytes))
                  .arg(withUnit(totalBytes))
                  .arg(withUnit(bytesPerSecond)));
    } else {
       ui->statusLabel->setText(tr("unknown size - %1 uploaded - %2/s")
                 .arg(withUnit(receivedBytes))
                 .arg(withUnit(bytesPerSecond)));
    }
}

void RecordingPage::uploadFinished(bool fromHistory, QString historyItemFilePath)
{
    _loader->stop();
    ui->statusLabel->setText("Done");
    QJsonDocument jsonResponse;

    QString itemId;
    if(fromHistory){
        jsonResponse = utils::loadJson(historyItemFilePath);
        QFileInfo fi(historyItemFilePath);
        itemId = fi.baseName();
    }else{
        QString reply = networkReply->readAll();
        jsonResponse = QJsonDocument::fromJson(reply.toUtf8());
        itemId = QString::number(QDateTime::currentMSecsSinceEpoch());
    }

    if(jsonResponse.isEmpty()){
        qDebug()<<"API:"<<"Empty response returned from API call. Please report to developer.";
        return;
    }

    QJsonObject jsonObj     = jsonResponse.object();
    QString status          = jsonObj.value("status").toString();

    if(status.contains("error",Qt::CaseInsensitive)){
        //get error message
        QString error_message = jsonObj.value("error").toObject().value("error_message").toString();
        int error_code        = jsonObj.value("error").toObject().value("error_code").toInt();
        if(error_code == 901){
            emit showPutKeyError(error_message);
        }else if(error_code == 900){
            emit showFixKeyError(error_message);
        }else{
            QMessageBox::critical(this,"Error",error_message);
        }
        return;
    }
    if (status.contains("success",Qt::CaseInsensitive) && jsonObj.value("result").isNull()) {
        QString msg = "The song was not recognized.\nTry again.";
        QMessageBox::critical(this,"Error",msg);
        stopRecording();
        emit back();
    }
    else
    {
        if(!fromHistory)
            utils::saveJson(jsonResponse,historyPath+itemId+".json");

        QJsonObject resultObj   = jsonObj.value("result").toObject();
        QString artist = resultObj.value("artist").toString();
        QString title = resultObj.value("title").toString();
        QString album = resultObj.value("album").toString();
        QString release_date = resultObj.value("release_date").toString();
        //QString song_link = resultObj.value("song_link").toString();

        QJsonObject spotifyObj     = resultObj.value("spotify").toObject();
        QJsonObject appleMusicObj  = resultObj.value("apple_music").toObject();

        QJsonObject albumObj    = spotifyObj.value("album").toObject();
        QString iconUrl         = albumObj.value("images").toArray().at(1)["url"].toString();
        QString preview_url     = spotifyObj.value("preview_url").toString();
        if(preview_url.isEmpty())
            preview_url = appleMusicObj.value("previews").toArray().at(0)["url"].toString();

        QString spotify_url     =  spotifyObj.value("uri").toString();

        //add to results
        SongItem *songItem = new SongItem(ui->resultListWidget);
        songItem->setObjectName("item_"+itemId);
        connect(songItem,SIGNAL(stopAllPlayers()),this,SLOT(stopAllPlayers()));
        songItem->adjustSize();
        songItem->init(artist,title,album,release_date,iconUrl,preview_url,spotify_url);
        QListWidgetItem* item;
        item = new QListWidgetItem(ui->resultListWidget);
        item->setSizeHint(songItem->minimumSizeHint());
        ui->resultListWidget->setItemWidget(item, songItem);
        ui->resultListWidget->addItem(item);
        if(fromHistory==false)
            ui->resultListWidget->setCurrentItem(item);
    }
}

void RecordingPage::on_debugButton_clicked()
{
    QMessageBox msg;
    msg.setWindowTitle(QApplication::applicationName()+"| Debug output.");
    msg.setText("Debug output of recording process..");
    msg.setDetailedText(debug);

    QAbstractButton *detailsButton = NULL;

    foreach (QAbstractButton *button, msg.buttons()) {
        if (msg.buttonRole(button) == QMessageBox::ActionRole) {
            detailsButton = button;
            break;
        }
    }
    if (detailsButton) {
        detailsButton->click();
    }
    QList<QTextEdit*> textBoxes = msg.findChildren<QTextEdit*>();
        if(textBoxes.size())
            textBoxes[0]->setFixedSize(450, 250);
    msg.exec();
}


void RecordingPage::stopAllPlayers()
{
    foreach (SongItem *item, ui->resultListWidget->findChildren<SongItem*>()) {
        if(item){
            QList<QMediaPlayer*> players = item->findChildren<QMediaPlayer*>();
            foreach (QMediaPlayer *player, players) {
                if(player != nullptr)
                    player->stop();
            }
        }
    }
}

void RecordingPage::showItemInfo()
{
    QListWidgetItem *item = ui->resultListWidget->currentItem();
    QWidget *itemWidget = ui->resultListWidget->itemWidget(item);
    if(itemWidget){
        QString itemId   = itemWidget->objectName().split("item_").last().trimmed();
        QString filePath = historyPath+itemId+".json";
        emit showJson(filePath);
    }
    if(ui->resultListWidget->count()==0)
        emit enableItemActions(false);
}


void RecordingPage::deleteSelected()
{
   QListWidgetItem *item = ui->resultListWidget->currentItem();
   QWidget *itemWidget = ui->resultListWidget->itemWidget(item);
   if(itemWidget){
       QString itemId   = itemWidget->objectName().split("item_").last().trimmed();
       QString filePath = historyPath+itemId+".json";
       QFile itemFile(filePath);
       itemFile.remove();
       delete ui->resultListWidget->takeItem(ui->resultListWidget->currentRow());
   }
   if(ui->resultListWidget->count()==0)
       emit enableItemActions(false);
}

void RecordingPage::on_resultListWidget_itemClicked(QListWidgetItem *item)
{
    if(item != nullptr)
     emit enableItemActions(true);
    else
     emit enableItemActions(false);
}

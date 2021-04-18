#ifndef RECORDINGPAGE_H
#define RECORDINGPAGE_H

#include <QWidget>
#include <QDebug>
#include <QProcess>
#include <QTemporaryFile>
#include <QMessageBox>
#include <QtNetwork>
#include <QTime>
#include <QListWidget>

#include "waitingspinnerwidget.h"
#include "songitem.h"

namespace Ui {
class RecordingPage;
}

class RecordingPage : public QWidget
{
    Q_OBJECT

public:
    explicit RecordingPage(QWidget *parent = nullptr);
    ~RecordingPage();

public slots:
    void animate();
    void stopRecording();
    void startRecording(QString selectedDevice);
    void stopAllPlayers();
    void showHistory();
    void clearDebug();
    void deleteSelected();
    void showItemInfo();
    void cancelAllRequests();
signals:
    void back();
    void enableItemActions(bool enable =  false);
    void showPutKeyError(const QString error);
    void showFixKeyError(const QString error);
    void showJson(QString jsonPath);
private slots:
    void on_cancelButton_clicked();

    void createRecodingFile();
    void on_debugButton_clicked();

    void processRecording();
    void uploadFinished(bool fromHistory = false, QString historyItemFilePath = QString());
    void uploadProgress(qint64 byteSent, qint64 bytesTotal);
    QString withUnit(qreal bytes);
    void loadHistory();

    void on_resultListWidget_itemClicked(QListWidgetItem *item);


private:
    Ui::RecordingPage *ui;

    QString recordingFileName;
    QString historyPath;
    QList<qint64> processIdList;
    QString debug;

    QNetworkAccessManager *networkManager;
    QNetworkReply *networkReply;
    QTime m_timeAdded;

    WaitingSpinnerWidget *_loader = nullptr;

    QSettings settings;
};

#endif // RECORDINGPAGE_H

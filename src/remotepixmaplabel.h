#ifndef REMOTEPIXMAPLABEL_H
#define REMOTEPIXMAPLABEL_H


#include <QLabel>
#include <QtNetwork>

class RemotePixmapLabel: public QLabel
{
    Q_OBJECT
public:
    RemotePixmapLabel(QWidget* parent = 0) :
        QLabel(parent), networkManager_(0)
    {}
    RemotePixmapLabel(const QString& text, QWidget* parent = 0) :
        QLabel(text, parent), networkManager_(0)
    {}
    ~RemotePixmapLabel();

signals:
    void pixmapLoaded(QByteArray data);
    void pixmapLoading();

public slots:
    void setRemotePixmap(const QString&  url, QString fallbackUrlStr = "");
private slots:
    void pixmapReceived(QNetworkReply*);
    void showRetry(const QString urlStr);
    void deleteButtons();
private:
    QNetworkAccessManager* networkManager_= nullptr;
    QNetworkDiskCache *diskCache = nullptr;
    QString fallbackUrlStr;
};

#endif // REMOTEPIXMAPLABEL_H

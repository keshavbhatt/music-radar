#ifndef SONGITEM_H
#define SONGITEM_H

#include <QWidget>
#include <QMediaPlayer>
#include <QtNetwork>

namespace Ui {
class SongItem;
}

class SongItem : public QWidget
{
    Q_OBJECT

public:
    explicit SongItem(QWidget *parent = nullptr, QNetworkAccessManager *manager = nullptr);
    ~SongItem();

signals:
    void stopAllPlayers();

public slots:
    void init(QString artist = "Unknown",
              QString title  = "Unknown",
              QString album = "Unknown",
              QString release_date = "Unknown",
              QString iconUrl = "Unknown",
              QString preview_url = "Unknown",
              QString spotify_url = "Unknown");
    void animate();
private slots:
    void on_playButton_clicked();

    void on_youtubeButton_clicked();

    void on_spotifyButton_clicked();

private:
    Ui::SongItem *ui;
    QString preview_url, title, artist, spotify_url;
    QMediaPlayer *player = nullptr;
    QNetworkAccessManager *networkManager_ = nullptr;
};

#endif // SONGITEM_H

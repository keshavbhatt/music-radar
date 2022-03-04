#include "songitem.h"
#include "ui_songitem.h"

#include <QDesktopServices>
#include <QGraphicsOpacityEffect>
#include <QMessageBox>

SongItem::SongItem(QWidget *parent, QNetworkAccessManager *manager)
    : QWidget(parent), ui(new Ui::SongItem) {
  ui->setupUi(this);

  this->networkManager_ = manager;

  animate();
}

void SongItem::animate() {
  this->hide();
  QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
  this->setGraphicsEffect(eff);
  QPropertyAnimation *a = new QPropertyAnimation(eff, "opacity");
  a->setDuration(400);
  a->setStartValue(0);
  a->setEndValue(1);
  a->setEasingCurve(QEasingCurve::InCurve);
  a->start(QPropertyAnimation::DeleteWhenStopped);
  connect(a, &QPropertyAnimation::finished, this,
          [eff]() { eff->deleteLater(); });
  this->show();
}

void SongItem::init(QString artist, QString title, QString album,
                    QString release_date, QString iconUrl, QString preview_url,
                    QString spotify_url) {
  this->preview_url = preview_url;
  this->title = title;
  this->artist = artist;
  this->spotify_url = spotify_url;

  ui->cover->init(this->networkManager_, iconUrl,
                  "qrc:///icons/app/icon-256.png");
  ui->titleLabel->setText(title);
  ui->artistLabel->setText("by: " + artist);
  ui->albumLabel->setText("from: " + album + " (" + release_date + ")");

  if (preview_url.isEmpty() || preview_url == "Unknown")
    ui->playButton->hide();

  if (spotify_url.isEmpty() || spotify_url == "Unknown")
    ui->spotifyButton->hide();
}

SongItem::~SongItem() { delete ui; }

void SongItem::on_playButton_clicked() {

  if (player == nullptr) {
    player = new QMediaPlayer(this);
    player->setMedia(QUrl(this->preview_url));
    player->setVolume(100);
    connect(player, &QMediaPlayer::stateChanged, player,
            [=](QMediaPlayer::State state) {
              if (state == QMediaPlayer::StoppedState) {
                ui->playButton->setIcon(QIcon(":/icons/play-line.png"));
              }
              if (state == QMediaPlayer::PlayingState) {
                ui->playButton->setIcon(QIcon(":/icons/stop-line.png"));
              }
            });
    connect(player, &QMediaPlayer::mediaStatusChanged, player,
            [=](QMediaPlayer::MediaStatus mediastate) {
              if (mediastate == QMediaPlayer::LoadingMedia ||
                  mediastate == QMediaPlayer::BufferingMedia) {
                ui->playButton->setIcon(QIcon(":/icons/loader-2-fill.png"));
              }
              if (mediastate == QMediaPlayer::BufferedMedia) {
                ui->playButton->setIcon(QIcon(":/icons/stop-line.png"));
              }
            });

    connect(player, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
            player, [=](QMediaPlayer::Error error) {
              Q_UNUSED(error);
              ui->playButton->setIcon(QIcon(":/icons/play-line.png"));
              QMessageBox::critical(this, "Error", player->errorString());
            });
  }

  if (player->state() != QMediaPlayer::PlayingState) {
    emit stopAllPlayers();
    player->play();
  } else {
    player->blockSignals(true);
    player->stop();
    ui->playButton->setIcon(QIcon(":/icons/play-line.png"));
    player->blockSignals(false);
  }
}

void SongItem::on_youtubeButton_clicked() {
  QString term = title.replace(" ", "+") + ", " + artist.replace(" ", "+");
  QDesktopServices::openUrl(
      QUrl("https://www.youtube.com/results?search_query=" + term +
           "&sp=EgIQAQ%253D%253D"));
}

void SongItem::on_spotifyButton_clicked() {
  QDesktopServices::openUrl(QUrl(this->spotify_url));
}

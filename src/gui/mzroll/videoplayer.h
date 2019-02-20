#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QMediaPlayer>
#include <QWidget>

class QAbstractButton;
class QSlider;
class QLabel;
class QUrl;
class QCheckBox;
class QPushButton;
class QSettings;

class VideoPlayer : public QWidget
{
    Q_OBJECT
public:
    VideoPlayer(QSettings* settings, QWidget *parent = nullptr);
    ~VideoPlayer();

    void setUrl(const QUrl &url);

    virtual void closeEvent(QCloseEvent* event);

public slots:
    void play();

private slots:
    void mediaStateChanged(QMediaPlayer::State state);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void setPosition(int position);
    void setPosition();
    void mediaError(QMediaPlayer::Error error);

private:
    QLabel* m_title;
    QLabel* m_message;
    QLabel* m_extraText;
    QMediaPlayer* m_mediaPlayer;
    QAbstractButton *m_playButton;
    QWidget* m_hboxWidget;
    QWidget* m_vboxWidget;
    QCheckBox* m_checkBox;
    QPushButton* m_closeButton;
    QVideoWidget* m_vidWidget;
    QSlider *m_positionSlider;
    QLabel *m_errorLabel;
    QSettings* m_settings;
};

#endif

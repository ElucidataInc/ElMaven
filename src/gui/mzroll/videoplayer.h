#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QMediaPlayer>
#include <QWidget>
#include <QLabel>

class QAbstractButton;
class QSlider;
class QLabel;
class QUrl;
class QCheckBox;
class QPushButton;
class QSettings;
class MainWindow;

class VideoPlayer;
class UrlLabel: public QLabel
{
    Q_OBJECT
public:
    UrlLabel(VideoPlayer* parent);
    virtual void mousePressEvent(QMouseEvent* event);
signals:
    void clicked();
};

class VideoPlayer : public QWidget
{
    Q_OBJECT
public:
    VideoPlayer(QSettings* settings, MainWindow* mw, QWidget *parent = nullptr);
    ~VideoPlayer();

    void setUrl(const QUrl &url);

    virtual void closeEvent(QCloseEvent* event);
    virtual void showEvent(QShowEvent* event);

public slots:
    void play();

private slots:
    void mediaStateChanged(QMediaPlayer::State state);
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void setPosition(int position);
    void setPosition();
    void mediaError(QMediaPlayer::Error error);
    void linkClicked();

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
    UrlLabel* m_knowMoreLabel;
    QLabel *m_errorLabel;
    QSettings* m_settings;
    MainWindow* m_mainWindow;

    int playedVideoOnce;
    int linkClickedOnce;
};

#endif

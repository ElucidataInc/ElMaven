#include "videoplayer.h"
#include "mainwindow.h"

#include <QtWidgets>
#include <QVideoWidget>
#include <QSizePolicy>
#include <QSettings>
#include <QPushButton>


UrlLabel::UrlLabel(VideoPlayer* parent):
    QLabel(parent)
{
    setOpenExternalLinks(true);
    setText("<a href=\"https://polly.elucidata.io/prelogin/home\">Book a Demo</a>");
}

void UrlLabel::mousePressEvent(QMouseEvent* event)
{
    emit clicked();
}

VideoPlayer::VideoPlayer(QSettings *settings,MainWindow* mw, QWidget* parent)
    : QWidget(parent)
    , _settings(settings)
    , _mainWindow(mw)
{
    setWindowModality(Qt::WindowModal);
    QRect availableGeometry = QApplication::desktop()->availableGeometry();
    resize(availableGeometry.width()/1.7, availableGeometry.height()/1.3);
    setWindowTitle("New Machine learning Algorithm");
    QBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20,20,20,20);
    _title = new QLabel(this);
    _title->setText("<b>Make your analyses more insightful with ML. View your fluxomics workflow in PollyPhi</b>");
    _title->setContentsMargins(0,0,0,10);
    _title->setAlignment(Qt::AlignCenter);
    layout->addWidget(_title);

    _message = new QLabel(this);
    _message->setWordWrap(true);
    _message->setAlignment(Qt::AlignCenter);
    _message->setContentsMargins(0,0,0,20);
    _message->setText("Accurately classify groups as Good and Bad quickly with the new Machine Learning \
algorithm. Save time classifying data and spend more time analyzing it. ");
    layout->addWidget(_message);


    _extraText =new QLabel(this);
    _extraText->setText("Take a quick peek inside!");
    _extraText->setAlignment(Qt::AlignCenter);

    layout->addWidget(_extraText);

    _vidWidget = new QVideoWidget(this);

#if defined(__w64)
    _vidWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
#elif defined(__APPLE__)
    _vidWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
#endif

    _vidWidget->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(_vidWidget);

    layout->itemAt(0)->setAlignment(Qt::AlignCenter);


    _mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
    _mediaPlayer->setVideoOutput(_vidWidget);


    QBoxLayout* controlsLayout = new QHBoxLayout(this);

    _playButton = new QPushButton;
    _playButton->setEnabled(false);
    _playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    controlsLayout->addWidget(_playButton);

    _positionSlider = new QSlider(Qt::Horizontal);
    _positionSlider->setRange(0, 0);
    controlsLayout->addWidget(_positionSlider);

    layout->addLayout(controlsLayout);

    _knowMoreLabel = new UrlLabel(this);
    layout->addWidget(_knowMoreLabel);
    layout->itemAt(5)->setAlignment(Qt::AlignCenter);

    QBoxLayout* footerLayout = new QHBoxLayout(this);

    _checkBox = new QCheckBox(this);
    _checkBox->setText("Don't show this again");
    _checkBox->setChecked(_settings->value("hideVideoPlayer", 0).toBool());
    if(_checkBox->isChecked())
        _checkBox->setEnabled(false);
    footerLayout->addWidget(_checkBox);

    _closeButton = new QPushButton(this);
    _closeButton->setText("Close");



    footerLayout->addWidget(_closeButton);
    footerLayout->itemAt(1)->setAlignment(Qt::AlignRight);

    layout->addLayout(footerLayout);


    connect(_playButton, &QAbstractButton::clicked,
            this, &VideoPlayer::play);
    connect(_positionSlider, &QAbstractSlider::sliderMoved,
            this, static_cast<void (VideoPlayer::*)(int)>(&VideoPlayer::setPosition));
    connect(_positionSlider, &QAbstractSlider::sliderPressed,
            this, static_cast<void (VideoPlayer::*)()>(&VideoPlayer::setPosition));
    connect(_mediaPlayer, &QMediaPlayer::stateChanged,
            this, &VideoPlayer::mediaStateChanged);
    connect(_mediaPlayer, &QMediaPlayer::positionChanged, this, &VideoPlayer::positionChanged);
    connect(_mediaPlayer, &QMediaPlayer::durationChanged, this, &VideoPlayer::durationChanged);
    connect(_mediaPlayer,QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
            this, &VideoPlayer::mediaError);

    connect(_closeButton, &QPushButton::clicked, this , &VideoPlayer::close);

    connect(_knowMoreLabel, &UrlLabel::clicked, this, &VideoPlayer::linkClicked);

    _mediaPlayer->setPosition(0);

#if defined(Q_OS_MACOS) || defined(Q_OS_LINUX)
    setUrl(QUrl("qrc:/videos/PollyPhi-Workflow.mp4"));
#elif defined(Q_OS_WIN32)
    setUrl(QUrl("qrc:/videos/PollyPhi-Workflow.avi"));
#endif

    _playedVideoOnce = 0;
    _linkClickedOnce = 0;
}

VideoPlayer::~VideoPlayer()
{
}

void VideoPlayer::showEvent(QShowEvent* event)
{
    // avoid the empty/black screen when the widget shows
    _mediaPlayer->play();
    _mediaPlayer->pause();
}


void VideoPlayer::closeEvent(QCloseEvent *event)
{
    _settings->setValue("hideVideoPlayer", (int)_checkBox->isChecked());

    if(_checkBox->isChecked())
        _checkBox->setEnabled(false);

    _mediaPlayer->pause();
    _mediaPlayer->setPosition(0);

}

void VideoPlayer::linkClicked()
{
    if(!_linkClickedOnce) {
        std::cerr << "sending event for Know more prompt\n";
        _mainWindow->getAnalytics()->hitEvent("MLModel","ReadDocumentation");
        _linkClickedOnce++;
    }
}

void VideoPlayer::setUrl(const QUrl &url)
{
    setWindowFilePath(url.isLocalFile() ? url.toLocalFile() : QString());
    _mediaPlayer->setMedia(url);
    _playButton->setEnabled(true);
}

void VideoPlayer::play()
{
    if(!_playedVideoOnce) {
        // for every session we only want one event
        std::cerr << "sending event for Ml Video\n";
        _mainWindow->getAnalytics()->hitEvent("MLModel", "WatchedVideo");
        _playedVideoOnce++;
    }

    switch (_mediaPlayer->state()) {
    case QMediaPlayer::PlayingState:
        _mediaPlayer->pause();
        break;
    default:
        _mediaPlayer->play();

#if defined (__APPLE__)
        // BUG: Videos, on macOS,  dont scale according to the widget size. As a result videos get cut off from the widget
        // SOLUTION: Resize the video widget.
        // Kudos to https://stackoverflow.com/a/51736245/4069760 .
        QSize originalSize = _vidWidget->size();
        QSize newSize = originalSize + QSize(1,1);
        _vidWidget->resize(newSize);
        _vidWidget->resize(originalSize);
#endif

        break;
    }
}

void VideoPlayer::mediaError(QMediaPlayer::Error err)
{
    qDebug() << "error : " << err << " " << _mediaPlayer->errorString();
}

void VideoPlayer::mediaStateChanged(QMediaPlayer::State state)
{
    switch(state) {
    case QMediaPlayer::PlayingState:
        _playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    default:
        _playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    }
}

void VideoPlayer::positionChanged(qint64 position)
{
    _positionSlider->setValue(position);
}

void VideoPlayer::durationChanged(qint64 duration)
{
    _positionSlider->setRange(0, duration);
}

void VideoPlayer::setPosition()
{
    _mediaPlayer->setPosition(_positionSlider->value());
}
void VideoPlayer::setPosition(int position)
{
    _mediaPlayer->setPosition(position);
}
